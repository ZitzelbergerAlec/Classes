#Instructions for assignment 2


In this assignment, you will be working with UNIX file I/O system calls.
Write a C program on os-class called myar.c. This program will illustrate the use of file I/O on UNIX by maintaining a UNIX archive library, in the standard archive format.
Once compiled your program should run in a manner similar to the standard UNIX command ar. You may wish to look at the man page for ar for insight, however, for this assignment, the following is the syntax your program must support:
myar key afile name ...
where afile is the name of the archive file to be used, and key is one of the following options:
-q	 quickly append named files to archive	  
-x	 extract named files	  
-t	 print a concise table of contents of the archive	  
-v	print a verbose table of contents of the archive	 
-d	delete named files from archive	 
-A	 quickly append all ``regular'' files in the current directory	 (except the archive itself)
-w	 Extra credit: for a given timeout, add all modified files to the archive.	 (except the archive itself)

The archive file maintained must use exactly the standard format defined in /usr/inc1ude/ar.h, and in fact may be tested with archives created with the ar command. The options listed above are compatible with the options having the same name in the ar command, except for the following exceptions: the -v and -t command take no further argument, and list all files in the archive. -v is short for -t -v on the ar command. The -A command in a new option not in the usual ar command.
Notes:
For the -q command myar should create an archive file if it doesn't exist, using permissions ``666''. For the other commands myar reports an error if the archive does not exist, or is in the wrong format.
You will have to use the system calls stat and utime to properly deal with extracting and restoring the proper timestamps. Since the archive format only allows one timestamp, store the mtime and use it to restore both the atime and mtime. Permissions should also be restored to the original value, subject to umask limitation.
The -q and -A commands do not check to see if a file by the chosen name already exists. It simply appends the files to the end of the archive.
The -x and -d commands operate on the first file matched in the archive, without checking for further matches.
In the case of the -d option, you will have to build a new archive file to recover the space. Do this by unlinking the original file after it is opened, and creating a new archive with the original name.
You are required to handle multiple file names as arguments.
Since file I/O is expensive, do not make more than one pass through the archive file, an issue especially relevant to the multiple delete case.
For the -w flag, the command will take as long as specified by the timeout argument. You should print out a status message upon adding a new file. This may result in many different copies of the same file in the archive.
For extra credit, any time a file is added that already exists, remove the old copy from the archive, but ONLY if it is not the same. If identical, do not add the new file.
Your write-up should include (at minimum) the following:
a design for your system, as well as places your implementation deviated from this design
a work log, detailing what you did when -- this can fairly easily be created if you are using some form of revision control
any challenges you overcame in completing this assignment
answers to the following questions:
what do you think the main point of this assignment is?
how did you ensure your solution was correct? Testing details, for instance.
what did you learn?
Things to turn in:
source code
write-up, as a tex document
any support files necessary to compile your tex document
a makefile to build both source code and your tex file
You are not allowed to use Lyx to create the document. You must create the document "by hand", instead. I will provide a makefile and a LaTeX template file for your use -- see the course website. You must modify the makefile to handle your source code, as well.
