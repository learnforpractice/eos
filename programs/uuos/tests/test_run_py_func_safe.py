import os
import sys
import json

test_dir = os.path.dirname(__file__)
sys.path.append(os.path.join(test_dir, '..'))

import _uuos

def f(a , b, c):
    print(a, b, c)

def safe_runner(func):
    def runner(*args):
        ret, result = _uuos.run_py_func_safe(func, args)
        if not ret:
            error = _uuos.uuos_get_last_error()
            if error:
                error = json.loads(error)
            raise Exception(error)
        return result
    return runner

@safe_runner
def say_hello(a, b, c):
    print('hello', a, b, c)
    _uuos._db.get_i64(1)
    print('hello,world')

class Runner(object):
    
    def __init__(self):
        pass
    
    @safe_runner
    def say_hello(self, a, b, c):
        print(a, b, c)

say_hello(1, 2, 3)

error = _uuos.uuos_get_last_error()
print(error)

runner = Runner()
runner.say_hello(4, 5, 6)

print('goodbye, world')
