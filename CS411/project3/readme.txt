Project 3:

In this project you will be developing your solution on the given hardware. Using the default Linux 3.0.4 kernel, you are asked to implement the following:
Write a RAM Disk device driver for the Linux Kernel which allocates a chunk of memory and presents it as a block device.
Using the Linux Kernel's Crypto API, add encryption to your block device such that your device driver encrypts and decrypts data when it is written and read.
This will be developed in the drivers/block directory.
The source repo from which to export is svn+ssh://os-class.engr.oregonstate.edu/scratch/cs411/linux. You will be using this source tree for the remainder of the projects.
You are not required to develop your solution on the hardware given, however when you demo your solution, it must be done via the hardware given. To receive full credit, your solution must be correct and working on the hardware provided.
You are also required to do the following:
Group write-up: Prepare a document containing the design you plan to use to implement the following algorithms.
Individual write-up: Answer the following questions in sufficient detail:
What do you think the main point of this assignment is?
How did you personally approach the problem? Design decisions, algorithm, etc.
How did you ensure your solution was correct? Testing details, for instance.
What did you learn?
Make sure your group submission is a tarball labelled CS411_project3_group#.tar.bz2 and includes the following:
A linux patch file containing your solution
The group write-up in LaTeX format.
Each individual will be required to submit their individual write-up in LaTeX format also conatined in a tarball labelled CS411_project3_writeup.tar.bz2
