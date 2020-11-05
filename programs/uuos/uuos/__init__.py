__all__ = [
    'application',
    'blocklog',
    'chainapi',
    'config',
    'connection',
    'nativeobject',
    'p2pmanager',
    'rpcserver',
    'set_config',
    'recover_key',
    'current_time_nano',
    'sign_digest',
    'set_log_level',
    'set_default_log_level',
    'pack_native_object',
    'unpack_native_object',
    'db'
]

import sys
import importlib

class CustomImporter(object):
    def find_module(self, fullname, mpath=None):
        # if fullname in ['_uuos', '_vm_api']:
        if fullname in ['_uuos', '_vm_api', '_python_vm']:
            return self
        return

    def load_module(self, module_name):
        print('++++load_module:', module_name)
        mod = sys.modules.get(module_name)
        if mod is None:
            uuos_module = sys.modules.get('_uuos')
            if not uuos_module:
                return

            uuos_so = uuos_module.__file__
            spec = importlib.util.spec_from_file_location(module_name, uuos_so)
            mod = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(mod)
            sys.modules[module_name] = mod
        return mod

#sys.meta_path.insert(0, CustomImporter())
sys.meta_path.append(CustomImporter())

import _uuos
import _vm_api
import _python_vm

import ujson as json

class LogLevel:
    all = 0
    debug = 1
    info = 2
    warn = 3
    error = 4
    off  = 5

log_names = (
    'default',
    'net_plugin_impl',
    'http_plugin',
    'producer_plugin',
    'transaction_tracing'
)

def N(s):
    return _uuos.N(s)

def s2n(s):
    return _uuos.s2n(s)

def n2s(n):
    return _uuos.n2s(n)

def set_config(config):
    return _uuos.uuos_set_config(config)

def recover_key(digest, sign):
    return _uuos.uuos_recover_key(digest, sign)

def current_time_nano():
    return _uuos.uuos_current_time_nano()

def sign_digest(digest, priv_key):
    return _uuos.sign_digest(digest, priv_key)

def set_log_level(logger_name='default', level=10):
    return _uuos.set_log_level(logger_name, level)

def set_default_log_level(level):
    return _uuos.set_log_level('default', level)

def pack_native_object(obj_type, obj):
    return _uuos.pack_native_object(obj_type, obj)

def unpack_native_object(obj_type, packed_message):
    return _uuos.unpack_native_object(obj_type, packed_message)

def set_default_data_dir(dir):
    _uuos.set_default_data_dir(dir)

def set_default_config_dir(dir):
    _uuos.set_default_config_dir(dir)

def get_last_error():
    return _uuos.get_last_error()

def set_accepted_block_callback(cb):
    _uuos.set_accepted_block_callback(cb)

def initialize_logging(config_path):
    _uuos.uuos_initialize_logging(config_path)

def call_contract_off_chain(params):
    r = _uuos.uuos_call_contract_off_chain(params)
    return json.loads(r)

def run_py_func_safe(func, args):
    return _uuos.run_py_func_safe(func, args)

api = None
def init():
    global api
    ptr = _uuos.uuos_get_chain_ptr()
    from . import chainapi
    api = chainapi.ChainApi(ptr)
