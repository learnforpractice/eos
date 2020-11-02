from chainlib import *
def apply(receiver, code, action):
    itr = db_store_i64('alice', 'table', 'alice', 'hello', 'world')
    db_update_i64(itr, 0, b'abc')
