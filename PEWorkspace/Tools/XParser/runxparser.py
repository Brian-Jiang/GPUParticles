import xconv
from sys import stdin
import os
import xparser

while 1:
	print 'Enter .x filename:'
	filename = os.environ["PYENGINE_WORKSPACE_DIR"] + '/AssetsIn/XFiles/' + stdin.readline().lower().strip(' \n')
	xparser.parseXFile(filename)
	print 'Do you want to parse another file? [y/n]'
	ch = stdin.readline().lower()
	if not 'y' in ch: break
