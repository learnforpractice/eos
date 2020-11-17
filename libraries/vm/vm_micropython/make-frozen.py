#!/usr/bin/env python
#
# Create frozen modules structure for MicroPython.
#
# Usage:
#
# Have a directory with modules to be frozen (only modules, not packages
# supported so far):
#
# frozen/foo.py
# frozen/bar.py
#
# Run script, passing path to the directory above:
#
# ./make-frozen.py frozen > frozen.c
#
# Include frozen.c in your build, having defined MICROPY_MODULE_FROZEN_STR in
# config.
#
from __future__ import print_function
import sys
import os
from pyeoskit import eosapi

def module_name(f):
    return f


modules = []

if len(sys.argv) > 1:
    root = sys.argv[1].rstrip("/")
    root_len = len(root)

    for dirpath, dirnames, filenames in os.walk(root):
        for f in filenames:
            if not f.endswith('.py'):
                continue
            fullpath = dirpath + "/" + f
            code = None
            with open(fullpath, 'r') as f:
                code = f.read()
                code = eosapi.compile_py_src(code)
            mpy = fullpath[:-3] + '.mpy'
            with open(mpy, 'wb') as f:
                f.write(code)

    for dirpath, dirnames, filenames in os.walk(root):
        for f in filenames:
            if not f.endswith('.mpy'):
                continue
            fullpath = dirpath + "/" + f
            st = os.stat(fullpath)
            modules.append((fullpath[root_len + 1 :], st))

output = sys.argv[2]
output = open(output, 'w')

print("#include <stdint.h>", file=output)
print("#include <stdlib.h>", file=output)
print("#include <string.h>", file=output)

print("const char mp_frozen_str_names[] = {", file=output)
for f, st in modules:
    m = module_name(f)
    print('"%s\\0"' % m, file=output)
print('"\\0"};', file=output)

print("const uint32_t mp_frozen_str_sizes[] = {", file=output)

for f, st in modules:
    print("%d," % st.st_size, file=output)

print("0};", file=output)

print("const char mp_frozen_str_content[] = {", file=output)
for f, st in modules:
    data = open(sys.argv[1] + "/" + f, "rb").read()

    # We need to properly escape the script data to create a C string.
    # When C parses hex characters of the form \x00 it keeps parsing the hex
    # data until it encounters a non-hex character.  Thus one must create
    # strings of the form "data\x01" "abc" to properly encode this kind of
    # data.  We could just encode all characters as hex digits but it's nice
    # to be able to read the resulting C code as ASCII when possible.

    data = bytearray(data)  # so Python2 extracts each byte as an integer
    esc_dict = {ord("\n"): "\\n", ord("\r"): "\\r", ord('"'): '\\"', ord("\\"): "\\\\"}
    chrs = ['"']
    break_str = False
    for c in data:
        try:
            chrs.append(esc_dict[c])
        except KeyError:
            if 32 <= c <= 126:
                if break_str:
                    chrs.append('" "')
                    break_str = False
                chrs.append(chr(c))
            else:
                chrs.append("\\x%02x" % c)
                break_str = True
    chrs.append('\\0"')
    print("".join(chrs), file=output)

print('"\\0"};', file=output)

s = '''
size_t vm_load_frozen_module(const char *str, size_t len, char *content, size_t content_size) {
    const char *name = mp_frozen_str_names;
    size_t offset = 0;
    for (int i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l == len && !memcmp(str, name, l)) {
            size_t str_size = mp_frozen_str_sizes[i];
            if (content == NULL || content_size == 0) {
                return str_size;
            }
            size_t copy_size = str_size;
            if (copy_size > content_size) {
                copy_size = content_size;
            }
            memcpy(content, mp_frozen_str_content + offset, copy_size);
            return copy_size;
        }
        name += l + 1;
        offset += mp_frozen_str_sizes[i] + 1;
    }
    return 0;
}

typedef enum {
    MP_IMPORT_STAT_NO_EXIST,
    MP_IMPORT_STAT_DIR,
    MP_IMPORT_STAT_FILE,
} mp_import_stat_t;

static mp_import_stat_t mp_frozen_stat_helper(const char *name, const char *str) {
    size_t len = strlen(str);

    for (int i = 0; *name != 0; i++) {
        size_t l = strlen(name);
        if (l >= len && !memcmp(str, name, len)) {
            if (name[len] == 0) {
                return MP_IMPORT_STAT_FILE;
            } else if (name[len] == '/') {
                return MP_IMPORT_STAT_DIR;
            }
        }
        name += l + 1;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

uint32_t vm_frozen_stat(const char *str) {
    mp_import_stat_t stat;

    stat = mp_frozen_stat_helper(mp_frozen_str_names, str);
    if (stat != MP_IMPORT_STAT_NO_EXIST) {
        return (uint32_t)stat;
    }

    return (uint32_t)MP_IMPORT_STAT_NO_EXIST;
}
'''

print(s, file=output)

output.close()

