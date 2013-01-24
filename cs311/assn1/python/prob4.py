import csv

def generateTriangles(max):
    	#generate array of triangle words
	triangles = []
    	for n in range(1,max,1):
        	triangles.append((n*(n+1))/2)
	return triangles

def readFile(wordfile):
    	#Read the file
    	reader = csv.reader(open(wordfile, "r"), delimiter = ",", quotechar='\"')
	listofwords = []

    	for word in reader:
        	listofwords.append(word)

    	listofwords = listofwords[0] #To compensate for the fact that the entire array is read into the first spot in listofwords
	return listofwords

def assignValues(listofwords, triangles):
    	#Create dictionary for assigning values to letters
    	value = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5, 'F': 6, 'G': 7, 'H': 8, 'I': 9, 'J': 10, 'K': 11, 'L': 12, 'M': 13, 'N': 14, 'O': 15, 'P': 16, 'Q': 17, 'R': 18, 'S': 19, 'T': 20, 'U': 21, 'V': 22, 'W': 23, 'X': 24, 'Y': 25, 'Z': 26}

    	tottriangles = 0 #total number of triangle words
	for word in listofwords:
        	total = 0
        	for letter in word:
            		total = total + value[letter]
        		if total in triangles: #is in array of triangles:
				tottriangles += 1
	return tottriangles

def main():
	triangles = generateTriangles(100) #generate 100 triangle values
	listofwords = readFile('words.txt')
	total = assignValues(listofwords, triangles)
	print "Total number of triangle words in list is:", total

if __name__ == '__main__':
	main()
