CC = icc
CXX = icpc
CFLAGS = -Wall -std=c99 -openmp -O3 -g -I.
CXXFLAGS = -Wall -openmp -O3 -g
#overkill on the flags, but that doesn't hurt anything
LDFLAGS = -lrt -lpthread 
#replace this if you want to change the output name

#default is to compile
default:	compile

#depends on all of you source and header files
compile:
	${CC} ${CFLAGS} tprimes.c -o tprimes ${LDFLAGS}
	${CC} ${CFLAGS} pprimes.c -o pprimes ${LDFLAGS}
