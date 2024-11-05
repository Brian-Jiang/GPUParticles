
import os
import sys
import marshal

sys.path.append(os.path.normpath(os.path.join(os.environ["PYENGINE_WORKSPACE_DIR"], "Tools")))
print sys.path
import PyClient
import PyClient.assetmanagercommon

#files in Pyengine2 folder
root = os.environ["PYENGINE_WORKSPACE_DIR"] # this is a path to the workspace directory. Pyengine2 is in that directory
pyengine2Root = os.path.join(root, 'PrimeEngine')

def main():
	print "\nExecuting PrimeEngine automated build:\n"
	tobuild = [["Release__Win32", "Release|Win32"], ["Debug_PS3__Win32", "Debug_PS3|Win32"], ["Debug_OGL__Win32", "Debug_OGL|Win32"], ["Release__Xbox360","Release|Xbox 360"]]
	#os.system("p4 -u akovalovs -p gpserver01.usc.edu:1666 login -p")
	originalStdout = sys.stdout
	#p4syncf = open("p4sync.txt", "wb")
	#sys.stdout = p4syncf
	p4user = "akovalovs"
	p4client = "csci499"
	syncroot = "//gamepipe/CSCI_499/MainDev/..."
	print "Syncing user %s workspace %s path %s:" % (p4user, p4client, syncroot)
	os.system('p4 -G -u %s -P WhereHair -p gpserver01.usc.edu:1666 -c %s sync %s#head > p4sync.txt' % (p4user, p4client, syncroot))
	#p4syncf.close()
	#sys.stdout = originalStdout
	p4syncf = open("p4sync.txt", "rb")
	d = marshal.load(p4syncf)
	p4syncf.close()
	print "Result:"
	print d['data']
	for conf in tobuild:
		print "Cleaning %s:" % (conf[1])
		os.system('devenv "..\\PEWorkspace.sln" /Clean "%s" > "clean_%s.txt"' % (conf[1], conf[0]))
		res = open("clean_%s.txt" % (conf[0]))
		print "Result:"
		print res.readlines()[-1]
		res.close()
		print "Building %s:" % (conf[1])
		os.system('devenv "..\\PEWorkspace.sln" /Build "%s" > "build_%s.txt"' % (conf[1], conf[0]))
		res = open("build_%s.txt" % (conf[0]))
		print "Result:"
		print res.readlines()[-1]
		res.close()
if __name__ == "__main__":
	main()
