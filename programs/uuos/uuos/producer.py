import asyncio
from .application import get_app, Application

from _uuos import (
    producer_new,
    producer_free,
    producer_on_incoming_block,
    producer_start_block,
    producer_calc_pending_block_time,
    producer_calc_pending_block_deadline_time,
    producer_maybe_produce_block,
    producer_now_time,
    producer_get_pending_block_mode,
    producer_process_incomming_transaction,
    producer_process_raw_transaction,
    producer_create_snapshot
)

from .native_object import ProducerParams
import ujson as json
from . import chain

import logging
logger=logging.getLogger(__name__)
logger.addHandler(logging.StreamHandler())

'''
   struct producer_params {
      vector<string> producers;
      vector<string> signature_providers;
      vector<string> greylist_account;
      int producer_threads;
      int keosd_provider_timeout_us;
      string snapshots_dir;

      int32_t   max_transaction_time_ms;
      int32_t   max_irreversible_block_age;
      int32_t   produce_time_offset_us;
      int32_t   last_block_time_offset_us;
      int32_t   max_scheduled_transaction_time_per_block_ms;
      int32_t   subjective_cpu_leeway_us;
      double    incoming_defer_ratio;
      uint32_t  greylist_limit;
   };
{
   "_producers":["eosio"],
   "_keosd_provider_timeout_us":5000,
   "_produce_time_offset_us":0,
   "_last_block_time_offset_us":0,
   "_max_scheduled_transaction_time_per_block_ms":100,
   "_max_transaction_time_ms":30,
   "_max_irreversible_block_age_us":-1000000,
   "_incoming_defer_ratio":"1.00000000000000000"
}
'''

g_producer_config = dict(
    producers = [],
    signature_providers = ['EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV=KEY:5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'],
    greylist_account = [],
    producer_threads = 1,
    keosd_provider_timeout_us = 5000,
    snapshots_dir = 'snapshots',
    max_transaction_time_ms = 30,
    max_irreversible_block_age = -1,
    produce_time_offset_us = 0,
    last_block_time_offset_us = 0,
    max_scheduled_transaction_time_per_block_ms = 100,
    subjective_cpu_leeway_us = 31000, # default subjective cpu leeway in microseconds
    incoming_defer_ratio = 1.0,
    greylist_limit = 1000,
    production_enabled = False,
)

class StartBlockResult:
    succeeded = 0
    failed = 1
    waiting = 2
    exhausted = 3

class PendingBlockMode:
   producing = 0
   speculating = 1

block_interval_ms = 500
block_interval_us = block_interval_ms*1000



class Hub(object):

    def __init__(self):
        self.subscriptions = set()

    def publish(self, message):
        for queue in self.subscriptions:
            queue.put_nowait(message)

class Subscription(object):

    def __init__(self, hub):
        self.hub = hub
        self.queue = asyncio.Queue()
        self.hub.subscriptions.add(self.queue)

    def get_queue(self):
        return self.queue

    def __enter__(self):
        self.hub.subscriptions.add(self.queue)
        return self.queue

    def __exit__(self, type, value, traceback):
        self.hub.subscriptions.remove(self.queue)

class Message(object):
    type_transaction = 0
    type_raw_transaction = 1
    def __init__(self, message_type, data):
        self.type = message_type
        self.data = data
        self.event = asyncio.Event()
        self.result = None

    def get_data(self):
        return self.data

    def publish(self):
        get_app().producer.publish_message(self)

    def notify(self, result):
        self.result = result
        self.event.set()

    async def wait(self):
        await self.event.wait()
        return self.result

class TransactionMessage(Message):

    def __init__(self, packed_trx):
        super().__init__(Message.type_transaction, packed_trx)
        get_app().producer.publish_message(self)

class RawTransactionMessage(Message):

    def __init__(self, packed_trx):
        super().__init__(Message.type_raw_transaction, packed_trx)
        get_app().producer.publish_message(self)


class Producer(object):
    def __init__(self, config):
        print('+++producer:', config.enable_stale_production)
        g_producer_config['production_enabled'] = config.enable_stale_production
        g_producer_config['producers'] = config.producer_name
        g_producer_config['snapshots_dir'] = config.snapshots_dir
        g_producer_config['data_dir'] = config.data_dir
        
        cfg = json.dumps(g_producer_config)
        self.ptr = producer_new(chain.chain_ptr, cfg)
        self.config = config

        self.pending_trx = {}

        self.hub = Hub()
        self.subs = Subscription(self.hub)
        self.trx_queue = self.subs.get_queue()

#        self.task = asyncio.create_task(self.handle_message())


    def publish_message(self, msg):
        self.hub.publish(msg)

    async def handle_message(self):
        with Subscription(self.hub) as queue:
            while True:
                msg = await queue.get()
#                logger.info(f'process message {msg}')
                try:
                    if msg.type == Message.type_transaction:
                        self.start_block()
                        ret, result, raw_packed_trx = self.process_incomming_transaction(msg.data.decode('utf8'))
                        #TODO: check failure of process transaction
                        msg.notify(result)
                        # continue
                        if ret == 0:
                            continue
                        for c in get_app().connections:
                            c.send_transaction(raw_packed_trx)
                    elif msg.type == Message.type_raw_transaction:
                        if chain.is_building_block():
                            ret, result = self.process_raw_transaction(msg.data)
                            msg.notify(result)
                            if ret == 0:
                                continue
                            for c in get_app().connections:
                                c.send_transaction(msg.data)
                except Exception as e:
                    logger.exception(e)
        logger.info('+++++++++++++handle message returned!')
        raise Exception('handle message return')

    def process_trx(self, msg):
#        logger.info(f'process message {msg}')
        try:
            if msg.type == Message.type_transaction:
                self.start_block()
                ret, result, raw_packed_trx = self.process_incomming_transaction(msg.data.decode('utf8'))
                #TODO: check failure of process transaction
                msg.notify(result)
                # continue
                if ret == 0:
                    return
                for c in get_app().connections:
                    c.send_transaction(raw_packed_trx)
            elif msg.type == Message.type_raw_transaction:
                if chain.is_building_block():
                    ret, result = self.process_raw_transaction(msg.data)
                    if not ret:
                        print(result)
                    msg.notify(result)
                    if ret == 0:
                        return
                    for c in get_app().connections:
                        c.send_transaction(msg.data)
        except Exception as e:
            logger.exception(e)

    async def run(self):
        # if not self.config.producer_name:
        #     return
        while True:
            result = self.start_block()
#            logger.info(f'+++++++++++++{result}')
            if result == 0 or result == 3: #succeeded or exhausted
                while not self.trx_queue.empty():
                    msg = await self.trx_queue.get()
                    self.process_trx(msg)

                mode = self.get_pending_block_mode()
                if mode == 0: #producing
                    deadline = self.calc_pending_block_deadline_time()
                    delay = deadline - self.now_time()
                    print('++++++delay:',delay/1e6)
                    await asyncio.sleep(delay/1e6)
                    self.maybe_produce_block()
                    continue
#                    deadline = self.calc_pending_block_deadline_time()
#                    delay = deadline - self.now_time()

#                    task = asyncio.create_task(self.produce_block(delay))
            elif result == 1: #failed
                pass
                # await asyncio.sleep(0.5)
            elif result == 2: #waiting
                pass
                # await asyncio.sleep(0.5)
            elif result == 3: #exhausted
                pass
            await asyncio.sleep(0.5)
        # while True:
        #     if not self.can_produce_block():
        #         await asyncio.sleep(0.2)

        #     self.start_produce_block()
        #     sleep_time = self.get_sleep_time()
        #     await asyncio.sleep(sleep_time)
        #     self.end_produce_block()

    def __delete__(self):
        producer_free(self.ptr)

    def get_sleep_time(self):
        return 1.0

    async def produce_block(self, delay):
        while True:
            await asyncio.sleep(delay/1e6)
            self.maybe_produce_block()
            deadline = self.calc_pending_block_deadline_time()
            delay = deadline - self.now_time()

    def start_produce_block(self):
        pass

    def end_produce_block(self):
        pass

    def can_produce_block(self):
        return False

    def start_block(self):
        return producer_start_block(self.ptr)

    def calc_pending_block_time(self):
        return producer_calc_pending_block_time(self.ptr)

    def calc_pending_block_deadline_time(self):
        return producer_calc_pending_block_deadline_time(self.ptr)

    def maybe_produce_block(self):
        return producer_maybe_produce_block(self.ptr)

    def now_time(self):
        return producer_now_time()

    def get_pending_block_mode(self):
        return producer_get_pending_block_mode(self.ptr)

    def process_incomming_transaction(self, trx):
        return producer_process_incomming_transaction(self.ptr, trx)
    
    def process_raw_transaction(self, raw_packed_trx):
        return producer_process_raw_transaction(self.ptr, raw_packed_trx)

    def start_production_loop(self):
        pass

    def on_incoming_block(self, block):
        num, block_id = producer_on_incoming_block(self.ptr, block)
        self.start_production_loop()
        return num, block_id
    
    def create_snapshot(self):
        return producer_create_snapshot(self.ptr)


