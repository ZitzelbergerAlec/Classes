#!/usr/bin/env python 
import socket
from xml.dom import minidom
import signal
import os
import select
import sys

#Global variables
conn = 0
perfect_numbers = []
compute_processes = dict() #Dictionary to hold compute processes by hostname->mods_per_sec

#signal handler
def handler(signum, frame):
	conn.close()
	sys.exit()
	
# Set the signal handler
signal.signal(signal.SIGQUIT, handler)


# to do: bump HOST and PORT into a lib file and have report and manage pull from that
HOST = "localhost"                 # Symbolic name meaning all available interfaces
PORT = 43283              # Arbitrary non-privileged port
BUFFSIZE = 4096

srvsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
srvsock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #Prevent socket from being left in TIME_WAIT state
srvsock.bind((HOST, PORT))
srvsock.listen(5) #5 is the maximum number of queued connections
descriptors = [srvsock] #array of sockets

def send_handshake(sock):
	sock.send("<request type=\"handshake\" sender=\"manage\"></request>")

def get_handshake(sock):
	data = get_data(sock)
	packet = data[0]
	xmldoc = minidom.parseString(packet)
	x = xmldoc.getElementsByTagName('request')[0]
	client = x.attributes['sender'].value
	request_type = x.attributes['type'].value
	if(client == "compute" and request_type == "handshake"):
		host,port = sock.getpeername()
		x = xmldoc.getElementsByTagName('performance')[0]
		mods_per_sec = x.attributes['mods_per_sec'].value
		compute_processes[host] = mods_per_sec

#Returns an array of packet data split by newline
def get_data(sock):
	data = sock.recv(BUFFSIZE)
	data = data.strip() #remove trailing and leading whitespace
	data = data.split('\n')
	return data

while 1:
	#reference: http://stackoverflow.com/questions/9306412/python-socket-programming-need-to-do-something-while-listening-for-connections
	#multiplex the socket. Wait for an event on a readable socket
	(sread, swrite, sexc) = select.select(descriptors, [], [])
	
	for sock in sread:
		if(sock == srvsock): #New connection
			newsock, (remhost, remport) = srvsock.accept()
			descriptors.append(newsock)
			print "New client joined at", remhost
			send_handshake(newsock)
			get_handshake(newsock)
		else:
			data = get_data(sock)
			if(data == ''):
					print "Client left"
					sock.close()
					descriptors.remove(sock)
			else:
				for packet in data:
					xmldoc = minidom.parseString(packet)
					x = xmldoc.getElementsByTagName('request')[0]
					client = x.attributes['sender'].value
					request_type = x.attributes['type'].value
					if(client == "compute"):
						if(request_type == "query"):
							x = xmldoc.getElementsByTagName('performance')[0]
							mods_per_sec = x.attributes['mods_per_sec'].value
							compute_processes[addr] = mods_per_sec
							print "Client requested new range. Client can compute", mods_per_sec, "mods per second"
							sock.send("<request type=\"new_range\" sender=\"manage\"><min value=\"1\"/><max value=\"9500\"/></request>")
						elif(request_type == "new_perfect"):
							x = xmldoc.getElementsByTagName('new_perfect')[0]
							perfect_numbers.append(x.attributes['value'].value)
							print "Found a new perfect number. Appended it to the list. Currently, perfect numbers are:", perfect_numbers
					elif(client == "compute-d"):
						print "Client is compute-d"
					elif(client == "report"):
						if(request_type == "query"):
							print "Client is report, request is query"
							print "Sending performance characteristics of clients. Current clients:", compute_processes
							querystring = "<request type=\"report_data\" sender=\"manage\">"
							for hostname in compute_processes:
								querystring.append("<client addr=\"")
								querystring.append(host)
								querystring.append("\" ")
								querystring.append("mods_per_sec=\"")
								querystring.append(compute_processes[host])
								querystring.append("\"/>")
							querystring.append("</request>")
							sock.send(querystring)

