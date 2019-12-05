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
    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.handshake_count = 0
        self.last_handshake = None
        self.target = 0
        self.syncing = False
        self.chain_ptr = chain.get_chain_ptr()

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
        self.writer.write(data)
    
    def close(self):
        self.writer.close()

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

    async def send_block_task(self):
        while self.current_block <= self.last_sync_request.end_block:
            block = chain.fetch_block_by_number(self.current_block)
#            len(block) + 1
            msg_len = struct.pack('I', len(block) + 1)
            self.writer.write(msg_len)

            msg_type = int.to_bytes(7, 1, 'little') #signed_block_message_type
            self.writer.write(msg_type)
            self.writer.write(block)
            self.current_block += 1
            await asyncio.sleep(0)

    def start_send_block(self):
        self.current_block = self.last_sync_request.start_block
        self.task = asyncio.create_task(self.send_block_task())
        logger.info('++++start_send_block')

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
        start_block = chain.last_irreversible_block_num() + 1
        end_block = start_block + sync_req_span
        if end_block > self.target:
            end_block = self.target
        logger.info(f"+++++start sync {start_block} {end_block}")
        self.sync_msg = SyncRequestMessage(start_block, end_block)
#        data = struct.pack('IB', 8+1, sync_request_message_type) + struct.pack('II', 1, 1000000)
        self.write(self.sync_msg.pack())

    async def handle_message(self):
        try:
            msg_type, msg = await self.read_message()
            if msg_type is None or msg is None:
                logger.info('closed connection, exit')
                return
            if msg_type == 0: #handshake_message_type:
                msg = HandshakeMessage.unpack(msg)
                self.target = msg.head_num
                logger.info(f'+++receive handshake {msg}')
                self.last_handshake = msg
                if self.fork_check():
                    return
                if msg.head_num < chain.fork_db_pending_head_block_num():
#                    self.target = msg.head_num
                    self.notify_last_irr_catch_up()
                else: #self.last_handshake.head_num > chain.fork_db_pending_head_block_num():
                    self.target = self.last_handshake.head_num
                    self.start_sync()
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg)
                print(msg)
                self.writer.close()
                return
            else:
                logger.info('unexpected message: {msg_type}')
                self.writer.close()
                return
        except Exception as e:
            logger.exception(e)
            return
        count = 0
    #    block_file = open('block.bin', 'wb')
        while True:
            msg_type, msg = await self.read_message()
            if msg_type is None or msg is None:
                logger.error('Fail to read msg')
                self.writer.close()
                return
            count += 1
            # if count % 100 == 0:
            #     print('+++count:', count)

            # handshake_message_type = 0
            # chain_size_message_type = 1
            # go_away_message_type = 2
            # time_message_type = 3
            # notice_message_type = 4
            # request_message_type = 5
            # sync_request_message_type = 6
            # signed_block_message_type = 7
            # packed_transaction_message_type = 8
            # controller_config_type = 9
            if msg_type == 0: #handshake_message_type
#                logger.info('bad handshake_message')
                self.last_handshake = HandshakeMessage.unpack(msg)
                print(self.last_handshake)
                if self.last_handshake.head_num < chain.fork_db_pending_head_block_num():
#                    self.target = msg.head_num
                    self.notify_last_irr_catch_up()
                else: #self.last_handshake.head_num > chain.fork_db_pending_head_block_num():
                    self.target = self.last_handshake.head_num
                    self.start_sync()
            elif msg_type == 1: # chain_size_message_type
                pass
            elif msg_type == 2: # go_away_message_type
                msg = GoAwayMessage.unpack(msg)
                logger.info(msg)
                self.writer.close()
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
            elif msg_type == 6: #sync_request_message_type
                msg = SyncRequestMessage.unpack(msg)
                print(msg)
                if msg.start_block == 0:
                    continue
                self.last_sync_request = msg
                self.start_send_block()
            elif msg_type == 7:
                if False:
                    block = SignedBlockMessage.unpack(msg)
                    if not block:
                        continue
                        logger.info('bad block')
                        continue
                    block_num = bytes.fromhex(block.previous[:8])
                    block_num = int.from_bytes(block_num, 'big')
                    if block_num % 1 == 0:
                        print('++++block_num:', block_num)
                num, block_id = chain_on_incoming_block(self.chain_ptr, msg)
                if num % 10000 == 0:
                    logger.info(f"{num}, {block_id}")
                if self.target - num < 1000:
                    logger.info(f"{num}, {block_id}")
                if self.sync_msg.end_block == num:
                    if self.target == num:
                        self.send_handshake()
                    else:
                        self.start_sync()
            elif msg_type == 8:
                msg = PackedTransactionMessage(msg)
                logger.info(msg)
