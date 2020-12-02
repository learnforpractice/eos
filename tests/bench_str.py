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
t = []
i = 0
while i < 100000:
    t.append(i)
    i += 1
print("augmented assignment and list append:", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = 1.0
print("assignment_float.py", get_time()-t0)


t0 = get_time()
n = 60
for i in range(10000):
    2 ** n
print("big integers", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = {0:0}
print("build_dict.py", get_time()-t0)

t0 = get_time()
d = {}
for i in range(10000):
    d[i] = i
print("build_dict2.py", (get_time()-t0)*10)


t0 = get_time()
a = {0:0}
for i in range(1000000):
    a[0] = i
assert a[0]==999999
print("set_dict_item.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    a = {0, 2.7, "x"}
print("build_set.py", get_time()-t0)

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
a = [1, 2, 3]
for i in range(100000):
    a[:]
print("list slice.py", get_time()-t0)


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
for i in range(1000000):
    def f(x):
        pass
print("create function, single positional argument.py", get_time()-t0)

t0 = get_time()
for i in range(1000000):
    def f(x, y=1, *args, **kw):
        pass
print("create function, complex arguments.py", get_time()-t0)

t0 = get_time()
def f(x):
    return x
for i in range(1000000):
    f(i)
print("function_call.py", get_time()-t0)


t0 = get_time()
def f(x, y=0, *args, **kw):
    return x
for i in range(100000):
    f(i, 5, 6, a=8)
print("function call, complex arguments.py", get_time()-t0)

t0 = get_time()
for i in range(10000):
    class A:
        pass
print("create simple class.py", get_time()-t0)

t0 = get_time()
for i in range(10000):
    class A:
        def __init__(self, x):
            self.x = x
print("create class with int.py", get_time()-t0)

t0 = get_time()
class A:
    pass
for i in range(1000000):
    A()
print("create instance of simple class.py", get_time()-t0)

t0 = get_time()
class A:
    def __init__(self, x):
        self.x = x
for i in range(1000000):
    A(i)
print("create instance of class with init.py", get_time()-t0)



t0 = get_time()
class A:
    def __init__(self, x):
        self.x = x

    def f(self):
        return self.x
a = A(1)
for i in range(100000):
    a.f()
print("call instance method.py", get_time()-t0)
