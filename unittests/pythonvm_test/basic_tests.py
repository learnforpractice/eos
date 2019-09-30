def apply(receiver, first_receiver, action):
    if action == N('test1'):
        print('hello, world')
    elif action == N('test2'):
        try:
            a = bytes(3*1024*1024)
        except MemoryError:
            pass
