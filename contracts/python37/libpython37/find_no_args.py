import os
def find_func_define(lines, func_name):
	for line in lines:
		if line.find(func_name) >= 0:
			if line.find(',') < 0:
				print(line)
			break
def extract_func_name(line):
	start = line.find(',')
	if start < 0:
		return
	end = line.find(', METH_NOARGS')
	if end < 0:
		return
	func_name = line[start+2:end]
	func_name = func_name.replace('(PyCFunction)', '')
	return func_name

for root, dirs, files in os.walk('.'):
	for file in files:
		if not file.endswith('.c'):
			continue
		with open(os.path.join(root,file), 'r') as f:
			print(file)
			lines = f.readlines()
			for line in lines:
				if line.find('METH_NOARGS') >=0:
					func_name = extract_func_name(line)
					if func_name:
						find_func_define(lines, func_name)
