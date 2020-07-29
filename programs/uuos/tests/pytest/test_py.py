import os
import time
import pytest
from uuos import application

logger = application.get_logger(__name__)

class AttrDict(dict):
    def __init__(self, *args, **kwargs):
        super(AttrDict, self).__init__(*args, **kwargs)
        self.__dict__ = self

class Test(object):

    @classmethod
    def setup_class(cls):
        pass

    @classmethod
    def teardown_class(cls):
        pass

    def setup_method(self, method):
        pass

    def teardown_method(self, method):
        pass

    def test_py(self):
        a = AttrDict({'hello':'world'})
        logger.info('%s', a)


