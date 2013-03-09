# Echo server program
import socket
from xml.dom import minidom
import signal, os
import sys

#Global variables
conn = 0
perfect_numbers = [];

#signal handler
def handler(signum, frame):
	conn.close()
	sys.exit()
	
# Set the signal handler
signal.signal(signal.SIGQUIT, handler)

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 43283              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print 'binding to port', PORT
s.bind((HOST, PORT))
s.listen(1)
conn, addr = s.accept()
print 'Connected by', addr


while 1:
	data = conn.recv(1024)
	if not data: break
	data = data.strip() #remove trailing and leading whitespace
	xmldoc = minidom.parseString(str(data))
	x = xmldoc.getElementsByTagName('request')[0]
	client = x.attributes['sender'].value
	request_type = x.attributes['type'].value
	if(client == "compute"):
		if(request_type == "query"):
			x = xmldoc.getElementsByTagName('performance')[0]
			mods_per_sec = x.attributes['mods_per_sec'].value
			print "Client requested new range. Client can compute", mods_per_sec, "mods per second"
			conn.send("<request type=\"new_range\" sender=\"manage\"><min value=\"1\"/><max value=\"9500\"/></request>")
		elif(request_type == "new_perfect"):
			x = xmldoc.getElementsByTagName('new_perfect')[0]
			perfect_numbers.append(x.attributes['value'].value)
			print "Found a new perfect number. Appended it to the list. Currently, perfect numbers are:", perfect_numbers
conn.close()
