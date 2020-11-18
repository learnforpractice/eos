import db
from chain import *
self_account = name('alice')
def apply(receiver, code, action):
    if action == name('senddefer'):
        print('senddeffer')
        tx = transaction_new(0, 0, 0, 0, 0, 10)

        action_id = action_new(self_account, name('dosomething'), self_account, name('active'), b'hello,worddddd')
        transaction_add_action(tx, action_id)

        payer = self_account
        transaction_send(tx, 0x1ffffffffffffffff, payer, False)
    elif action == name('cancel'):
        r = transaction_cancel(0x1ffffffffffffffff)
        print(r)
        r = transaction_cancel(1)
        print(r)
        r = transaction_cancel(11)
        print(r)
    elif action == name('dosomething'):
        data = read_action_data()
        print(data)
