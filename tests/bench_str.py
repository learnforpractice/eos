import time

t0 = time.time()
for i in range(1000000):
    a = 1
print("assignment.py", time.time()-t0)

t0 = time.time()
a = 0
for i in range(1000000):
    a += 1
print("augm_assign.py", time.time()-t0)

t0 = time.time()
for i in range(1000000):
    a = 1.0
print("assignment_float.py", time.time()-t0)

t0 = time.time()
for i in range(1000000):
    a = {0:0}
print("build_dict.py", time.time()-t0)

t0 = time.time()
a = {0:0}

for i in range(1000000):
    a[0] = i

assert a[0]==999999
print("set_dict_item.py", time.time()-t0)

t0 = time.time()
for i in range(1000000):
    a = [1, 2, 3]
print("build_list.py", time.time()-t0)

t0 = time.time()
a = [0]

for i in range(1000000):
    a[0] = i
print("set_list_item.py", time.time()-t0)

t0 = time.time()
a, b, c = 1, 2, 3
for i in range(1000000):
    a + b + c
print("add_integers.py", time.time()-t0)

t0 = time.time()
a, b, c = 'a', 'b', 'c'
for i in range(1000000):
    a + b + c
print("add_strings.py", time.time()-t0)

t0 = time.time()
for _i in range(100000):
    str(_i)
print("str_of_int.py", time.time()-t0)

t0 = time.time()
for i in range(1000000):
    def f():
        pass
print("create_function.py", time.time()-t0)

t0 = time.time()
def f(x):
    return x
for i in range(1000000):
    f(i)
print("function_call.py", time.time()-t0)

