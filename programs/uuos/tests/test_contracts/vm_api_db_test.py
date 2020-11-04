from chainlib import *
def apply(receiver, code, action):
    code = name('alice')
    scope = name('alice')
    table = name('table')
    payer = name('alice')
    key = name('hello')
    try:
        itr1 = db_store_i64(scope, table, payer, 1, 123)
        raise 'should not go here'
    except TypeError:
        pass

    try:
        itr1 = db_store_i64(None, table, payer, 1, 123)
        raise 'should not go here'
    except TypeError:
        pass

    itr1 = db_store_i64(scope, table, payer, 1, '1')
    itr2 = db_store_i64(scope, table, payer, 3, '3')
    itr3 = db_store_i64(scope, table, payer, 5, '5')
    itr4 = db_store_i64(scope, table, payer, 7, '7')
    itr5 = db_store_i64(scope, table, payer, 'hello', 'world')

    assert itr1 == db_find_i64(code, scope, table, 1)
    assert itr2 == db_find_i64(code, scope, table, 3)
    assert itr3 == db_find_i64(code, scope, table, 5)
    assert itr4 == db_find_i64(code, scope, table, 7)
    assert itr5 == db_find_i64(code, scope, table, 'hello')

    assert db_get_i64(itr1) == b'1'
    assert db_get_i64(itr2) == b'3'
    assert db_get_i64(itr3) == b'5'
    assert db_get_i64(itr4) == b'7'
    assert db_get_i64(itr5) == b'world'

    assert db_previous_i64(itr1) == (-1, 0)
    assert db_previous_i64(itr2) == (itr1, 1)
    assert db_previous_i64(itr3) == (itr2, 3)
    assert db_previous_i64(itr4) == (itr3, 5)
    assert db_previous_i64(itr5) == (itr4, 7)

    assert db_next_i64(itr1) == (itr2, 3)
    assert db_next_i64(itr2) == (itr3, 5)
    assert db_next_i64(itr3) == (itr4, 7)
    assert db_next_i64(itr4) == (itr5, s2n('hello'))
    assert db_next_i64(itr5) == (-2, 0)

    assert itr1 == db_upperbound_i64(code, scope, table, 0)
    assert itr2 == db_upperbound_i64(code, scope, table, 1)
    assert itr2 == db_upperbound_i64(code, scope, table, 2)

    assert itr3 == db_upperbound_i64(code, scope, table, 3)
    assert itr3 == db_upperbound_i64(code, scope, table, 4)

    assert itr4 == db_upperbound_i64(code, scope, table, 5)
    assert itr4 == db_upperbound_i64(code, scope, table, 6)


    assert itr1 == db_lowerbound_i64(code, scope, table, 0)
    assert itr1 == db_lowerbound_i64(code, scope, table, 1)
    
    assert itr2 == db_lowerbound_i64(code, scope, table, 2)
    assert itr2 == db_lowerbound_i64(code, scope, table, 3)

    assert itr3 == db_lowerbound_i64(code, scope, table, 4)
    assert itr3 == db_lowerbound_i64(code, scope, table, 5)

    assert itr4 == db_lowerbound_i64(code, scope, table, 6)
    assert itr4 == db_lowerbound_i64(code, scope, table, 7)

    itr_end = db_end_i64(code, scope, table)
    assert db_previous_i64(itr_end) == (itr5, s2n('hello'))



    db_update_i64(itr1, 0, b'11')
    assert db_get_i64(itr1) == b'11'

    db_remove_i64(itr1)
    assert db_find_i64(code, scope, table, 1) < 0

    assert db_get_i64(itr2) == b'3'
    assert db_get_i64(itr3) == b'5'
    assert db_get_i64(itr4) == b'7'
    assert db_get_i64(itr5) == b'world'

    itrs_64 = (itr2, itr3, itr4, itr5)


#+++++++++++++++++++db i256++++++++++++++++++++++++++++++++++
    itr1 = db_store_i256(scope, table, payer, '1', '1')
    itr2 = db_store_i256(scope, table, payer, '3', '3')
    itr3 = db_store_i256(scope, table, payer, '5', '5')
    itr4 = db_store_i256(scope, table, payer, '7', '7')
    itr5 = db_store_i256(scope, table, payer, 'hello', 'world')
    print(itr1, itr2, itr3, itr4, itr5)
    
    assert itr1 == db_find_i256(code, scope, table, '1')
    assert itr2 == db_find_i256(code, scope, table, '3')
    assert itr3 == db_find_i256(code, scope, table, '5')
    assert itr4 == db_find_i256(code, scope, table, '7')
    assert itr5 == db_find_i256(code, scope, table, 'hello')

    assert db_get_i256(itr1) == b'1'
    assert db_get_i256(itr2) == b'3'
    assert db_get_i256(itr3) == b'5'
    assert db_get_i256(itr4) == b'7'
    assert db_get_i256(itr5) == b'world'

    assert db_previous_i256(itr1) == (-1, b'\x00'*32)
    assert db_previous_i256(itr2) == (itr1, b'1'+b'\x00'*31)
    assert db_previous_i256(itr3) == (itr2, b'3'+b'\x00'*31)
    assert db_previous_i256(itr4) == (itr3, b'5'+b'\x00'*31)
    assert db_previous_i256(itr5) == (itr4, b'7'+b'\x00'*31)

    assert db_next_i256(itr1) == (itr2, b'3'+b'\x00'*31)
    assert db_next_i256(itr2) == (itr3, b'5'+b'\x00'*31)
    assert db_next_i256(itr3) == (itr4, b'7'+b'\x00'*31)
    assert db_next_i256(itr4) == (itr5, b'hello'+b'\x00'*27)
    assert db_next_i256(itr5) == (-2, b'\x00'*32)


    assert itr1 == db_upperbound_i256(code, scope, table, b'0')
    assert itr2 == db_upperbound_i256(code, scope, table, b'1')
    assert itr2 == db_upperbound_i256(code, scope, table, b'2')

    assert itr3 == db_upperbound_i256(code, scope, table, b'3')
    assert itr3 == db_upperbound_i256(code, scope, table, b'4')

    assert itr4 == db_upperbound_i256(code, scope, table, b'5')
    assert itr4 == db_upperbound_i256(code, scope, table, b'6')


    assert itr1 == db_lowerbound_i256(code, scope, table, b'0')
    assert itr1 == db_lowerbound_i256(code, scope, table, b'1')
    
    assert itr2 == db_lowerbound_i256(code, scope, table, b'2')
    assert itr2 == db_lowerbound_i256(code, scope, table, b'3')

    assert itr3 == db_lowerbound_i256(code, scope, table, b'4')
    assert itr3 == db_lowerbound_i256(code, scope, table, b'5')

    assert itr4 == db_lowerbound_i256(code, scope, table, b'6')
    assert itr4 == db_lowerbound_i256(code, scope, table, b'7')



    itr_end = db_end_i256(code, scope, table)
    assert db_previous_i256(itr_end) == (itr5, b'hello'+b'\x00'*27)

    db_update_i256(itr1, 0, b'11')
    assert db_get_i256(itr1) == b'11'

    db_remove_i256(itr1)
    assert db_find_i256(code, scope, table, b'1') < 0

    assert db_get_i256(itr2) == b'3'
    assert db_get_i256(itr3) == b'5'
    assert db_get_i256(itr4) == b'7'
    assert db_get_i256(itr5) == b'world'

    assert 8 == db_get_table_count(code, scope, table)

#++++++++++cleanup++++++++++++++++++++++++
    db_remove_i256(itr2)
    db_remove_i256(itr3)
    db_remove_i256(itr4)
    db_remove_i256(itr5)

    for itr in itrs_64:
        db_remove_i64(itr)


    itr0 = db_idx64_store(scope, table, payer, 1, 11)
    itr1 = db_idx64_store(scope, table, payer, 3, 33)
    itr2 = db_idx64_store(scope, table, payer, 5, 55)
    itr3 = db_idx64_store(scope, table, payer, 7, 77)

    assert (1, 3) == db_idx64_next(itr0)
    assert (-1, 0) == db_idx64_previous(itr0)
    assert (2, 5) == db_idx64_next(itr1)
    assert (0, 1) == db_idx64_previous(itr1)

    assert (2, 5) == db_idx64_previous(itr3)


    assert (0, 11) == db_idx64_find_primary(code, scope, table, 1)
    assert (1, 33) == db_idx64_find_primary(code, scope, table, 3)
    assert (2, 55) == db_idx64_find_primary(code, scope, table, 5)
    assert (3, 77) == db_idx64_find_primary(code, scope, table, 7)

    assert (0, 1) == db_idx64_find_secondary(code, scope, table, 11)
    assert (1, 3) == db_idx64_find_secondary(code, scope, table, 33)
    assert (2, 5) == db_idx64_find_secondary(code, scope, table, 55)
    assert (3, 7) == db_idx64_find_secondary(code, scope, table, 77)


    assert (0, 1) == db_idx64_lowerbound(code, scope, table, 11)
    assert (1, 3) == db_idx64_upperbound(code, scope, table, 11)

    assert (-2, 0) == db_idx64_lowerbound(code, scope, table, 88)
    assert (-2, 0) == db_idx64_upperbound(code, scope, table, 88)


    secondary0 = 0x0102030405060708090a0b0c0d0e0f11
    secondary1 = 0x0102030405060708090a0b0c0d0e0f33
    secondary2 = 0x0102030405060708090a0b0c0d0e0f55
    secondary3 = 0x0102030405060708090a0b0c0d0e0f77

    itr0 = db_idx128_store(scope, table, payer, 1, secondary0)
    itr1 = db_idx128_store(scope, table, payer, 3, secondary1)
    itr2 = db_idx128_store(scope, table, payer, 5, secondary2)
    itr3 = db_idx128_store(scope, table, payer, 7, secondary3)

    assert itr0 == 0
    assert itr1 == 1
    assert itr2 == 2
    assert itr3 == 3

    assert (0, secondary0) == db_idx128_find_primary(code, scope, table, 1)
    assert (1, secondary1) == db_idx128_find_primary(code, scope, table, 3)
    assert (2, secondary2) == db_idx128_find_primary(code, scope, table, 5)
    assert (3, secondary3) == db_idx128_find_primary(code, scope, table, 7)

    assert (0, 1) == db_idx128_find_secondary(code, scope, table, secondary0)
    assert (1, 3) == db_idx128_find_secondary(code, scope, table, secondary1)
    assert (2, 5) == db_idx128_find_secondary(code, scope, table, secondary2)
    assert (3, 7) == db_idx128_find_secondary(code, scope, table, secondary3)


    assert (0, 1) == db_idx128_lowerbound(code, scope, table, secondary0)
    assert (1, 3) == db_idx128_upperbound(code, scope, table, secondary0)

    ret = db_idx128_lowerbound(code, scope, table, 0x0102030405060708090a0b0c0d0e0f78)
    assert (-2, 0) == db_idx128_lowerbound(code, scope, table, 0x0102030405060708090a0b0c0d0e0f78)
    assert (-2, 0) == db_idx128_upperbound(code, scope, table, 0x0102030405060708090a0b0c0d0e0f78)

    secondary_0_update = 0x0102030405060708090a0b0c0d0e0f111
    db_idx128_update(itr0, payer, secondary_0_update)
    assert (0, 1) == db_idx128_find_secondary(code, scope, table, secondary_0_update)
    assert (0, secondary_0_update) == db_idx128_find_primary(code, scope, table, 1)
