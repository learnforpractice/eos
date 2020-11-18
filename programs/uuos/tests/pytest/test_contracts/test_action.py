import struct
from chain import *
def apply(a, b, c):
    print(a, b, c)
    if c == name('sayhello'):
        data = read_action_data()
        print(data)
    elif c == name('testaction'):
        a = action_new('alice', 'sayhello', 'alice', 'active', 'hello')
        action_send(a)
    elif c == name('sendinline'):
        account = name('alice')
        action = name('sayhello')
        actor = name('alice')
        permission = name('active')
        data = read_action_data()
        send_inline(account, action, actor, permission, data)
    elif c == name('sendraw'):
        account = name('alice').to_int()
        action = name('sayhello').to_int()
        actor = name('alice').to_int()
        permission = name('active').to_int()
        action = struct.pack('QQ', account, action) + struct.pack('B', 1) + struct.pack('QQ', actor, permission)
        data = b'hello,world'
        action += struct.pack('B', len(data)) + data
        send_inline_raw(action)
    elif c == name('contextfree'):
        account = name('alice')
        action = name('sayhello')
        actor = name('alice')
        permission = name('active')
        data = read_action_data()
        send_context_free_inline(account, action, data)
    elif c == name('contextraw'):
        account = name('alice').to_int()
        action = name('sayhello').to_int()
        actor = name('alice').to_int()
        permission = name('active').to_int()
        action = struct.pack('QQ', account, action) + struct.pack('B', 0)
        data = b'hello,world'
        action += struct.pack('B', len(data)) + data
        send_context_free_inline_raw(action)
