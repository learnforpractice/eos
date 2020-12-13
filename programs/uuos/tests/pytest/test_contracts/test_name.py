from chain import *
def apply(a, b, c):
    print(type(a) == name, type(b) == name, type(c) == name)
    print('++++name:', a)
    assert a == name('alice')
    assert int(a) == s2n('alice')
    assert int(b) == s2n('alice')
    assert str(a) == 'alice'
    assert name('alice') == name(s2n('alice'))

    print(name('alice') , name(s2n('alice')))

    assert a.to_int() == s2n('alice')
    assert 'alice' == a.to_str()
    assert name('helloworld111') == name(s2n('helloworld111'))
    assert name('helloworld111').to_str() == 'helloworld111'
    assert name('helloworld111').to_int() == s2n('helloworld111')
    assert name(b'\x00\x00\x00\x00\x00\x1a\xa3j') == name('hello')
    assert name('hello').to_bytes() == b'\x00\x00\x00\x00\x00\x1a\xa3j'
    assert name.from_bytes(b'\x00\x00\x00\x00\x00\x1a\xa3j') == name('hello')
    assert name('hello').to_int() == s2n('hello')
    assert name('hello').to_str() == 'hello'
    assert bytes(name('hello')) == name('hello').to_bytes()

#    print(name('hello') == s2n('hello'))
