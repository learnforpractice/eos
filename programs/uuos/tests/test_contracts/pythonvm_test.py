#-*- encoding: utf8
def apply(receiver, code, action):
    if action == N('test1'):
        print('hello,world!')
    elif action == N('test2'):
        print('你好，世界')
    elif action == N('test3'):
        data = read_action_data()
        print(data)
        print(data.decode('utf8'))
    elif action == N('test4'):
        while True: pass
    elif action == N('test5'):
        a = bytes(8*1024*1024)
