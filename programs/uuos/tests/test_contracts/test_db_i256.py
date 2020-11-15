from chain import *
def apply(a, b, c):
    code = 'alice'
    scope = 'alice'
    table = 'table11'
    payer = 'alice'
    key = 'hello'


    itr1 = db_store_i256(scope, table, payer, 1, '1')
    itr2 = db_store_i256(scope, table, payer, 3, '3')
    itr3 = db_store_i256(scope, table, payer, 5, '5')
    itr4 = db_store_i256(scope, table, payer, 7, '7')
    print(itr1, itr2, itr3, itr4)

    assert itr2 == db_upperbound_i256(code, scope, table, 1)
    assert itr1 == db_lowerbound_i256(code, scope, table, 1)


    if 0:
        itr1 = db_find_i256(code, scope, table, '1')
        print(itr1,)
        return

    ret = db_get_i256(itr1)
    print(itr1, ret)

    db_remove_i256(itr1)
    db_remove_i256(itr2)
    db_remove_i256(itr3)
    db_remove_i256(itr4)

