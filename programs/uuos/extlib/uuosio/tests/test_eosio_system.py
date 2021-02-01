import os
import json
import pytest
import logging
import tempfile

from chaintester import ChainTester
import log
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

    def test_1(self):
        _id = self.chain.c.get_block_id_for_num(1)
        logger.info(_id)

        _id = self.chain.c.id()
        logger.info(_id)

