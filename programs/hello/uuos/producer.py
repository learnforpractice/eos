import asyncio
from _hello import producer_new, producer_free
from .native_object import ProducerParams
import ujson as json
from . import chain

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

config = dict(
    producers = ["eosio"],
    signature_providers = ['EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV=KEY:5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'],
    greylist_account = [],
    producer_threads = 1,
    keosd_provider_timeout_us = 5000,
    snapshots_dir = 'snapshot',
    max_transaction_time_ms = 30,
    max_irreversible_block_age = -1,
    produce_time_offset_us = 0,
    last_block_time_offset_us = 0,
    max_scheduled_transaction_time_per_block_ms = 100,
    subjective_cpu_leeway_us = 31000, # default subjective cpu leeway in microseconds
    incoming_defer_ratio = 1.0,
    greylist_limit = 1000,
)

class Producer(object):
    def __init__(self):
        cfg = json.dumps(config)
        self.ptr = producer_new(chain.chain_ptr, cfg)

    def __delete__(self):
        producer_free(self.ptr)

    def get_sleep_time(self):
        return 1.0

    def start_produce_block(self):
        pass

    def end_produce_block(self):
        pass

    def can_produce_block(self):
        return False

    def on_incoming_block(self, block):
        pass

    async def run(self):
        while True:
            if not self.can_produce_block():
                await asyncio.sleep(0.2)

            self.start_produce_block()
            sleep_time = self.get_sleep_time()
            await asyncio.sleep(sleep_time)
            self.end_produce_block()

