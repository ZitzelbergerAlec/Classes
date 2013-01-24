import subprocess
import shlex
import os
import sys
import stat
import time

def ls_os(path):
	files = os.listdir(path)
	files.extend('. ..'.split())
	
	sorted_files = []

	for f in files:
		status = os.lstat(f)	
		perms = status.st_mode
		uid = status.st_uid
		gid = status.st_gid
		size = status.st_size
		mtime = status.st_mtime #modification time
		links = status.st_nlink #number of links
		
		file_mdata = [mtime, perms, uid, gid, size, f, links]
		
		sorted_files.append(file_mdata)
	sorted_files.sort()	
	
	for f in sorted_files:
		print f[1], f[6], f[2], f[3], '%4s' % f[4], ' '.join(time.ctime(f[0]).split()[1:4]), f[5],
		if os.path.islink(f[5]):
			print f[5], " -> ", os.readlink(f[5]) 
		else:
			print f[5]
		 

def main():
	ls_os('.')

if __name__ == '__main__':
	main()
