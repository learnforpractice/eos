import ujson as json
import struct

from _hello import *

message_header_size = 4

handshake_message_type = 0
chain_size_message_type = 1
go_away_message_type = 2
time_message_type = 3
notice_message_type = 4
request_message_type = 5
sync_request_message_type = 6
signed_block_message_type = 7
packed_transaction_message_type = 8
controller_config_type = 9

default_handshake_msg = {
    "network_version":1206,
    "chain_id":"cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f",
    "node_id":"74988beaf865bfc19e94d6b44340bec658f57e0318fbe62a03a917b0bd78b03a",
    "key":"EOS1111111111111111111111111111111114T1Anm",
    "time":"1575022351028286000",
    "token":"0000000000000000000000000000000000000000000000000000000000000000",
    "sig":"SIG_K1_111111111111111111111111111111111111111111111111111111111111111116uk5ne",
    "p2p_address":"127.0.0.1:9877 - 74988be",
    "last_irreversible_block_num":8987,
    "last_irreversible_block_id":"0000231b11661fc9673f59812b4bda6a3b8276cbe3ac3d1fad2760ff8e3787cc",
    "head_num":8988,
    "head_id":"0000231cf256f921b3d26e369451410107bd1a31ce7827f5fb3c1e1ae70bece8",
    "os":"osx",
    "agent":"\"EOS Test Agent\"",
    "generation":1
}

default_config = {
    "sender_bypass_whiteblacklist": [],
    "actor_whitelist": [],
    "actor_blacklist": [],
    "contract_whitelist": [],
    "contract_blacklist": [],
    "action_blacklist": [],
    "key_blacklist": [],
    "blocks_dir": "/Users/newworld/dev/uuos2/build/programs/dd/blocks",
    "state_dir": "/Users/newworld/dev/uuos2/build/programs/dd/state",
    "state_size": 1073741824,
    "state_guard_size": 134217728,
    "reversible_cache_size": 356515840,
    "reversible_guard_size": 2097152,
    "sig_cpu_bill_pct": 5000,
    "thread_pool_size": 2,
    "read_only": False,
    "force_all_checks": False,
    "disable_replay_opts": False,
    "contracts_console": False,
    "allow_ram_billing_in_notify": False,
    "disable_all_subjective_mitigations": False,
    "uuos_mainnet": True,
    "genesis_accounts_file": "",
    "genesis": {
        "initial_timestamp": "2018-06-01T12:00:00.000",
        "initial_key": "EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV",
        "initial_configuration": {
            "max_block_net_usage": 1048576,
            "target_block_net_usage_pct": 1000,
            "max_transaction_net_usage": 524288,
            "base_per_transaction_net_usage": 12,
            "net_usage_leeway": 500,
            "context_free_discount_net_usage_num": 20,
            "context_free_discount_net_usage_den": 100,
            "max_block_cpu_usage": 200000,
            "target_block_cpu_usage_pct": 1000,
            "max_transaction_cpu_usage": 150000,
            "min_transaction_cpu_usage": 100,
            "max_transaction_lifetime": 3600,
            "deferred_trx_expiration_window": 600,
            "max_transaction_delay": 3888000,
            "max_inline_action_size": 4096,
            "max_inline_action_depth": 4,
            "max_authority_depth": 6
        }
    },
    "wasm_runtime": "wabt",
    "read_mode": "SPECULATIVE",
    "block_validation_mode": "FULL",
    "db_map_mode": "mapped",
    "db_hugepage_paths": [],
    "resource_greylist": [],
    "trusted_producers": [],
    "greylist_limit": 1000
}

def normal_setattr(self, attr, value):
    self._dict[attr] = value

def custom_setattr(self, attr, value):
    if attr == '_dict':
        type(self).old_setattr(self, attr, value)
    else:
        if not attr in self._dict:
            raise AttributeError(attr)
        self._dict[attr] = value

def custom_getattr(self, attr):
    return self._dict[attr]

class NativeObject(object):
    def __init__(self, msg_dict):
#        super(NativeObject, self).__init__(msg_dict)
        self._dict = msg_dict
        if hasattr(NativeObject, 'old_setattr'):
            pass
        else:
            NativeObject.old_setattr = NativeObject.__setattr__
            NativeObject.__setattr__ = custom_setattr

    def __getattr__(self, attr):
        return self._dict[attr]

    def dumps(self):
        return json.dumps(self._dict)

    def pack(self):
        msg = json.dumps(self._dict)
        return pack_native_object(self.obj_type, msg)

    @classmethod
    def unpack(cls, msg):
        msg = unpack_native_object(cls.obj_type, msg)
        msg = json.loads(msg)
        return cls(msg)

    def __str__(self):
        return json.dumps(self._dict, sort_keys=False, indent=4)

    def __repr__(self):
        return json.dumps(self._dict, sort_keys=False, indent=4)

class NativeMessage(NativeObject):

    def pack(self):
        msg = json.dumps(self._dict)
        msg = pack_native_object(self.obj_type, msg)
        return struct.pack('I', len(msg)+1) + struct.pack('B', self.obj_type) + msg

class HandshakeMessage(NativeMessage):
    obj_type = handshake_message_type

class SyncRequestMessage(NativeMessage):
    obj_type = sync_request_message_type
    def __init__(self, start_block=0, end_block=0):
        d = dict(start_block=start_block, end_block=end_block)
        super(SyncRequestMessage, self).__init__(d)
        self.__dict__ = self

class TimeMessage(NativeMessage):
    obj_type = time_message_type

class NoticeMessage(NativeMessage):
    obj_type = notice_message_type

class SignedBlockMessage(NativeMessage):
    obj_type = signed_block_message_type

class ControllerConfig(NativeObject):
    obj_type = controller_config_type


if __name__ == '__main__':
    h = HandshakeMessage({'a':1})
    h[123] = '456'
    print(h.pack())
