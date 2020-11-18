def float_equal(a, b):
    return abs(a - b) < 0.000000001

def apply(a, b, c):
    #test for initialize bigint from small int
    print(float128)
    print(float128(1.0), float128(1))

    a = float128(5.0) * float128(3.5)
    b = float128(17.5)
    assert float_equal(a, b)

    a = float128(5.0) / float128(3.5)
    b = float128(1.4285714285714286)
    assert float_equal(a, b)

    print('+++float128(18.5)/float128(3.5):', float128(18.5)//float128(3.5))
    assert float_equal(float128(18.5)//float128(3.5), float128(5.0))

    print(float128(5.0) * 3.5)
    print(3.5 * float128(5.0))
    print(float128(18446744073709551615.5) * float128(5.5))
    print(18446744073709551615.5)

    assert float_equal(float128(4722366482869645213695.0/2), 2361183241434822606848.0)

