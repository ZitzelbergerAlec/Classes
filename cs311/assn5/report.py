#!/usr/bin/env python 
import socket 
from xml.dom import minidom
import os
import sys
import getopt

HOST = 'localhost' 
PORT = 43283
BUFFSIZE = 4096
sockfd = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
sockfd.connect((HOST,PORT)) 

#Reference for getopt(): http://pymotw.com/2/getopt/
options, remainder = getopt.getopt(sys.argv[1:], 'k')
kflag = 0
for opt, arg in options:
    if opt in ('-k'):
    	kflag = 1

sockfd.send("<request type=\"query\" sender=\"report\"></request>\n") 
data = sockfd.recv(BUFFSIZE) 
sockfd.close() 
print 'Received:', data

