from _hello import *

chain_ptr = None
def get_info():
    return chain_get_info(chain_ptr)

def get_account(name):
    return chain_get_account(chain_ptr, name)
