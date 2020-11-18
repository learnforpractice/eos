from chain import *
self_account = name('alice')
def apply(receiver, code, action):
    if action == name('senddefer'):
        print('senddeffer')
        tx = transaction_new(0, 0, 0, 0, 0, 10)

        action_id = action_new(self_account, name('dosomething'), self_account, name('active'), b'hello,worddddd')
        transaction_add_action(tx, action_id)

        payer = self_account
        transaction_send(tx, 1, payer, False)
    elif action == name('dosomething'):
        data = read_action_data()
        print(data)
