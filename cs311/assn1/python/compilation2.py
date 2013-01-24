import os
import getopt
import sys
import getopt
import csv
import string

#Start Prob 1
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

def maketerm(term):
	makedir(term)

def makecourse(term, course):
	#os.getcwd gets current working directory
    #use current working directory if specified term is not absolute path
	makedir(term + "/" + course)
	print("Directory \"" + term + "/" + course + "\" does not exist. Creating it.")
	makedir(term + "/" + course + "/assignments")
	makedir(term + "/" + course + "/examples")
	makedir(term + "/" + course + "/exams")
	makedir(term + "/" + course + "/lecture_notes")
	makedir(term + "/" + course + "/submissions")
	
	makelink("/usr/local/classes/eecs/" + term + "/" + course + "/public_html", 
	os.getcwd() + "/" + term + "/" + course + "/website") #symlink(source, link_name)
	
	makelink("/usr/local/classes/eecs/" + term + "/" + course + "/handin", 
	os.getcwd() + "/" + term + "/" + course + "/handin") #symlink(source, link_name)

def prob1():
	opts, args = getopt.getopt(sys.argv[1:], "t:c:", ["term=", "course="])
	course = 0
	term = 0
	for o,a in opts:
		if o in ("-t", "--term"):
			term = a;
		if o in ("-c", "--course"):
			course = a 
	if term != 0:
		maketerm(term)
		if course != 0:
    			makecourse(term, course)
		else:
    			print "Please specify a course with the -c or --course option in the command line"
	else:
		print "Please specify a term with the -t or --term option in the command line"
# End Prob 1

#Start Prob 2
def findmaxproduct(monsternumber):
	maximum = 0
	for i in range(0, len(monsternumber)-4, 1): maximum = max((int(monsternumber[i]) * 
	int(monsternumber[i+1]) * int(monsternumber[i+2]) * int(monsternumber[i+3]) * 
	int(monsternumber[i+4]), maximum))
	return maximum

def prob2():
	monsternumber = "7316717653133062491922511967442657474235534919493496983520312774506326239578318016984801869478851843858615607891129494954595017379583319528532088055111254069874715852386305071569329096329522744304355766896648950445244523161731856403098711121722383113622298934233803081353362766142828064444866452387493035890729629049156044077239071381051585930796086670172427121883998797908792274921901699720888093776657273330010533678812202354218097512545405947522435258490771167055601360483958644670632441572215539753697817977846174064955149290862569321978468622482839722413756570560574902614079729686524145351004748216637048440319989000889524345065854122758866688116427171479924442928230863465674813919123162824586178664583591245665294765456828489128831426076900422421902267105562632111110937054421750694165896040807198403850962455444362981230987879927244284909188845801561660979191338754992005240636899125607176060588611646710940507754100225698315520005593572972571636269561882670428252483600823257530420752963450"
	max = findmaxproduct(monsternumber)
	print "Max product is", str(max)
#End prob 2

#start prob 3
def readnames(namefile):
    	reader = csv.reader(open(namefile, "r"), delimiter = ",", quotechar='\"')
	listofnames = []
    
    	for name in reader:
        	listofnames.append(name)
    
   	listofnames = listofnames[0]
    
    	listofnames.sort()
    	return listofnames

def findtotal(listofnames):
    
    	#Create dictionary for assigning values to letters
	values = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5, 'F': 6, 'G': 7, 'H': 8}
     	# value.update is a way of appending to a dictionary. I do this to avoid
        # exceeding the 80-character limit on filelines specified in the style guide
	values.update({ 'I': 9, 'J': 10, 'K': 11, 'L': 12, 'M': 13, 'N': 14})	
	values.update({'O': 15, 'P': 16, 'Q': 17, 'R': 18, 'S': 19, 'T': 20})
	values.update({'U': 21, 'V': 22, 'W': 23, 'X': 24, 'Y': 25, 'Z': 26})
    
    	total = 0
    	i = 0
    	for name in listofnames:
        	i += 1 #increment i
        	subtotal = 0
        	for letter in name:
            		subtotal = subtotal + values[letter]
        	total = total + i * subtotal
    	return total

def prob3():
    	print "Finding the total number value of the list of names..."
	listofnames = readnames("names.txt")
    	total = findtotal(listofnames)
    	print "Total is", total
#end prob 3

#Start Prob 4
def generateTriangles(max):
    	#generate array of triangle words
	triangles = []
    	for n in range(1,max,1):
        	triangles.append((n*(n+1))/2)
	return triangles

def readFile(wordfile):
    	#Read the file
	#Use csv because the file is a csv and this library is built for that
    	reader = csv.reader(open(wordfile, "r"), delimiter = ",", quotechar='\"')
	listofwords = []

    	for word in reader:
        	listofwords.append(word)

    	listofwords = listofwords[0] 
	#To compensate for the fact that the entire array is 
	#read into the first spot in listofwords
	return listofwords

def assignValues(listofwords, triangles):
    	#Create dictionary for assigning values to letters
    	value = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5, 'F': 6, 'G': 7, 'H': 8}
	# value.update is a way of appending to a dictionary. I do this to avoid
	# exceeding the 80-character limit on filelines specified in the style guide
	value.update({'I': 9, 'J': 10, 'K': 11, 'L': 12, 'M': 13, 'N': 14})  
	value.update({'O': 15, 'P': 16, 'Q': 17, 'R': 18, 'S': 19, 'T': 20}) 
	value.update({'U': 21, 'V': 22, 'W': 23, 'X': 24, 'Y': 25, 'Z': 26})

    	tottriangles = 0 #total number of triangle words
	for word in listofwords:
        	total = 0
        	for letter in word:
            		total = total + value[letter]
        	if total in triangles: #is in array of triangles:
            		tottriangles += 1
    	return tottriangles

def prob4():
	triangles = generateTriangles(1000)
	listofwords = readFile('words.txt')
	total = assignValues(listofwords, triangles)
	print "Total number of triangle words in list is", total
#End Prob 4

def main():
	print "Problem 1: Creates directories for term and course."
        print "Problem 2: Finds the greatest product of 5 consecutive numbers in 1000-digit number."
        print "Problem 3: Finds the total score of names from a list of names."
        print "Problem 4: Finds the total number of triangle words from a list."
	while(1):
		var = raw_input("Enter 1-4 and then return to select problems 1-4 or 'q' to quit: ")
    		if var == "1":
        		prob1()
    		elif var == "2":
        		prob2()
    		elif var == "3":
        		prob3()
    		elif var == "4":
        		prob4()
		elif var == "q":
			quit()
    		else:
        		print "Sorry, I didn't understand that. Please try again."

if __name__ == '__main__':
	main()
