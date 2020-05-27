import _uuos

class DB(object):

    def __init__(self, chain_ptr):
        self.db_ptr = _uuos.db_get_ptr(chain_ptr)

    def find_i64(self, code, scope, table, id):
        return _uuos.db_find_i64(self.db_ptr, code, scope, table, id )

    def get_i64(self, itr):
        return _uuos.db_get_i64(self.db_ptr, itr)

    def next_i64(self, itr):
        return _uuos.db_next_i64(self.db_ptr, itr)

    def previous_i64(self, itr):
        return _uuos.db_previous_i64(self.db_ptr, itr)

    def db_find_i64(self, code, scope, table, id):
        return _uuos.db_find_i64(self.db_ptr, code, scope, table, id )

    def db_remove_i64(self, itr):
        return _uuos.db_remove_i64(self.db_ptr, itr)

    def db_lowerbound_i64(self, code, scope, table, id ):
        return _uuos.db_lowerbound_i64(self.db_ptr, code, scope, table, id)

    def db_upperbound_i64(self, code, scope, table, id ):
        return _uuos.db_upperbound_i64(self.db_ptr, code, scope, table, id)

    def db_end_i64(self, code, scope, table):
        return _uuos.db_end_i64(self.db_ptr, code, scope, table)

