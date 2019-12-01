from native_object import *
import ujson

import unittest


class TestSum(unittest.TestCase):

    def test_controller_config(self):
        cfg = ControllerConfig(config)
        packed_cfg = cfg.pack()
        print(packed_cfg)

    #    packed_cfg = bytes.fromhex('00000000000000322f55736572732f6e6577776f726c642f6465762f75756f73322f6275696c642f70726f6772616d732f64642f626c6f636b73312f55736572732f6e6577776f726c642f6465762f75756f73322f6275696c642f70726f6772616d732f64642f73746174650000004000000000000000080000000000004015000000000000200000000000881300000200000000000000010000d0c752936d05000002c0ded2bc1f1305fb0faac5e6c03ee3a1924234985427b6167ca569d13df435cf0000100000000000e8030000000008000c000000f40100001400000064000000400d0300e8030000f049020064000000100e00005802000080533b0000100000040006000100000000000000000000000000000000000000000000000000000000000000000000e8030000')
    #    print(packed_cfg)
        cfg2 = ControllerConfig.unpack(packed_cfg)
        print(cfg2)

        self.assertTrue(cfg == cfg2, 'shoud be True')

    def test_chain_new(self):
        cfg = ControllerConfig(config)
        # "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
        # "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
        cfg.blocks_dir = 'dd/blocks'
        cfg.state_dir = 'dd/state'
        cfg = ujson.dumps(cfg)
        ptr = chain_new(cfg, 'cd')
        print(ptr)
        chain_free(ptr)

if __name__ == '__main__':
    unittest.main()


