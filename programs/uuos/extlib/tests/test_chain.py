import os
import json
import pytest
import logging
import tempfile

from chaintest import ChainTest
import log
logger = log.get_logger(__name__)

class TestSystem(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        pass

    def test_1(self):
        args = {
            "account": 'eosio',
            "vmtype": 1,
            "vmversion": 0,
            "code": b'hello'.hex()
        }
        r = self.chain.push_action('eosio', 'setcode', args, {"eosio":'active'})
        logger.info(r)
