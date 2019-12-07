import os
import io
import time
import sys
import ujson as json
import struct
import logging
import asyncio

from . import chain, chain_api
from .native_object import *

logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

sync_req_span = 200

class Connection(object):
    def __init__(self, reader, writer, producer):
        self.reader = reader
        self.writer = writer
        self.handshake_count = 0
        self.last_handshake = None
        self.target = 0
        self.syncing = False
        self.chain_ptr = chain.get_chain_ptr()
        self.producer = producer
        self.closed = False
        self.catch_up = False

    async def read(self, length):
        buffer = io.BytesIO()
        while True:
            data = await self.reader.read(length)
            if not data:
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
        if msg_len >=500*1024 or msg_len < 2:
            logger.info(f'bad length: {msg_len}')
            return (None, None)

        msg_type = await self.read(1)
        if not msg_type:
            logger.error('fail to read msg type')
            return (None, None)
        msg_type = msg_type[0]

        msg = await self.read(msg_len-1)
        if not msg:
            logger.error('fail to read msg')
            return (None, None)
        return msg_type, msg

    def write(self, data):
        if not self.closed:
            self.writer.write(data)
    
    def close(self):
        self.writer.close()
        self.closed = True

    def fork_check(self):
        return False
        lib_num = chain.last_irreversible_block_num()
        msg = self.last_handshake
        if lib_num > msg.last_irreversible_block_num:
            if chain.get_block_id_for_num(lib_num) != msg.last_irreversible_block_id:
                sha256_empty = '0000000000000000000000000000000000000000000000000000000000000000'
                msg = dict(reason=GoAwayReason.forked, node_id=sha256_empty)
                msg = GoAwayMessage(msg)
                logger.info(f'\033[91m++++chain forked, close connection\033[0m')
                self.write(msg.pack())
                self.close()
                return True
        return False

    def notify_last_irr_catch_up(self):
        head_num = chain.fork_db_pending_head_block_num()
        lib_num = chain.last_irreversible_block_num()
        msg = {
            "known_trx":{
                "mode":2, #last_irr_catch_up
                "pending":lib_num,
                "ids":[

                ]
            },
            "known_blocks":{
                "mode":2, #last_irr_catch_up
                "pending":head_num,
                "ids":[

                ]
            }
        }
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    def notify_none(self):
        head_num = chain.fork_db_pending_head_block_num()
        lib_num = chain.last_irreversible_block_num()
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

    def send_notice_message(self, msg):
        logger.info(f'send notice message {msg}')
        msg = NoticeMessage(msg)
        self.writer.write(msg.pack())

    def send_block_by_num(self, num):
        block = chain.fetch_block_by_number(num)
        self.send_block(block)

    def send_block(self, block):
        msg_len = struct.pack('I', len(block) + 1)
        self.writer.write(msg_len)
        msg_type = b'\x07'#int.to_bytes(7, 1, 'little') #signed_block_message_type
        self.writer.write(msg_type)
        self.writer.write(block)

    async def send_block_task(self):
        while self.current_block <= self.last_sync_request.end_block:
            print("current_block:", self.current_block)
            self.send_block_by_num(self.current_block)
            self.current_block += 1
            await asyncio.sleep(0)

    def start_send_block(self):
        self.current_block = self.last_sync_request.start_block
        self.task = asyncio.create_task(self.send_block_task())
        logger.info(f'++++start_send_block {self.current_block}')
        print(self.last_sync_request.end_block)

        # self.last_sync_request.start_block
        # self.last_sync_request.end_block

    def send_handshake(self):
        self.handshake_count += 1
        msg = HandshakeMessage(default_handshake_msg)
        msg.network_version = 1206
        msg.chain_id = chain.id()
        num = chain.last_irreversible_block_num()
        msg.last_irreversible_block_num = num
        msg.last_irreversible_block_id = chain.get_block_id_for_num(num)
        num = chain.fork_db_pending_head_block_num()
        msg.head_num = num
        msg.head_id = chain.get_block_id_for_num(num)
        msg.generation = self.handshake_count
        msg.time = str(int(time.time()*1000000))
        logger.info(f'++++send handshake {msg}')
        msg = msg.pack()
        self.write(msg)

    def start_sync(self):
        print("chain.last_irreversible_block_num():", chain.last_irreversible_block_num())
        start_block = chain.last_irreversible_block_num() + 1
        end_block = start_block + sync_req_span
        if end_block > self.target:
            end_block = self.target
        logger.info(f"+++++start sync {start_block} {end_block}")
        self.sync_msg = SyncRequestMessage(start_block, end_block)
#        data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 1000000)
        self.write(self.sync_msg.pack())

    async def request_catch_up(self, msg):
        head_num = chain.fork_db_pending_head_block_num()
        for num in (self.last_handshake.head_num, head_num):
            logger.info(f'+++send catch up block {num}')
            self.send_block_by_num(num)
            asyncio.sleep(0)
        self.catch_up = True

    async def handle_message_loop(self):
        try:
            while await self.handle_message():
                pass
        except Exception as e:
            print(e)
            return

    async def handle_message(self):
        msg_type, msg = await self.read_message()
        if msg_type is None or msg is None:
            logger.error('Fail to read msg')
            self.close()
            return
        if msg_type == 0: #handshake_message_type
#                logger.info('bad handshake_message')
            self.last_handshake = msg = HandshakeMessage.unpack(msg)
            if not self.last_handshake:
                self.close()
                return
            print(self.last_handshake.head_num, chain.fork_db_pending_head_block_num(), chain.last_irreversible_block_num())
            print(self.last_handshake)
            head_num = chain.fork_db_pending_head_block_num()
            if msg.head_num < chain.last_irreversible_block_num():
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
                for num in range(msg.head_num+1, head_num+1):
                    block_id = chain.get_block_id_for_num(num)
                    notice_msg['known_blocks']['ids'].append(block_id)
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
            else: #self.last_handshake.head_num > chain.fork_db_pending_head_block_num():
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
            xmt = int(xmt)/1e6
            logger.info(msg)
            logger.info(time.localtime(xmt))
        elif msg_type == 4:
            msg = NoticeMessage.unpack(msg)
            pending = msg.known_blocks['pending']
            if pending > chain.last_irreversible_block_num():
                self.target = pending
                self.start_sync()
                #self.send_handshake(c)
            logger.info(f'receive notice message: {msg}')
            # msg = NoticeMessage({
            #     "known_trx":{
            #         "mode":0,
            #         "pending":0,
            #         "ids":[]
            #     },
            #     "known_blocks":{
            #         "mode":1,
            #         "pending":chain.fork_db_pending_head_block_num(),
            #         "ids":[]
            #     }
            # })
            # logger.info(msg)
            # self.send(msg.pack())
        elif msg_type == 5:#request_message_type
            msg = RequestMessage.unpack(msg)
            logger.info(msg)
            if msg.req_blocks['mode'] == 1: # request catch_up message
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
            print(msg)
            if msg.start_block == 0:
                return True
            self.last_sync_request = msg
            self.start_send_block()
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
#                logger.info(f"{num}, {block_id}")
#                num, block_id = chain_on_incoming_block(self.chain_ptr, msg)
            if num % 10000 == 0:
                logger.info(f"{num}, {block_id}")
            # if self.target - num < 1000:
            logger.info(f"{num}, {block_id}")
            logger.info(f"{self.sync_msg.end_block} {num}")
            if self.sync_msg.end_block == num:
                if self.target == num:
                    self.send_handshake()
                else:
                    self.start_sync()
        elif msg_type == 8:
            msg = PackedTransactionMessage(msg)
            logger.info(msg)
        return True
