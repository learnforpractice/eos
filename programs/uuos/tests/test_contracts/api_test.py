def apply(receiver, code, action):
    if action == N('test1'):
        data = read_action_data()
        assert data == b'hello,world!'
    elif action == N('test2'):
        data = b'hello,world!'
        set_action_return_value(data)
    elif action == N('test3'):
        data = read_action_data()
        set_action_return_value(data)
    elif action == N('test4'):
        data = 'hello,world!'
        set_action_return_value(data)
