import marshal

def gen_base58():
    with open('base58.py', 'r') as f:
        code = f.read()
    code = compile(code, "contract", 'exec')
    code = marshal.dumps(code)
    with open('M_base58.c', 'w') as f:
        f.write('unsigned char M_base58[] = {')
        n = 0
        for c in code:
            f.write('%d,'%(c,))
            n += 1
            if n%16 == 0:
                f.write('\n')
        f.write('\n};')


def gen_ascii():
    with open('ascii.py', 'r') as f:
        code = f.read()
    code = compile(code, "contract", 'exec')
    code = marshal.dumps(code)
    with open('M_ascii.c', 'w') as f:
        f.write('unsigned char M_ascii[] = {')
        n = 0
        for c in code:
            f.write('%d,'%(c,))
            n += 1
            if n%16 == 0:
                f.write('\n')
        f.write('\n};')


def gen_frozen_module(name):
    with open(name+'.py', 'r') as f:
        code = f.read()
    code = compile(code, "contract", 'exec')
    code = marshal.dumps(code)
    with open('../M/M_%s.c'%(name,), 'w') as f:
        f.write('unsigned char M_%s[] = {'%(name,))
        n = 0
        for c in code:
            f.write('%d,'%(c,))
            n += 1
            if n%16 == 0:
                f.write('\n')
        f.write('\n};')
gen_frozen_module('db')
