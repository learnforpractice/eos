
import imp
import sys
import importlib

uuos_so = '/Users/newworld/dev/uuos3/build/programs/pyeos//_uuos.cpython-37m-darwin.so'

class CustomImporter(object):
    def find_module(self, fullname, mpath=None):
        if fullname in ['_uuos', '_vm_api']:
            return self
        return
    def load_module(self, fullname):
        print('++++load_module:', fullname)
        mod = sys.modules.get(fullname)
        if mod is None:
            mod = imp.load_dynamic(fullname, uuos_so)
            sys.modules[fullname] = mod
        return mod

class CustomImporter2(object):
    def find_module(self, fullname, mpath=None):
        if fullname in ['_uuos', '_vm_api']:
            return self
        return
    def load_module(self, module_name):
        print('++++load_module:', module_name)
        mod = sys.modules.get(module_name)
        if mod is None:
            spec = importlib.util.spec_from_file_location(module_name, uuos_so)
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            sys.modules[module_name] = module
        return mod

sys.meta_path.insert(0, CustomImporter2())

import _uuos
import _vm_api
print(_uuos, _vm_api)


