import os
import tempfile
import sys
import subprocess

#TODO: Support specifying the arguments for nm as well, OSX needs them

if len(sys.argv) != 5:
	print "Usage: manhandle.py [libwhatever.a] [prefix] [objcopy binary path] [nm binary path]"
	sys.exit()

print "Mangling Symbols in " + sys.argv[1]
print "Prefixing with " + sys.argv[2]
print "Using objcopy " + sys.argv[3]
print "Using nm " + sys.argv[4]

proc = subprocess.Popen(sys.argv[4] + " --defined-only -A " + sys.argv[1] + " | sed -e 's/.*\ //'", stdout=subprocess.PIPE, shell=True)
things = proc.communicate()[0].split('\n')

donethings = []
tf, tfn = tempfile.mkstemp()

for i in things:
	i = i.strip("\n\r ")
	if not i in donethings and not "basic_stringbuf" in i and not "vector" in i and not "string" in i and not "printf" in i:
		if len(i) and i[0] == '_':
			donethings.append(i)
			os.write(tf, i + " _" + sys.argv[2] + i[1:])
			os.write(tf, "\n")
		elif len(i) and i[0] == '.':
			donethings.append(i)
			os.write(tf, i + " ." + sys.argv[2] + i[1:])
			os.write(tf, "\n")
		else:
			donethings.append(i)
			os.write(tf, i + " " + sys.argv[2] + i)
			os.write(tf, "\n")
		

os.close(tf)
os.system(sys.argv[3] + " --redefine-syms " + tfn + " " + sys.argv[1])
os.remove(tfn)

