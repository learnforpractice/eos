#-*- encoding: utf8
import struct
def recursive_call():
    recursive_call()

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
    elif action == N('test51'):
        data = read_action_data()
        n, start, end = struct.unpack('III', data)
        a = bytearray(n)
        for i in range(start, end):
            assert a[i] == 0
            a[i] = i % 256
            assert a[i] == i % 256
    elif action == N('test52'):
        recursive_call()
