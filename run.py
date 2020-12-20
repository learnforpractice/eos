import shlex
import subprocess
cmd = 'bash ./run-test.sh --count 10 -x programs/uuos/tests/pytest/test_micropython.py'
cmd = shlex.split(cmd)

while True:
	subprocess.check_output(cmd)
