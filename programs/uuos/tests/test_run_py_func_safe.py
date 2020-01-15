import os
import sys
import json
import traceback

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import uuos
from uuos import db
from uuos.saferunner import safe_runner

def f(a , b, c):
    print(a, b, c)


@safe_runner
def say_hello(a, b, c):
    print('hello', a, b, c)
    db.get_i64(1)
    print('hello,world')

class Runner(object):
    
    def __init__(self):
        pass
    
    @safe_runner
    def say_hello(self, a, b, c):
        print(a, b, c)

    @safe_runner
    def say_hello2(self):
        raise Exception('goodbye,world')

    @safe_runner
    @safe_runner
    def say_hello3(self):
        raise Exception('goodbye,world')


try:
    say_hello(1, 2, 3)
except Exception as e:
    print(e)

error = uuos.get_last_error()
print(error)

runner = Runner()
runner.say_hello(4, 5, 6)

try:
    runner.say_hello2()
except Exception as e:
    traceback.print_exc()

try:
    runner.say_hello3()
except Exception as e:
    traceback.print_exc()

print('goodbye, world')
