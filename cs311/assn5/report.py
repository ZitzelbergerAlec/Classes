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

def send_handshake(sock):
	sock.send("<request type=\"handshake\" sender=\"report\"></request>\n") 

#Reference for getopt(): http://pymotw.com/2/getopt/
options, remainder = getopt.getopt(sys.argv[1:], 'k')
kflag = 0
for opt, arg in options:
    if opt in ('-k'):
    	kflag = 1

#wait for server handshake
data = get_data(sockfd)

#Send our handshake
send_handshake(sockfd)

if(kflag == 0):
	#Send query for data
	sockfd.send("<request type=\"query\" sender=\"report\"></request>\n") 
	data = get_data(sockfd)
	for packet in data:
		xmldoc = minidom.parseString(packet)
		#Parse and print connected clients
		print "Connected compute clients:"
		x = xmldoc.getElementsByTagName('client')
		for client in x:
			host = client.attributes['addr'].value
			mods_per_sec = client.attributes['mods_per_sec'].value
			print "Host:", host, "Mods per sec:", mods_per_sec
		#Parse and print perfect numbers
		print "Found perfect numbers:"
		x = xmldoc.getElementsByTagName('perfect_number')
		for perfect_number in x:
			print perfect_number.attributes['value'].value
sockfd.close() 