import shutil

with open('pythonvm.c.bin', 'r') as f:
    lines = f.readlines()

with open('pythonvm.c.bin.bk', 'w') as f:
    ignore = False
    for line in lines:
        if line.startswith('    target_'):
            line = line.replace('();', ':')
            line = line.replace('    target_', 'TARGET_')
            f.write(line)
            continue
        elif line.find('static u32 _PyEval_EvalFrameDefault(u32 p0, u32 p1)') >= 0:
            f.write(line)
            f.write('#include "opcode_targets.h"\n')
            continue
        elif line.find('static void computed_goto(u32);') >= 0:
            continue
        elif line.find('static void computed_goto(u32 p0) {') >= 0:
            ignore = True
            continue

        if ignore:
            if line.find('static') >= 0:
                f.write('#define computed_goto(p0) \\\n')
                f.write('  goto *opcode_targets[p0];\n\n')
                ignore = False
        if not ignore:
            f.write(line)

shutil.move('pythonvm.c.bin.bk', 'pythonvm.c.bin')



# #define computed_goto(p0) \
#   goto *opcode_targets[p0];
