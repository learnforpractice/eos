from _db import *
import struct
class DBi64:
    def __init__(self, code, scope, table, data_type=bytes):
        self.code = code
        self.scope = scope
        self.table = table
        self.data_type = data_type

    def __getitem__(self, key):
        itr = find_i64(self.code, self.scope, self.table, key)
        if itr >= 0:
            data = get_i64(itr)
            if self.data_type == bytes:
                return data
            return self.data_type.unpack(data)
        raise KeyError

    def __setitem__(self, key, obj):
        '''
        if value is not a bytes object, call value.pack()
        '''
        if not isinstance(obj, self.data_type):
            raise ValueError
        if not isinstance(obj, bytes):
            value = obj.pack()
        else:
            value = obj
        itr = find_i64(self.code, self.scope, self.table, key)
        if itr >= 0:
            update_i64(itr, obj.payer, value)
        else:
            store_i64(self.scope, self.table, obj.payer, key, value)

    def __delitem__(self, key):
        itr = find_i64(self.code, self.scope, self.table, key)
        if itr >= 0:
            return remove_i64(itr)
        raise KeyError

    def __contains__(self, id):
        itr = find_i64(self.code, self.scope, self.table, id)
        return itr >= 0

    def __iter__(self):
        self.itr = end_i64(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.key = previous_i64(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.key

class Idx64:

    def __init__(self, primary_index, index):
        self.primary_index = primary_index
        self.code = primary_index.code
        self.scope = primary_index.scope
        self.table = (primary_index.table&0xFFFFFFFFFFFFFFF0)+index
        self.payer = 0

    def __getitem__(self, secondary):
        itr, primary = idx64_find_secondary(self.code, self.scope, self.table, secondary)
        if itr >= 0:
            return self.primary_index[primary]
        raise KeyError

    def __setitem__(self, key, value):
        itr, secondary = idx64_find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            if secondary == value:
                return
            idx64_update(itr, self.payer, value)
        else:
            idx64_store(self.scope, self.table, self.payer, key, value)

    def __delitem__(self, key):
        itr, secondary = idx64_find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            return idx64_remove(itr)
        raise KeyError

    def __contains__(self, secondary):
        itr, primary = idx64_find_secondary(self.code, self.scope, self.table, secondary)
        return itr >= 0

    def __iter__(self):
        self.itr = idx64_end(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.key = idx64_previous(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.key


class Idx:

    def __init__(self, primary_index, index, index_type=64):
        self.primary_index = primary_index
        self.code = primary_index.code
        self.scope = primary_index.scope
        self.table = (primary_index.table&0xFFFFFFFFFFFFFFF0)+index
        self.payer = 0
        if index_type == 64:
            self.store = idx64_store
            self.update = idx64_update
            self.remove = idx64_remove
            self.next = idx64_next
            self.previous = idx64_previous
            self.find_primary = idx64_find_primary
            self.find_secondary = idx64_find_secondary
            self.lowerbound = idx64_lowerbound
            self.upperbound = idx64_upperbound
            self.end = idx64_end
        elif index_type == 128:
            self.store = idx128_store
            self.update = idx128_update
            self.remove = idx128_remove
            self.next = idx128_next
            self.previous = idx128_previous
            self.find_primary = idx128_find_primary
            self.find_secondary = idx128_find_secondary
            self.lowerbound = idx128_lowerbound
            self.upperbound = idx128_upperbound
            self.end = idx128_end
        elif index_type == 256:
            self.store = idx256_store
            self.update = idx256_update
            self.remove = idx256_remove
            self.next = idx256_next
            self.previous = idx256_previous
            self.find_primary = idx256_find_primary
            self.find_secondary = idx256_find_secondary
            self.lowerbound = idx256_lowerbound
            self.upperbound = idx256_upperbound
            self.end = idx256_end

    def __getitem__(self, secondary):
        itr, primary = self.find_secondary(self.code, self.scope, self.table, secondary)
        if itr >= 0:
            return self.primary_index[primary]
        raise KeyError

    def __setitem__(self, key, value):
        itr, secondary = self.find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            if secondary == value:
                return
            self.update(itr, self.payer, value)
        else:
            self.store(self.scope, self.table, self.payer, key, value)

    def __delitem__(self, key):
        itr, secondary = self.find_primary(self.code, self.scope, self.table, key)
        if itr >= 0:
            return self.remove(itr)
        raise KeyError

    def __contains__(self, secondary):
        itr, primary = self.find_secondary(self.code, self.scope, self.table, secondary)
        return itr >= 0

    def __iter__(self):
        self.itr = self.end(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.key = self.previous(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.primary_index[self.key]

class MultiIndex:

    def __init__(self, code, scope, table, data_type):
        self.code = code
        self.scope = scope
        self.table = table
        self.primary_index = DBi64(code, scope, table, data_type)
        self.indexes = []
        index = 0
        for index_type in data_type.get_secondary_indexes():
            index_obj = Idx(self.primary_index, index, index_type)
            self.indexes.append(index_obj)
            index += 1
        self.data_type = data_type

    def get_primary_index(self):
        return self.primary_index

    def get_secondary_index(self, index):
        return self.indexes[index]

    def __getitem__(self, key):
        return self.primary_index[key]

    def __setitem__(self, key, obj):
#        if key in self.primary_index:
#            self.__delitem__(key)
        self.primary_index[key] = obj
        i = 0
        for index in self.indexes:
            index.payer = obj.payer
            index[key] = obj.get_secondary_value(i)
            i += 1

    def store(self, obj):
        self.__setitem__(obj.get_primary_value(), obj)

    def __delitem__(self, key):
        obj = self.primary_index[key]
        del self.primary_index[key]
        i = 0
        for index in self.indexes:
            secondary_key = obj.get_secondary_value(i)
            del index[secondary_key]
            i += 1

    def __contains__(self, id):
        itr = find_i64(self.code, self.scope, self.table, id)
        return itr >= 0

    def __iter__(self):
        self.itr = end_i64(self.code, self.scope, self.table)
        return self

    def __next__(self):
        if self.itr == -1:
            raise StopIteration
        self.itr, self.key = previous_i64(self.itr)
        if self.itr < 0:
            raise StopIteration
        return self.primary_index[self.key]
