import time

def get_time():
    if hasattr(time, 'ticks_us'):
        return time.ticks_us()/1e6
    return time.time()

t0 = get_time()
for i in range(1000000):
    a = 1
print("assignment.py", get_time()-t0)

t0 = get_time()
a = 0
for i in range(1000000):
    a += 1
print("augm_assign.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = 1.0
print("assignment_float.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = {0:0}
print("build_dict.py", get_time()-t0)

t0 = get_time()
a = {0:0}

for i in range(1000000):
    a[0] = i

assert a[0]==999999
print("set_dict_item.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = [1, 2, 3]
print("build_list.py", get_time()-t0)

t0 = get_time()
a = [0]

for i in range(1000000):
    a[0] = i
print("set_list_item.py", get_time()-t0)

t0 = get_time()
a, b, c = 1, 2, 3
for i in range(1000000):
    a + b + c
print("add_integers.py", get_time()-t0)

t0 = get_time()
a, b, c = 'a', 'b', 'c'
for i in range(1000000):
    a + b + c
print("add_strings.py", get_time()-t0)

t0 = get_time()
for _i in range(100000):
    str(_i)
print("str_of_int.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    def f():
        pass
print("create_function.py", get_time()-t0)

t0 = get_time()
def f(x):
    return x
for i in range(1000000):
    f(i)
print("function_call.py", get_time()-t0)

