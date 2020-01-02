import ujson as json
from . import pack_native_object, unpack_native_object

def custom_setattr(self, attr, value):
    if attr == '_dict':
        type(self).old_setattr(self, attr, value)
    else:
        if not attr in self._dict:
            raise AttributeError(attr)
        self._dict[attr] = value

class JsonObject(object):
    def __init__(self, msg_dict):
#        super(JsonObject, self).__init__(msg_dict)
        if isinstance(msg_dict, str):
            msg_dict = json.loads(msg_dict)
        self._dict = msg_dict
        if hasattr(JsonObject, 'old_setattr'):
            pass
        else:
            JsonObject.old_setattr = JsonObject.__setattr__
            JsonObject.__setattr__ = custom_setattr

    def __getattr__(self, attr):
        ret = self._dict[attr]
        if isinstance(ret, dict):
            return JsonObject(ret)
        return ret

    def dumps(self):
        return json.dumps(self._dict)

    def pack(self):
        msg = json.dumps(self._dict)
        return pack_native_object(self.obj_type, msg)

    @classmethod
    def unpack(cls, msg):
        msg = unpack_native_object(cls.obj_type, msg)
        msg = json.loads(msg)
        return cls(msg)

    def __str__(self):
        return json.dumps(self._dict, sort_keys=False, indent=4)

    def __repr__(self):
        return json.dumps(self._dict, sort_keys=False, indent=4)
