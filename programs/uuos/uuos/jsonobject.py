import ujson as json
from _uuos import pack_native_object, unpack_native_object

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
        self._dict = msg_dict
        if hasattr(JsonObject, 'old_setattr'):
            pass
        else:
            JsonObject.old_setattr = JsonObject.__setattr__
            JsonObject.__setattr__ = custom_setattr

    def __getattr__(self, attr):
        return self._dict[attr]

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
