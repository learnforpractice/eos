import os
import time
import json
from uuosio.chaintester import ChainTester
from uuosio import log, uuos
logger = log.get_logger(__name__)

print(os.getpid())
input('<<<')

class TestMicropython(object):

    @classmethod
    def setup_class(cls):
        cls.tester = ChainTester()

    @classmethod
    def teardown_class(cls):
        cls.tester.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        self.tester.produce_block()

    def test_native_contract(self):
        uuos.enable_native_contracts(True)
        eosio_contract = 'build/libraries/vm_api/test/libnative_eosio_system.dylib'
        ret = uuos.set_native_contract(uuos.s2n('eosio'), eosio_contract)
        assert ret

        self.tester.buy_ram_bytes('hello', 'hello', 10*1024*1024)

        uuos.enable_native_contracts(False)
        uuos.set_native_contract(uuos.s2n('eosio.mpy'), '')

