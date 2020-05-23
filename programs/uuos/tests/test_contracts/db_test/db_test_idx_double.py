

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
            itr = db.idx_double_end(code, scope, table)
            if itr == -1:
                break
            itr, primary = db.idx_double_previous(itr)
            if itr < 0:
                break
            print(itr, primary)
            db.idx_double_remove(itr)
        print('destroy done!')
    elif action == N('init'):
        db.idx_double_store(scope, table, payer, 1, 1.0)
        db.idx_double_store(scope, table, payer, 3, 3.0)
        db.idx_double_store(scope, table, payer, 5, 5.0)
        db.idx_double_store(scope, table, payer, 7, 7.0)
        print('init done!')
    elif action == N('test'):
        itr, primary = db.idx_double_upperbound(code, scope, table, 3.0)
        print('upperbound of 3:', primary)
        assert primary == 5

        itr, primary = db.idx_double_upperbound(code, scope, table, 2.0)
        print('upperbound of 2:', primary)
        assert primary == 3

        itr, primary = db.idx_double_lowerbound(code, scope, table, 3.0)
        print('lowerbound of 3:', primary)
        assert primary == 3

        itr, primary = db.idx_double_lowerbound(code, scope, table, 2.0)
        print('lowerbound of 2:', primary)
        assert primary == 3
    elif action == N('test1'):
        print('+++++++++test1')
        itr = db.idx_double_end(code, scope, table)
        assert itr != -1
        expected_values = [7, 5, 3, 1]
        while True:
            itr, primary = db.idx_double_previous(itr)
            if itr < 0:
                break
            print(itr, primary)
            assert expected_values[itr] == primary
    elif action == N('test2'):
        print('+++++++++test2')
        itr, primary = db.idx_double_lowerbound(code, scope, table, 1.0)
        assert itr != -1
        print(itr, primary)
        expected_values = [1, 3, 5, 7]
        while True:
            itr, primary = db.idx_double_next(itr)
            if itr < 0:
                break
            print(itr, primary)
            assert expected_values[itr] == primary
    elif action == N('test3'):
        itr, primary = db.idx_double_find_secondary(code, scope, table, 3.0)
        assert itr >= 0
        assert primary == 3

        db.idx_double_update(itr, payer, 88.0)
        itr, primary = db.idx_double_find_secondary(code, scope, table, 3.0)
        assert itr < 0

        itr, primary = db.idx_double_find_secondary(code, scope, table, 88.0)
        assert itr >= 0
        assert primary == 3
