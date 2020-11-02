from chainlib import *
def apply(receiver, code, action):
    assert s2n('alice') == 3773036822876127232
    assert s2n('Alice') == 0
    assert s2n('Alice8') == 0

    try:
        s2n(123)
        raise 'should not go here'
    except TypeError:
        pass

    assert n2s(3773036822876127232) == 'alice'
    try:
        n2s(None)
        raise 'should not go here'
    except TypeError:
        pass

    require_recipient('bob')

    try:
        require_recipient(None)
        raise 'should not go here'
    except TypeError:
        pass

    require_auth(3773036822876127232)
    require_auth('alice')

    producers = get_active_producers()
    print(producers)
    assert producers == ('uuos', )

    require_auth('alice')
    try:
        require_auth(None)
    except TypeError:
        pass

    require_auth2('alice', 'active')
    require_auth2(b'alice', b'active')

    try:
        require_auth2(None, None)
    except TypeError:
        pass

    assert has_auth('active')
    assert has_auth(s2n('active'))
    try:
        has_auth(None)
    except TypeError:
        pass

    assert is_account('uuos')
    assert is_account(s2n('uuos'))
    try:
        is_account(None)
    except TypeError:
        pass

    t = publication_time()
    print('+++publication_time:', t)

    assert 'alice' == current_receiver()
    data = read_action_data()
    print(len(data), data)

    h = sha256('hello,world')
    assert_sha256('hello,world', h)
    itr = db_store_i64('alice', 'table', 'alice', 'hello', 'world')
    db_update_i64(itr, 0, b'abc')
