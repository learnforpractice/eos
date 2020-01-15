
from _uuos import _db
from .saferunner import safe_runner

@safe_runner
def store_i64(*args):
    return _db.store_i64(*args)

@safe_runner
def update_i64(*args):
    return _db.update_i64(*args)

@safe_runner
def find_i64(*args):
    return _db.find_i64(*args)

@safe_runner
def get_i64(*args):
    return _db.get_i64(*args)

@safe_runner
def remove_i64(*args):
    return _db.remove_i64(*args)

@safe_runner
def next_i64(*args):
    return _db.next_i64(*args)

@safe_runner
def previous_i64(*args):
    return _db.previous_i64(*args)

@safe_runner
def upperbound_i64(*args):
    return _db.upperbound_i64(*args)

@safe_runner
def lowerbound_i64(*args):
    return _db.lowerbound_i64(*args)

@safe_runner
def end_i64(*args):
    return _db.end_i64(*args)

@safe_runner
def get_count(*args):
    return _db.get_count(*args)

@safe_runner
def idx64_store(*args):
    return _db.idx64_store(*args)

@safe_runner
def idx64_update(*args):
    return _db.idx64_update(*args)

@safe_runner
def idx64_remove(*args):
    return _db.idx64_remove(*args)

@safe_runner
def idx64_next(*args):
    return _db.idx64_next(*args)

@safe_runner
def idx64_previous(*args):
    return _db.idx64_previous(*args)

@safe_runner
def idx64_find_primary(*args):
    return _db.idx64_find_primary(*args)

@safe_runner
def idx64_find_secondary(*args):
    return _db.idx64_find_secondary(*args)

@safe_runner
def idx64_lowerbound(*args):
    return _db.idx64_lowerbound(*args)

@safe_runner
def idx64_upperbound(*args):
    return _db.idx64_upperbound(*args)

@safe_runner
def idx64_end(*args):
    return _db.idx64_end(*args)

@safe_runner
def idx128_store(*args):
    return _db.idx128_store(*args)

@safe_runner
def idx128_update(*args):
    return _db.idx128_update(*args)

@safe_runner
def idx128_remove(*args):
    return _db.idx128_remove(*args)

@safe_runner
def idx128_next(*args):
    return _db.idx128_next(*args)

@safe_runner
def idx128_previous(*args):
    return _db.idx128_previous(*args)

@safe_runner
def idx128_find_primary(*args):
    return _db.idx128_find_primary(*args)

@safe_runner
def idx128_find_secondary(*args):
    return _db.idx128_find_secondary(*args)

@safe_runner
def idx128_lowerbound(*args):
    return _db.idx128_lowerbound(*args)

@safe_runner
def idx128_upperbound(*args):
    return _db.idx128_upperbound(*args)

@safe_runner
def idx128_end(*args):
    return _db.idx128_end(*args)

@safe_runner
def idx256_update(*args):
    return _db.idx256_update(*args)

@safe_runner
def idx256_remove(*args):
    return _db.idx256_remove(*args)

@safe_runner
def idx256_next(*args):
    return _db.idx256_next(*args)

@safe_runner
def idx256_previous(*args):
    return _db.idx256_previous(*args)

@safe_runner
def idx256_find_primary(*args):
    return _db.idx256_find_primary(*args)

@safe_runner
def idx256_find_secondary(*args):
    return _db.idx256_find_secondary(*args)

@safe_runner
def idx256_lowerbound(*args):
    return _db.idx256_lowerbound(*args)

@safe_runner
def idx256_upperbound(*args):
    return _db.idx256_upperbound(*args)

@safe_runner
def idx256_end(*args):
    return _db.idx256_end(*args)

@safe_runner
def idx_double_store(*args):
    return _db.idx_double_store(*args)

@safe_runner
def idx_double_update(*args):
    return _db.idx_double_update(*args)

@safe_runner
def idx_double_remove(*args):
    return _db.idx_double_remove(*args)

@safe_runner
def idx_double_next(*args):
    return _db.idx_double_next(*args)

@safe_runner
def idx_double_previous(*args):
    return _db.idx_double_previous(*args)

@safe_runner
def idx_double_find_primary(*args):
    return _db.idx_double_find_primary(*args)

@safe_runner
def idx_double_find_secondary(*args):
    return _db.idx_double_find_secondary(*args)

@safe_runner
def idx_double_lowerbound(*args):
    return _db.idx_double_lowerbound(*args)

@safe_runner
def idx_double_upperbound(*args):
    return _db.idx_double_upperbound(*args)

@safe_runner
def idx_double_end(*args):
    return _db.idx_double_end(*args)

@safe_runner
def idx_long_double_store(*args):
    return _db.idx_long_double_store(*args)

@safe_runner
def idx_long_double_update(*args):
    return _db.idx_long_double_update(*args)

@safe_runner
def idx_long_double_remove(*args):
    return _db.idx_long_double_remove(*args)

@safe_runner
def idx_long_double_next(*args):
    return _db.idx_long_double_next(*args)

@safe_runner
def idx_long_double_previous(*args):
    return _db.idx_long_double_previous(*args)

@safe_runner
def idx_long_double_find_primary(*args):
    return _db.idx_long_double_find_primary(*args)

@safe_runner
def idx_long_double_find_secondary(*args):
    return _db.idx_long_double_find_secondary(*args)

@safe_runner
def idx_long_double_lowerbound(*args):
    return _db.idx_long_double_lowerbound(*args)

@safe_runner
def idx_long_double_upperbound(*args):
    return _db.idx_long_double_upperbound(*args)

@safe_runner
def idx_long_double_end(*args):
    return _db.idx_long_double_end(*args)
