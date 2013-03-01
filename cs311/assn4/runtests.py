import subprocess


for l in range(500000000, 4000000001, 500000000):
	proc=subprocess.Popen(['tprimes','-l',str(l),'-t','1'])
	proc.communicate()
	for t in range(50,501,50):
		proc=subprocess.Popen(['tprimes','-l',str(l),'-t',str(t)])
		proc.communicate()


for t in range(50,501,50):
	proc=subprocess.Popen(['tprimes','-t',str(t)])
	proc.communicate()
