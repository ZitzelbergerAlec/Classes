import os
import sys
import getopt


def makedir(path): #Checks if directory exists before creating it
	if not os.path.exists(path):
		os.makedirs(path)
	else:
		print "Directory exists: ", path

def makelink(path, name):
	if not os.path.islink(name):
		os.symlink(path, name)
	else:
		print "Link exists: ", path
def main():
	term = sys.argv[1]
	course = sys.argv[2]
	maketerm(term)
	makecourse(term, course)

def maketerm(term):
	makedir(term)

def makecourse(term, course):
	makedir(os.getcwd() + "/" + term + "/" + course) #os.getcwd gets current working directory
	print("Directory \"" + term + "/" + course + "\" does not exist. Creating it.")
	makedir(os.getcwd() + "/" + term + "/" + course + "/assignments")
	makedir(os.getcwd() + "/" + term + "/" + course + "/examples")
	makedir(os.getcwd() + "/" + term + "/" + course + "/exams")
	makedir(os.getcwd() + "/" + term + "/" + course + "/lecture_notes")
	makedir(os.getcwd() + "/" + term + "/" + course + "/submissions")	
	makelink("/usr/local/classes/eecs/" + term + "/" + course + "/public_html", os.getcwd() + "/" + term + "/" + course + "/website")	#symlink(source, link_name)
	makelink("/usr/local/classes/eecs/" + term + "/" + course + "/handin", os.getcwd() + "/" + term + "/" + course + "/handin")	#symlink(source, link_name)

if __name__ == "__main__":
	main()
