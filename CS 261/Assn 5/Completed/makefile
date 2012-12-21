default: prog 

dynArray.o: dynArray.c dynArray.h type.h 
	gcc -Wall -std=c99 -c dynArray.c

toDoList.o: toDoList.c toDoList.h type.h 
	gcc -Wall -std=c99 -c toDoList.c

prog: dynArray.o toDoList.o main.c
	gcc -Wall -std=c99 -o prog dynArray.o toDoList.o main.c

heapsort: dynArray.o toDoList.o main.c
	gcc -Wall -std=c99 -o heapsort dynArray.o toDoList.o main.c

clean:
	rm dynArray.o	
	rm toDoList.o

cleanall: clean
	rm prog
	rm heapsort
