import db

def apply(receiver, code, action):
    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    data = read_action_data()
    if action == N('destroy'):
        while True:
            itr = db.idx_long_double_end(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.idx_long_double_previous(itr)
            if itr < 0:
                break
            print(itr, primary)
            db.idx_long_double_remove(itr)
        print('destroy done!')
    elif action == N('init'):
        db.idx_long_double_store(scope, table, payer, 2, b'1'+b'0'*15)
        db.idx_long_double_store(scope, table, payer, 4, b'3'+b'0'*15)
        db.idx_long_double_store(scope, table, payer, 6, b'5'+b'0'*15)
        db.idx_long_double_store(scope, table, payer, 8, b'7'+b'0'*15)
        print('init done!')
    elif action == N('test'):
        itr, primary = db.idx_long_double_upperbound(code, scope, table, b'3'+b'0'*15)
        print('upperbound of 3:', primary)
        assert primary == 6

        itr, primary = db.idx_long_double_upperbound(code, scope, table, b'2'+b'0'*15)
        print('upperbound of 2:', primary)
        assert primary == 4

        itr, primary = db.idx_long_double_lowerbound(code, scope, table, b'3'+b'0'*15)
        print('lowerbound of 3:', primary)
        # assert primary == 4

        itr, primary = db.idx_long_double_lowerbound(code, scope, table, b'2'+b'0'*15)
        print('lowerbound of 2:', primary)
        assert primary == 4
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.idx_long_double_end(code, scope, table)
        assert itr != -1
        expect_values = [8, 6, 4, 2]
        while True:
            itr, primary = db.idx_long_double_previous(itr)
            if itr < 0:
                break
            assert expect_values[itr] == primary
    elif action == N('test2'):
        print('+++++++++test2')
        itr, primary = db.idx_long_double_lowerbound(code, scope, table, b'1'+b'0'*15)
        assert itr != -1
        print(itr, primary)
        expect_values = [2, 4, 6, 8]
        while True:
            itr, primary = db.idx_long_double_next(itr)
            if itr < 0:
                break
            print(itr, primary)
            assert expect_values[itr] == primary
    elif action == N('test3'):
        itr, secondary = db.idx_long_double_find_primary(code, scope, table, 2)
        assert itr >= 0
        assert secondary == b'1'+b'0'*15


        itr, secondary = db.idx_long_double_find_primary(code, scope, table, 4)
        assert itr >= 0
        assert secondary == b'3'+b'0'*15

        itr, primary = db.idx_long_double_find_secondary(code, scope, table, b'1'+b'0'*15)
        assert itr >= 0
        assert primary == 2

        db.idx_long_double_update(itr, payer, b'88'*8)

        itr, secondary = db.idx_long_double_find_primary(code, scope, table, 2)
        print(itr, secondary)

        itr, primary = db.idx_long_double_find_secondary(code, scope, table, b'1'+b'0'*15)
        assert itr < 0


        itr, primary = db.idx_long_double_find_secondary(code, scope, table, b'88'*8)
        print(itr, primary)
        assert itr >= 0
        assert primary == 2
