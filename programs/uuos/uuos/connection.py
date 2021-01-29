import os
import io
import time
import sys
import ujson as json
import struct
import logging
import asyncio
import hashlib
import random
import traceback
from datetime import datetime

from . import current_time_nano, sign_digest
from . import chain
from .nativeobject import (
    default_handshake_msg,
    HandshakeMessage,
    GoAwayMessage,
    NoticeMessage,
    TimeMessage,
    SyncRequestMessage,
    RequestMessage,
    SignedBlockMessage,
    PackedTransactionMessage
)

from .application import get_app, get_logger
from .producer import RawTransactionMessage
import uuos

logger=get_logger(__name__)

sync_req_span = 100
max_package_size = 5*1024*1024
max_time_interval = 30

net_messages = ("HANDSHAKE_MESSAGE",
    "CHAIN_SIZE_MESSAGE",
    "GO_AWAY_MESSAGE",
    "TIME_MESSAGE",
    "NOTICE_MESSAGE",
    "REQUEST_MESSAGE",
    "SYNC_REQUEST_MESSAGE",
    "SIGNED_BLOCK",         # which = 7
    "PACKED_TRANSACTION"  # which = 8
)

#    void connection::send_time(const time_message& msg) {
#       time_message xpkt;
#       xpkt.org = msg.xmt;
#       xpkt.rec = msg.dst;
#       xpkt.xmt = get_time();
#       enqueue(xpkt);
#    }
#       if(msg.org == 0)
#          {
#             c->send_time(msg);
#             return;  // We don't have enough data to perform the calculation yet.
#          }

DEBUG = False

def gen_node_id():
    aa = bytearray(32)
    for i in range(32):
        aa[i] = random.randint(0,255)
    return aa.hex()

g_node_id = gen_node_id()

class Connection(object):
    def __init__(self, host, port, client_mode=True):
        self.reader = None
        self.writer = None
        self.handshake_count = 0
        self.last_handshake = None
        self.target = 0
        self.syncing = False
        self.producer = get_app().producer
        self.closed = False
        self.catch_up = False
        self.host = host
        self.port = port
        self.sync_msg = None
        self.app = get_app()
        self.first_sync = True
        self.last_notice = None
        self.client_mode = client_mode

        self.time_counter = max_time_interval
        self.message_task = None
        self.sync_task = None
        self.cancel_sync_request = False
        self.time_message = None
        self.current_block = 0
        self.start_sync_block_num = 0
        self.received_block_count = 0
        self.start_time = time.time()

        self.org=0
        self.rec=0
        self.xmt=0
        self.dst=0

        self.chain = get_app().chain

    def status(self):
        ret = dict(peer=f'{self.host}:{self.port}', 
                    connecting=True,
                    syncing=self.syncing,
                    last_handshake = self.last_handshake._dict
            )
        return json.dumps(ret)

    def reset_time_counter(self):
        self.time_counter = max_time_interval

    async def connect(self):
        logger.info(f'++++++connect to {self.host}:{self.port}')
#        traceback.print_stack()
        try:
            self.reader, self.writer = await asyncio.open_connection(self.host, self.port, limit=1024*1024)
            return True
        except OSError as e:
            logger.info(f'Connect to {self.host}:{self.port} failed!')
#            logger.exception(e)
#            self.p2p_client_task.cancel()
        except Exception as e:
            logger.exception(e)
        return False

    async def start(self):
        logger.info('++++++++++connection start')
        self.send_handshake()
        msg_type = None
        raw_msg = None
        while True:
            msg_type, raw_msg = await self.read_message()
            # logger.info("%s %s", msg_type, raw_msg)
            if msg_type == 0:
                break
        handshake_message = HandshakeMessage.unpack(raw_msg)
        if not self.verify_handshake_message(handshake_message):
            self.close()
            return False
        ret = await self.handle_message(msg_type, raw_msg)
        # logger.info(f'handle_message return {ret}')
        if not ret:
            return False
        logger.info('+++++=create handle message loop task')
        self.message_task = asyncio.create_task(self.handle_message_loop())
        return True

    async def read(self, length):
        buffer = io.BytesIO()
        while True:
            data = await self.reader.read(length)
            if not data:
                logger.error('error reading data')
                self.close()
                return None
            buffer.write(data)
            length -= len(data)
            if length <= 0:
                break
        return buffer.getvalue()

    async def read_message(self):
        msg_len = await self.read(4)
        if not msg_len:
            return (None, None)
        msg_len = int.from_bytes(msg_len, 'little')
        if msg_len >= max_package_size or msg_len < 2:
            logger.info(f'bad length: {msg_len}')
            return (None, None)

        msg_type = await self.read(1)
        if not msg_type:
            logger.error('fail to read msg type')
            return (None, None)
        msg_type = msg_type[0]
        # logger.info(f'+++{msg_type}, {msg_len}')
        msg = await self.read(msg_len-1)
        if not msg:
            logger.error('fail to read msg')
            return (None, None)
        logger.info('++++receive message: %s', net_messages[msg_type])
        return msg_type, msg

    def write(self, data):
        if not self.closed:
            self.writer.write(data)
    
    def close(self):
        self.writer.close()
        self.closed = True

    def fork_check(self):
        return False
        lib_num = self.chain.last_irreversible_block_num()
        msg = self.last_handshake
        if lib_num > msg.last_irreversible_block_num:
            if self.chain.get_block_id_for_num(lib_num) != msg.last_irreversible_block_id:
                sha256_empty = '0000000000000000000000000000000000000000000000000000000000000000'
                msg = dict(reason=GoAwayReason.forked, node_id=sha256_empty)
                msg = GoAwayMessage(msg)
                logger.info(f'\033[91m++++chain forked, close connection\033[0m')
                self.write(msg.pack())
                self.close()
                return True
        return False

    def notify_last_irr_catch_up(self):
        head_num = self.chain.fork_db_pending_head_block_num()
        lib_num = self.chain.last_irreversible_block_num()
        msg = {
            "known_trx":{
                "mode":2, #last_irr_catch_up
                "pending":lib_num,
                "ids":[

                ]
            },
            "known_blocks":{
                "mode":2, #last_irr_catch_up
                "pending":lib_num,
                "ids":[

                ]
            }
        }
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    def notify_none(self):
        head_num = self.chain.fork_db_pending_head_block_num()
        lib_num = self.chain.last_irreversible_block_num()
        msg = {
            "known_trx":{
                "mode":1,
                "pending":0,
                "ids":[

                ]
            },
            "known_blocks":{
                "mode":0,
                "pending":0,
                "ids":[

                ]
            }
        }
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    def send_time_message(self, msg=None):
        if msg:
            msg = TimeMessage(msg)
        else:
            current_time = current_time_nano()
            self.org = current_time
            t = dict(org=self.org, rec=0, xmt=current_time, dst=0)
            msg = TimeMessage(msg)
        self.writer.write(msg.pack())

    def send_notice_message(self, msg):
        logger.info(f'send notice message {msg}')
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    def send_request_message(self, msg):
        msg = RequestMessage(msg)
        self.writer(msg.pack())
    #request_message_type

    def send_block_by_num(self, num):
        block = self.chain.fetch_block_by_number(num)
        if not block:
            return False
        self.send_block(block)
        return True

    def send_block(self, block):
        msg_len = struct.pack('I', len(block) + 1)
        self.writer.write(msg_len)
        msg_type = b'\x07'#int.to_bytes(7, 1, 'little') #signed_block_message_type
        self.writer.write(msg_type)
        self.writer.write(block)

    def send_transaction(self, raw_packed_trx):
        msg_len = struct.pack('I', len(raw_packed_trx) + 1)
        self.writer.write(msg_len)
        msg_type = b'\x08'#int.to_bytes(7, 1, 'little') #signed_block_message_type
        self.writer.write(msg_type)
        self.writer.write(raw_packed_trx)

    async def sync_blocks(self):
        current_block = self.last_sync_request.start_block
        while current_block <= self.last_sync_request.end_block:
#            print("current_block:", self.current_block)
            if not self.send_block_by_num(current_block):
                break
            current_block += 1
            await asyncio.sleep(0)

    def on_sync_request(self):
        self.sync_task = asyncio.create_task(self.sync_blocks())
        print(self.last_sync_request.end_block)

        # self.last_sync_request.start_block
        # self.last_sync_request.end_block

    def send_handshake(self):

        self.handshake_count += 1
        msg = HandshakeMessage(default_handshake_msg)
        msg.network_version = 1206
        msg.chain_id = get_app().chain.id()
        num = self.chain.last_irreversible_block_num()
        msg.last_irreversible_block_num = num
        msg.last_irreversible_block_id = self.chain.get_block_id_for_num(num)
        logger.info("+++send_handshake %s %s", num, msg.last_irreversible_block_id)
        if not msg.last_irreversible_block_id:
            msg.last_irreversible_block_id = (b'\x00'*32).hex()
            msg.last_irreversible_block_num = 0
        num = self.chain.fork_db_pending_head_block_num()
        msg.head_num = num
        msg.head_id = self.chain.get_block_id_for_num(num)
        if not msg.head_id:
            msg.head_num = 0
            msg.head_id = (b'\x00'*32).hex()
        msg.generation = self.handshake_count
#        msg.time = int(time.time()*1000000000)
        
        msg.node_id = g_node_id
        msg.p2p_address = get_app().config.p2p_listen_endpoint + " - " + msg.node_id[:8]
        peer_key = get_app().config.peer_private_key
        peer_key = json.loads(peer_key)
        if peer_key:
            peer_public_key = peer_key[0]
            peer_private_key = peer_key[1]
        else:
            peer_public_key = "EOS5vLqH3A65RYjiKGzyoHVg2jGHQFgTXK6Zco1qCt2oqMiCnsczH"
            peer_private_key = "5J8Jz3iicC4J9gCiUqZqJtNx3Q6Pa3BBaDmiu4GNeBUPDHmmrsm"
            peer_public_key = "EOS1111111111111111111111111111111114T1Anm"
            peer_private_key = "1111111111111111111111111111111111111"

        msg.key = peer_public_key

        handshake_time = int(time.time()*1e9) #uuos_current_time_nano()
        msg.time = str(handshake_time)

        if peer_key:
            h = hashlib.sha256()
            data = int.to_bytes(handshake_time, 8, 'little')
            h.update(data)
            msg.token = h.hexdigest()
            msg.sig = sign_digest(peer_private_key, h.digest())
        else:
            msg.token = '0000000000000000000000000000000000000000000000000000000000000000'
            msg.sig = 'SIG_K1_111111111111111111111111111111111111111111111111111111111111111116uk5ne'
        logger.info(f'++++send handshake {msg}')
        msg = msg.pack()
        self.write(msg)

    def verify_handshake_message(self, msg):
        config = get_app().config
        print(config.allowed_connection)
        if not config.allowed_connection:
            return True

        if 'none' in config.allowed_connection:
            return False

        if 'any' in config.allowed_connection:
            return True

        if 'specified' in config.allowed_connection:
            public_key = uuos.recover_key(msg.token, msg.sig)
            if public_key == msg.key and public_key in config.peer_key:
                return True

        if 'producers' in config.allowed_connection:
            if not get_app().producer.is_producer_key(msg.key):
                return False
            public_key = uuos.recover_key(msg.token, msg.sig)
            if public_key == msg.key and public_key in config.peer_key:
                return True

        logger.info(f'Peer {self.host} sent a handshake with an unauthorized key: {msg.key}.')
        return False

    def start_sync(self):
        # print("self.chain.last_irreversible_block_num():", self.chain.last_irreversible_block_num())
        if self.last_notice:
            pending = self.last_notice.known_blocks.pending
        else:
            pending = self.last_handshake.head_num
        
        irr_block_num = self.chain.last_irreversible_block_num()
        block_num = self.chain.fork_db_pending_head_block_num()
        logger.info(f'{irr_block_num}, {block_num}')
        # if self.start_sync_block_num == 0:
        #     self.start_sync_block_num = irr_block_num + 1

        if self.first_sync and irr_block_num < block_num:
            start_block = irr_block_num + 1
            end_block = block_num
            self.first_sync = False
        else:
            start_block = block_num + 1
            end_block = start_block + sync_req_span

        # start_block = self.start_sync_block_num
        # end_block = start_block + sync_req_span
        # self.start_sync_block_num = end_block

        # if end_block > self.last_handshake.head_num:
        #     end_block = self.last_handshake.head_num
        if end_block > pending:
            end_block = pending
        logger.info(f"+++++sync from {start_block} to {end_block}")
        self.sync_msg = SyncRequestMessage(start_block, end_block)
#        data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 1000000)
        self.write(self.sync_msg.pack())

    async def request_catch_up(self, msg):
        head_num = self.chain.fork_db_pending_head_block_num()
        for num in (self.last_handshake.head_num, head_num):
            logger.info(f'+++send catch up block {num}')
            self.send_block_by_num(num)
            asyncio.sleep(0)
        self.catch_up = True

    async def handle_message_loop(self):
        try:
            while True:
                msg_type, msg = await self.read_message()
                logger.info(f'+++++read message {msg_type} {len(msg)}')
                if not await self.handle_message(msg_type, msg):
                    break
        except Exception as e:
            logger.exception(e)
        logger.info('exit handle message loop')
        self.message_task = None
        self.close()

    async def handle_message(self, msg_type, msg):
        if msg_type is None or msg is None:
            logger.info('bad message')
            self.close()
            return
        self.time_counter = max_time_interval
        if msg_type == 0: #handshake_message_type
#                logger.info('bad handshake_message')
            self.last_handshake = msg = HandshakeMessage.unpack(msg)
            if not self.last_handshake:
                logger.info(f'unpack handshake message error!')
                self.close()
                return

            head_num = self.chain.fork_db_pending_head_block_num()
#            print(self.last_handshake.head_num, head_num, self.chain.last_irreversible_block_num())
            logger.info(f'+++handle message:{self.last_handshake}')
            if msg.head_num < self.chain.last_irreversible_block_num():
#                    self.target = msg.head_num
#                    self.notify_none()
                self.notify_last_irr_catch_up()
            elif msg.head_num < head_num:
                notice_msg = {
                    "known_trx":{
                        "mode":0,
                        "pending":0,
                        "ids":[

                        ]
                    },
                    "known_blocks":{
                        "mode":1,
                        "pending":head_num,
                        "ids":[
                        ]
                    }
                }
                block_id = self.chain.get_block_id_for_num(head_num)
                notice_msg['known_blocks']['ids'].append(block_id)
                # for num in range(msg.head_num+1, head_num+1):
                #     block_id = self.chain.get_block_id_for_num(num)
                #     notice_msg['known_blocks']['ids'].append(block_id)
                self.send_notice_message(notice_msg)
            elif msg.head_num == head_num:
                notice_msg = {
                    "known_trx":{
                        "mode":1,
                        "pending":0,
                        "ids":[

                        ]
                    },
                    "known_blocks":{
                        "mode":0,
                        "pending":0,
                        "ids":[

                        ]
                    }
                }
                self.send_notice_message(notice_msg)
                self.catch_up = True
            else: #self.last_handshake.head_num > self.chain.fork_db_pending_head_block_num():
                self.target = self.last_handshake.head_num
#                    self.start_sync()
        elif msg_type == 1: # chain_size_message_type
            pass
        elif msg_type == 2: # go_away_message_type
            msg = GoAwayMessage.unpack(msg)
            logger.info(msg)
            self.close()
            return
        elif msg_type == 3:
            msg = TimeMessage.unpack(msg)
            xmt = msg.xmt
            xmt = int(xmt)/1e9
            logger.info(msg)
            logger.info(time.localtime(xmt))
            current_time = current_time_nano()
            if msg.org == 0:
                t = dict(org=msg.xmt, rec=current_time, xmt=current_time, dst=0)
                self.send_time_message(t)
            self.time_message = msg
        elif msg_type == 4:
            msg = NoticeMessage.unpack(msg)
            pending = msg.known_blocks.pending
            self.last_notice = msg
            if pending > self.chain.fork_db_pending_head_block_num():
                self.target = pending
                logger.info(f'receive notice message: {msg}')
                self.start_sync()
                # request_msg = {
                #     "req_trx":{
                #         "mode":0,
                #         "pending":0,
                #         "ids":[

                #         ]
                #     },
                #     "req_blocks":{
                #         "mode":1,
                #         "pending":0,
                #         "ids":[

                #         ]
                #     }
                # }
                # self.send_request_message(request_msg)
                #self.send_handshake(c)
            # msg = NoticeMessage({
            #     "known_trx":{
            #         "mode":0,
            #         "pending":0,
            #         "ids":[]
            #     },
            #     "known_blocks":{
            #         "mode":1,
            #         "pending":self.chain.fork_db_pending_head_block_num(),
            #         "ids":[]
            #     }
            # })
            # logger.info(msg)
            # self.send(msg.pack())
        elif msg_type == 5:#request_message_type
            msg = RequestMessage.unpack(msg)
            logger.info(msg)
            if msg.req_blocks.mode == 1: # request catch_up message
                asyncio.create_task(self.request_catch_up(msg))
            # {
            #     "req_trx":{
            #         "mode":0,
            #         "pending":0,
            #         "ids":[

            #         ]
            #     },
            #     "req_blocks":{
            #         "mode":1,
            #         "pending":0,
            #         "ids":[

            #         ]
            #     }
            # }
        elif msg_type == 6: #sync_request_message_type
            msg = SyncRequestMessage.unpack(msg)
#            print('+++++sync request:', msg)
            if msg.start_block == 0:
                return True
            self.last_sync_request = msg
            if msg.start_block <= self.chain.fork_db_pending_head_block_num():
                if self.sync_task:
                    self.sync_task.cancel()
                    try:
                        await self.sync_task
                    except asyncio.CancelledError:
                        pass
                self.on_sync_request()
            else:
                logger.info(f"bad sync request message: {msg}")
        elif msg_type == 7:
            if False:
                block = SignedBlockMessage.unpack(msg)
                print(block)
                if not block:
                    return
                    logger.info('bad block')
                    return
                block_num = bytes.fromhex(block.previous[:8])
                block_num = int.from_bytes(block_num, 'big')
                print('++++block_num:', block_num)

            num, block_id = self.producer.on_incoming_block(msg)
            if num == 0:
                logger.info("something went wrong, ignore it!")
                return True
#                logger.info(f"{num}, {block_id}")
#                num, block_id = chain_on_incoming_block(self.chain_ptr, msg)
            if num % 10000 == 0:
                logger.info(f"{num}, {block_id}")
            self.received_block_count += 1
            if self.received_block_count % 100 == 0:
                t1 = time.gmtime(int(self.last_handshake.time)/1e9)
                t2 = time.gmtime()
                dt1 = datetime.fromtimestamp(time.mktime(t1))
                dt2 = datetime.fromtimestamp(time.mktime(t2))
                delta = dt2-dt1
                head_num = self.last_handshake.head_num + delta.seconds*2
                block_remains = head_num - num
                duration = time.time()-self.start_time

                if block_remains > 100:
                    bps = self.received_block_count/duration
                    logger.info('+++++BPS: %.2f, estimate remaining blocks: %d, estimate remain time: %02d:%02d:%02d'%(bps, block_remains, block_remains/bps/60/60, block_remains/bps%(60*60)/60, block_remains/bps%60))
                if duration >= 120.0:
                    logger.info('+++reset block counter')
                    self.received_block_count = 0
                    self.start_time = time.time()
            # if self.target - num < 1000:
            #logger.info(f"{num}, {block_id}")
            # if self.sync_msg:
            #     logger.info(f"{self.sync_msg.end_block} {num}")
            if self.sync_msg and self.sync_msg.end_block == num:
                # if self.last_handshake.head_num == num:
                #     self.send_handshake()
                if self.last_notice.known_blocks.pending == num:
                    self.send_handshake()
                else:
                    self.start_sync()
        elif msg_type == 8:
            if DEBUG:
                msg = PackedTransactionMessage(msg)
                logger.info(msg)
            msg = RawTransactionMessage(self, msg)
        return True
