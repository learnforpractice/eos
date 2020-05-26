import ujson as json
from . import run_py_func_safe, get_last_error

def safe_runner(func):
    def runner(*args):
        ret, result = run_py_func_safe(func, args)
        if not ret:
            error = get_last_error()
            if error:
                error = json.loads(error)
            raise Exception(error)
        if isinstance(result, Exception):
            raise result
        return result
    return runner
