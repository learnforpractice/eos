import db
def apply(receiver, code, action):
    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    id = 'hello'
    data = read_action_data()
    if action == N('destroy'):
        while True:
            itr = db.end_i64(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.previous_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(itr, data)
            db.remove_i64(itr)
        print('destroy done!')
    elif action == N('init'):
        db.store_i64(scope, table, payer, 1, b'1')
        db.store_i64(scope, table, payer, 3, b'3')
        db.store_i64(scope, table, payer, 5, b'5')
        db.store_i64(scope, table, payer, 7, b'7')
        print('init done!')
    elif action == N('test'):
        itr = db.upperbound_i64(code, scope, table, 3)
        data = db.get_i64(itr)
        print('upperbound of 3:', data)
        assert data == b'5'

        itr = db.upperbound_i64(code, scope, table, 2)
        data = db.get_i64(itr)
        print('upperbound of 2:', data)
        assert data == b'3'

        itr = db.lowerbound_i64(code, scope, table, 3)
        data = db.get_i64(itr)
        print('lowerbound of 3:', data)
        assert data == b'3'

        itr = db.lowerbound_i64(code, scope, table, 2)
        data = db.get_i64(itr)
        print('lowerbound of 2:', data)
        assert data == b'3'
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.end_i64(code, scope, table)
        assert itr != -1
        while True:
            itr, primary = db.previous_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(itr, data)
    elif action == N('test2'):
        print('+++++++++test2')
        itr = db.lowerbound_i64(code, scope, table, 1)
        assert itr != -1
        data = db.get_i64(itr)
        print(data)
        expect_values = [b'1', b'3', b'5', b'7']
        while True:
            itr, primary = db.next_i64(itr)
            if itr < 0:
                break
            data = db.get_i64(itr)
            print(primary, data)
            assert expect_values[itr] == data, 'bad value'
    elif action == N('test3'):
        itr = db.find_i64(code, scope, table, 1)
        assert itr >= 0, 'value not found'
        value = db.get_i64(itr)
        assert value == b'1', 'bad value'
        db.update_i64(itr, payer, b'88')
        itr = db.find_i64(code, scope, table, 1)
        value = db.get_i64(itr)
        assert value == b'88', 'bad value'

        
