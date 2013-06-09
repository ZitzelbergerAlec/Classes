Project 1:

Due: 23:59:59 (UTC-7), 15 April 2013

Please pick up the hardware you will be using in KEC 2063. One machine per group.
In this project you will be developing your solution on the given hardware. Using a modified Linux 3.0.4 Kernel, you are asked to implement the following:
Implement the Round Robin process scehduling algorithm
Implement the First-In First-Out(FIFO) process scheduling algorithm
Look specifically in the Kernel directory.
You will be required to make use of subversion for this and all subsequent projects. Export the kernel to use from our (read-only) repository at the URL 

svn+ssh://os-class.engr.oregonstate.edu/scratch/cs411/project1
Ensure that you use the 'export' command to svn. Using svnadmin, create a new svn repo on os-class under the /scratch directory. Please ensure you tag it with your group number. This will be the master copy for your group. As you work, ensure each of you is committing your work to this repo periodically.
See here for instructions on working with subversion in a group setting.
The easiest way to push a copy of the source onto your squidly would be to export a clean copy from the repo to a USB drive, and then copy from the USB drive to the squidly. Alternatively, get network connectivity, and do it directly on the squidly.
You are not required to develop your solution on the hardware given, however when you demo your solution, it must be done via the hardware given. TO receive full credit, your solution must be correct and working on the hardware provided.
Using the serial console with Squidlies:
Install the Serial Cable driver. Both Mac and Windows drivers are in the zip file.
Connect the usb serial cable to your computer and the designated squidly. On a Windows machine it should tell you which port it is using. If not, look at the device manager. On a Mac, it will be /dev/tty.usbserial.
On Windows: Once it is connected, open PuTTy, select a serial connection, and enter the COM port it is registered and the baudrate of 115200. Open a serial terminal, and now you will have access to serial display.
On Mac: open a terminal and execute the following command: 
screen /dev/tty.usbserial 115200
The username/password to access the squidlies initially is bob/bob123bob.
You are also required to do the following:
Group write-up: Prepare a document containing the design you plan to use to implement the following algorithms.
Individual write-up: Answer the following questions in sufficient detail:
What do you think the main point of this assignment is?
How did you personally approach the problem? Design decisions, algorithm, etc.
How did you ensure your solution was correct? Testing details, for instance.
What did you learn?
Make sure your group submission is a tarball labelled CS411_project1_group#.tar.bz2 and includes the following:
A linux patch file containing your solution
The group write-up in LaTeX format.
Each individual will be required to submit their individual write-up in LaTeX format also conatined in a tarball labelled CS411_project1_writeup.tar.bz2
