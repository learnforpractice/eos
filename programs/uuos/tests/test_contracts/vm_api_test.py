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

    # h = sha256('hello,world')
    # print(h)
    h = b'w\xdf&?I\x123V\xd2\x8aJ\x87\x15\xd2[\xf5\xb9\x80\xbe\xee\xb5\x03\xca\xb4n\xa6\x1a\xc9\xf32\x0e\xda'
    assert_sha256('hello,world', h)
