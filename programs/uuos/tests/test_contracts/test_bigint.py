def apply(a, b, c):
    a = bigint(123)
    b = bigint(123)
    print(bigint, a, b, a + b, a == b, a==123)
    c1 = 0x0fffffffffffffff
    c2 = bigint(0x0fffffffffffffff)
    print(c1, c2, c1 == c2)
    print("++++bigint(b'\x01\x01')", bigint(b'12345'))

    try:
        a = 0xffffffffffffff
        b = 0xfffffffffffff
        print(a/b)
    except Exception as e:
        print(e)

    a = 0xffffffff
    b = 0xfffffff
    print(a/b)

    # 0xffffffffffffffffff00/0xffffffffffffffff00
    a = int.from_bytes(b'\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00', 'little')
    b = int.from_bytes(b'\x00\xff\xff\xff\xff\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00', 'little')

    print(type(a), a)
    print(type(b), b)

    print(1.1)
