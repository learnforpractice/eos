import os
import logging
import unittest

logging.basicConfig(filename='logfile.log', level=logging.INFO,
                    format='%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(module)s %(lineno)d %(message)s')
handler = logging.StreamHandler()
handler.setFormatter(formatter)

def get_logger(name):
    logger=logging.getLogger(name)
    logger.addHandler(handler)
    return logger

logger = get_logger(__name__)

class UUOSTester(unittest.TestCase):
    def __init__(self, testName, extra_args=[]):
        logger.info('+++++++++++++++++++++UUOSTester++++++++++++++++')
        super(UUOSTester, self).__init__(testName)

    def test1(self):
        logger.info('+++++++++++++test1+++++++++++++++')

    def test2(self):
        logger.info('+++++++++++++test2+++++++++++++++')

# class UUOSTester2(unittest.TestCase):
#     def __init__(self, testName, extra_args=[]):
#         logger.info('+++++++++++++++++++++UUOSTester2++++++++++++++++')
#         super(UUOSTester2, self).__init__(testName)

#     def test2(self):
#         logger.info('+++++++++++++test2+++++++++++++++')


if __name__ == '__main__':
    unittest.main()
