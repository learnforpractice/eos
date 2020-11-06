def apply(a, b, c):
    #test for initialize bigint from small int
    try:
        bigint(-1)
        assert 0
    except ValueError:
        pass

    a = bigint(123)
    b = bigint(456)
    assert(int(a) == 123)
    assert(int(b) == 456)
    assert b > a
    assert a < b
    assert a != b

    print(bigint)
    print(a, b)
    print(a + b)
    print('b - a', a - b)
    print('int(b - a)', int(a - b))

    assert a != b
    assert a + b == bigint(579)
    assert b - a == bigint(333)
#    assert a - b == bigint(333)
    assert b * a == bigint(123*456)
    assert b / a == bigint(456//123)
    assert a / b == bigint(0)

    try:
        a / bigint(0)
        assert 0
    except ZeroDivisionError:
        pass

    a = bigint(0x0FFFFFFFFFFFFFFF)
    assert int(a) == 0x0FFFFFFFFFFFFFFF

    #test initialize bigint from str
    a = bigint('123')
    assert int(a) == 123

    #initialize bigint from hex
    a = bigint('0x123')
    assert int(a) == 0x123

    a = bigint('0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF')
    assert int(a) == 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
    print("bigint('0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF')", 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF)

    try:
        # larger than 258 bit integer
        a = bigint('0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF')
        assert 0
    except ValueError:
        pass


