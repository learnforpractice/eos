import os
import time
import pytest
import logging
from chaintest import ChainTest

logging.basicConfig(filename='logfile.log', level=logging.INFO, format='%(asctime)s %(levelname)s %(lineno)d %(module)s %(message)s')
logger=logging.getLogger(__name__)

class Test(object):

    @classmethod
    def setup_class(cls):
        cls.chain = ChainTest(uuos_network=False)

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

    def setup_method(self, method):
        logger.info("starting execution of tc: {}".format(method.__name__))

    def teardown_method(self, method):
        logger.info("Ending execution of tc: {}".format(method.__name__))

    def test_create_account(self):
        self.chain.test_create_account()

    def test_hello_world(self):
        logger.info('hello,world')


#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

