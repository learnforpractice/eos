import os
import json
import pytest
import logging
import tempfile

from chaintester import ChainTester
from uuosio import log, uuos
logger = log.get_logger(__name__)

class TestSystem(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTester()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.warning('test start: %s', method.__name__)

    def teardown_method(self, method):
        pass

    def test_pack_abi(self):
        abi = '''
{
    "version": "eosio::abi/1.0",
    "types": [],
    "structs": [],
    "actions": [{
        "name": "sayhello",
        "type": "string",
        "ricardian_contract": ""
    }],
    "tables": [],
    "ricardian_clauses": [],
    "error_messages": [],
    "abi_extensions": []
}
'''
        abi = uuos.pack_abi(abi)
        logger.info(abi)


