import os
import re

dir = r'D:\Secret'
list = []
prog  =  re.compile('Secret{.*?}')

for x in os.listdir(dir):
	x = dir+'\\'+x
	list.append(x)

for x2 in list:
	f = file(x2)
	x3 = f.readlines()
	for x4 in x3:
		if prog.match(x4):
			print x4

	




