import csv

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
    values = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5, 'F': 6, 'G': 7, 'H': 8, 'I': 9, 'J': 10, 'K': 11, 'L': 12, 'M': 13, 'N': 14, 'O': 15, 'P': 16, 'Q': 17, 'R': 18, 'S': 19, 'T': 20, 'U': 21, 'V': 22, 'W': 23, 'X': 24, 'Y': 25, 'Z': 26}

    total = 0
    i = 0
    for name in listofnames:
        i += 1 #increment i
        subtotal = 0
        for letter in name:
            subtotal = subtotal + values[letter]
        total = total + i * subtotal
    return total

def main():
    listofnames = readnames("names.txt")
    total = findtotal(listofnames)
    print "Total is ", total

if __name__ == '__main__':
    main()
