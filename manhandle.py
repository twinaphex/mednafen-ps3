import os
import tempfile
import sys
import subprocess

if len(sys.argv) != 4:
	print "Usage: manhandle.py [libwhatever.a] [prefix] [objcopy binary path]"
	sys.exit()

print "Mangling Symbols in " + sys.argv[1]
print "Prefixing with " + sys.argv[2]
print "Using objcopy " + sys.argv[3]

proc = subprocess.Popen("nm --defined-only -A -a " + sys.argv[1] + " | sed -e 's/.*\ //'", stdout=subprocess.PIPE, shell=True)
things = proc.communicate()[0].split('\n')

donethings = []
tf, tfn = tempfile.mkstemp()

for i in things:
	if not i in donethings and not "basic_stringbuf" in i and not "vector" in i and not "stat" in i:
		if "windows" in os.getenv("TARGETPLATFORM", "other"):
			if len(i) != 0 and i[0] == '_':
				donethings.append(i)
				os.write(tf, i + " _" + sys.argv[2] + i[1:])
				os.write(tf, "\n")
		else:
			donethings.append(i)
			os.write(tf, i + " " + sys.argv[2] + i)
			os.write(tf, "\n")


os.close(tf)
os.system(sys.argv[3] + " --redefine-syms " + tfn + " " + sys.argv[1])
os.remove(tfn)

