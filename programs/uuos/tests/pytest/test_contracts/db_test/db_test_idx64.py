

# int32_t db_idx64_store(uint64_t scope, table_name table, uint64_t payer, uint64_t id, const uint64_t* secondary);
# void db_idx64_update(int32_t iterator, uint64_t payer, const uint64_t* secondary);
# void db_idx64_remove(int32_t iterator);
# int32_t db_idx64_next(int32_t iterator, uint64_t* primary);
# int32_t db_idx64_previous(int32_t iterator, uint64_t* primary);
# int32_t db_idx64_find_primary(uint64_t code, uint64_t scope, table_name table, uint64_t* secondary, uint64_t primary);
# int32_t db_idx64_find_secondary(uint64_t code, uint64_t scope, table_name table, const uint64_t* secondary, uint64_t* primary);
# int32_t db_idx64_lowerbound(uint64_t code, uint64_t scope, table_name table, uint64_t* secondary, uint64_t* primary);
# int32_t db_idx64_upperbound(uint64_t code, uint64_t scope, table_name table, uint64_t* secondary, uint64_t* primary);
# int32_t db_idx64_end(uint64_t code, uint64_t scope, table_name table);

import db
def apply(receiver, code, action):
    code = receiver
    scope = 'scope'
    table = 'table'
    payer = receiver
    data = read_action_data()
    if action == N('destroy'):
        while True:
            itr = db.idx64_end(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.idx64_previous(itr)
            if itr < 0:
                break
            print(itr, primary)
            db.idx64_remove(itr)
        print('destroy done!')
    elif action == N('init'):
        db.idx64_store(scope, table, payer, 2, 1)
        db.idx64_store(scope, table, payer, 4, 3)
        db.idx64_store(scope, table, payer, 6, 5)
        db.idx64_store(scope, table, payer, 8, 7)
        print('init done!')
    elif action == N('test'):
        itr, primary = db.idx64_upperbound(code, scope, table, 3)
        print('upperbound of 3:', primary)
        assert primary == 6

        itr, primary = db.idx64_upperbound(code, scope, table, 2)
        print('upperbound of 2:', primary)
        assert primary == 4

        itr, primary = db.idx64_lowerbound(code, scope, table, 3)
        print('lowerbound of 3:', primary)
        assert primary == 4

        itr, primary = db.idx64_lowerbound(code, scope, table, 2)
        print('lowerbound of 2:', primary)
        assert primary == 4
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.idx64_end(code, scope, table)
        assert itr != -1
        expect_values = [8, 6, 4, 2]
        while True:
            itr, primary = db.idx64_previous(itr)
            if itr < 0:
                break
            assert expect_values[itr] == primary
    elif action == N('test2'):
        print('+++++++++test2')
        itr, primary = db.idx64_lowerbound(code, scope, table, 1)
        assert itr != -1
        print(itr, primary)
        expect_values = [2, 4, 6, 8]
        while True:
            itr, primary = db.idx64_next(itr)
            if itr < 0:
                break
            print(itr, primary)
            assert expect_values[itr] == primary
    elif action == N('test3'):
        itr, secondary = db.idx64_find_primary(code, scope, table, 2)
        assert itr >= 0
        assert secondary == 1

        itr, primary = db.idx64_find_secondary(code, scope, table, 1)
        assert itr >= 0
        assert primary == 2

        db.idx64_update(itr, payer, 88)
        itr, primary = db.idx64_find_secondary(code, scope, table, 1)
        assert itr < 0

        itr, primary = db.idx64_find_secondary(code, scope, table, 88)
        assert itr >= 0
        assert primary == 2

        # itr = db.find_i64(code, scope, table, 1)
        # assert itr >= 0, 'value not found'
        # value = db.get_i64(itr)
        # assert value == b'1', 'bad value'
        # db.update_i64(itr, payer, b'88')
        # itr = db.find_i64(code, scope, table, 1)
        # value = db.get_i64(itr)
        # assert value == b'88', 'bad value'