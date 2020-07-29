import ujson as json
from . import pack_native_object, unpack_native_object

def custom_setattr(self, attr, value):
    if attr == '_dict':
        type(self).old_setattr(self, attr, value)
    else:
        if not attr in self._dict:
            raise AttributeError(attr)
        self._dict[attr] = value

class JsonObject(dict):
    def __init__(self, *args, **kwargs):
        super(JsonObject, self).__init__(*args, **kwargs)
        self.__dict__ = self

    def dumps(self):
        return json.dumps(self)

    def pack(self):
        msg = json.dumps(self)
        return pack_native_object(self.obj_type, msg)

    @classmethod
    def unpack(cls, msg):
        msg = unpack_native_object(cls.obj_type, msg)
        msg = json.loads(msg)
        return cls(msg)

    def __str__(self):
        return json.dumps(self, sort_keys=False, indent=4)

    def __repr__(self):
        return json.dumps(self, sort_keys=False, indent=4)
