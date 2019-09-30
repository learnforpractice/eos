g_counter = 0
def apply(receiver, first_receiver, action):
    global g_counter

    if action == N('test1'):
        print('hello, world')
    elif action == N('test2'):
        try:
            a = bytes(3*1024*1024)
        except MemoryError:
            pass
    elif action == N('test3'):
        g_counter+=1
        eosio_assert(g_counter==1, "bad g_counter value")
