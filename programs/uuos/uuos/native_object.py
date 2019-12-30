import ujson as json
import struct

from uuos.jsonobject import JsonObject
from _uuos import pack_native_object, unpack_native_object

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
producer_params_type = 10

class GoAwayReason:
    reason = {
        0: 'no_reason',     #  no reason to go away
        1: '_self',         #  the connection is to itself
        2: 'duplicate',     #  the connection is redundant
        3: 'wrong_chain',   #  the peer's chain id doesn't match
        4: 'wrong_version', #  the peer's network version doesn't match
        5: 'forked',        #  the peer's irreversible blocks are different
        6: 'unlinkable',    #  the peer sent a block we couldn't use
        7: 'bad_transaction',#  the peer sent a transaction that failed verification
        8: 'validation',    #  the peer sent a block that failed validation
        9: 'benign_other',  #  reasons such as a timeout. not fatal but warrant resetting
        10: 'fatal_other',  #  a catch-all for errors we don't have discriminated
        11: 'authentication'#  peer failed authenication
    }
    
    no_reason = 0
    _self = 1
    duplicate = 2
    wrong_chain = 3
    wrong_version = 4
    forked = 5
    unlinkable = 6
    bad_transaction = 7
    validation = 8
    benign_other = 9
    fatal_other = 10
    authentication = 11

    @classmethod
    def get_reason(cls, i):
        return cls.reason[i]

class IdListModes:
    modes = {
        0: 'none',
        1: 'catch_up',
        2: 'last_irr_catch_up',
        3: 'normal'
    }

    none = 0
    catch_up = 1
    last_irr_catch_up = 2
    normal = 3

    @classmethod
    def get_mode(cls, i):
        return cls.modes[i]

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

default_handshake_msg = {
    "network_version":1206,
    "chain_id":"e1b12a9d0720401efa34556d4cb80f0f95c3d0a3a913e5470e8ea9ff44719381",
    "node_id":"e62f00d1d06e03d972d1110308e836a7b171b212d819bc8cd069d9d5807574c7",
    "key":"EOS5vLqH3A65RYjiKGzyoHVg2jGHQFgTXK6Zco1qCt2oqMiCnsczH",
    "time":"1576317504842818000",
    "token":"e9274a7d36793dabca7ea5f710f5a7544da66a39c28488e599bb441f13408077",
    "sig":"SIG_K1_KXBbTMwB5sc7xQxTzVAhZRto28ZjGLNxJTLDjzWqEoZ57aM4R9rkP2fALWiKnQd46BHijyEBU4yt6BpKwwcCAsbfH7gTHB",
    "p2p_address":"127.0.0.1:9876 - e62f00d",
    "last_irreversible_block_num":5465618,
    "last_irreversible_block_id":"00536612d620c27dabb7f2653d7620f732118c7a2f6a68c6a5ec5813aa4c71da",
    "head_num":5465655,
    "head_id":"005366375db0ebbf499414d28fb689bf98a8b0a316a1c621ef37bd1f2a257d68",
    "os":"osx",
    "agent":"\"EOS Test Agent\"",
    "generation":1
}

class NativeMessage(JsonObject):

    def pack(self):
        msg = json.dumps(self._dict)
        msg = pack_native_object(self.obj_type, msg)
        return struct.pack('I', len(msg)+1) + struct.pack('B', self.obj_type) + msg

class HandshakeMessage(NativeMessage):
    obj_type = handshake_message_type

class ChainSizeMessage(NativeMessage):
    obj_type = chain_size_message_type

class GoAwayMessage(NativeMessage):
    obj_type = go_away_message_type

    @classmethod
    def unpack(cls, msg):
        msg = unpack_native_object(cls.obj_type, msg)
        msg = json.loads(msg)
        msg['reason'] = GoAwayReason.get_reason(msg['reason'])
        return cls(msg)

class TimeMessage(NativeMessage):
    obj_type = time_message_type

class NoticeMessage(NativeMessage):
    obj_type = notice_message_type

class RequestMessage(NativeMessage):
    obj_type = request_message_type

class SyncRequestMessage(NativeMessage):
    obj_type = sync_request_message_type
    def __init__(self, start_block=0, end_block=0):
        d = {'start_block':start_block, 'end_block':end_block}
        super(SyncRequestMessage, self).__init__(d)

    @classmethod
    def unpack(cls, msg):
        msg = unpack_native_object(cls.obj_type, msg)
        msg = json.loads(msg)
        return cls(**msg)

class SignedBlockMessage(NativeMessage):
    obj_type = signed_block_message_type

class PackedTransactionMessage(NativeMessage):
    obj_type = packed_transaction_message_type


class ControllerConfig(JsonObject):
    obj_type = controller_config_type

class ProducerParams(JsonObject):
    obj_type = producer_params_type

def pack_transaction(trx):
    return pack_native_object(packed_transaction_message_type, msg)

if __name__ == '__main__':
    h = HandshakeMessage({'a':1})
    h[123] = '456'
    print(h.pack())
