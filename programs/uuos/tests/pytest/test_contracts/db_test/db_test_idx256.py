import db

def apply(receiver, code, action):
    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    data = read_action_data()
    if action == N('destroy'):
        while True:
            itr = db.idx256_end(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.idx256_previous(itr)
            if itr < 0:
                break
            print(itr, primary)
            db.idx256_remove(itr)
        print('destroy done!')
    elif action == N('init'):
        db.idx256_store(scope, table, payer, 2, 1<<128)
        db.idx256_store(scope, table, payer, 4, 3<<128)
        db.idx256_store(scope, table, payer, 6, 5<<128)
        db.idx256_store(scope, table, payer, 8, 7<<128)
        print('init done!')
    elif action == N('test'):
        itr, primary = db.idx256_upperbound(code, scope, table, 3<<128)
        print('upperbound of 3:', primary)
        assert primary == 6

        itr, primary = db.idx256_upperbound(code, scope, table, 2<<128)
        print('upperbound of 2:', primary)
        assert primary == 4

        itr, primary = db.idx256_lowerbound(code, scope, table, 3<<128)
        print('lowerbound of 3:', primary)
        assert primary == 4

        itr, primary = db.idx256_lowerbound(code, scope, table, 2<<128)
        print('lowerbound of 2:', primary)
        assert primary == 4
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.idx256_end(code, scope, table)
        assert itr != -1
        expect_values = [8, 6, 4, 2]
        while True:
            itr, primary = db.idx256_previous(itr)
            if itr < 0:
                break
            assert expect_values[itr] == primary
    elif action == N('test2'):
        print('+++++++++test2')
        itr, primary = db.idx256_lowerbound(code, scope, table, 1)
        assert itr != -1
        print(itr, primary)
        expect_values = [2, 4, 6, 8]
        while True:
            itr, primary = db.idx256_next(itr)
            if itr < 0:
                break
            print(itr, primary)
            assert expect_values[itr] == primary
    elif action == N('test3'):
        itr, secondary = db.idx256_find_primary(code, scope, table, 2)
        assert itr >= 0
        assert secondary == 1<<128

        itr, primary = db.idx256_find_secondary(code, scope, table, 1<<128)
        assert itr >= 0
        assert primary == 2

        db.idx256_update(itr, payer, 88<<128)
        itr, primary = db.idx256_find_secondary(code, scope, table, 1<<128)
        assert itr < 0

        itr, primary = db.idx256_find_secondary(code, scope, table, 88<<128)
        assert itr >= 0
        assert primary == 2
