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

    def test_create_account(self):
        self.chain.test_create_account()

    @classmethod
    def teardown_class(cls):
        cls.chain.free()

#./uuos/uuos -m pytest ../../programs/uuos/tests/test.py::Test::test_create_account

