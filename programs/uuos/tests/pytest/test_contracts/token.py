# -*- coding: utf-8 -*-
import struct


def create():
    data = read_action_data()
    issuer, maximum_supply, symbol = struct.unpack('QQQ', data)
    token_create(issuer, maximum_supply, symbol)


def issue():
    data = read_action_data()
    to, quantity, symbol = struct.unpack('QQQ', data[:24])
    memo = data[25:]
#    token_issue(to, quantity, symbol, memo)
    token_issue(to, quantity, symbol, 'hello,world')


def transfer():
    data = read_action_data()
    _from, _to, quantity, symbol = struct.unpack('QQQQ', data[:32])
    memo = data[33:]
    token_transfer(_from, _to, quantity, symbol, memo)


def open():
    data = read_action_data()
    owner, symbol, ram_payer = struct.unpack('QQQ', data)
    token_open(owner, symbol, ram_payer)


def retire():
    data = read_action_data()
    amount, symbol = struct.unpack('QQ', data[:16])
    memo = data[17:]
    token_retire(amount, symbol, memo)


def close():
    data = read_action_data()
    owner, symbol = struct.unpack('QQ', data)
    token_close(owner, symbol)


def apply(receiver, code, action):
    if receiver != code:
        return
    if action == N('create'):
        create()
    elif action == N('issue'):
        issue()
    elif action == N('transfer'):
        transfer()
    elif action == N('open'):
        open()
    elif action == N('retire'):
        retire()
    elif action == N('close'):
        close()
