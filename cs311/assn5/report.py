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

#Function definitions
#Returns an array of packet data split by newline
def get_data(sock):
	data = sock.recv(BUFFSIZE)
	data = data.strip() #remove trailing and leading whitespace
	data = data.split('\n')
	return data

#Reference for getopt(): http://pymotw.com/2/getopt/
options, remainder = getopt.getopt(sys.argv[1:], 'k')
kflag = 0
for opt, arg in options:
    if opt in ('-k'):
    	kflag = 1

#wait for server handshake
data = get_data(sockfd)
print 'Received:', data

#Send our handshake
sockfd.send("<request type=\"handshake\" sender=\"report\"></request>\n") 

#Send query for data
sockfd.send("<request type=\"query\" sender=\"report\"></request>\n") 
data = get_data(sockfd)
sockfd.close() 
print 'Received:', data

