CFLAGS=-g -Wall
LIB=-lspeex

all: speexdec.o speexenc.o
	g++ ${CFLAGS} -o speexdec speexdec.o ${LIB}
	g++ ${CFLAGS} -o speexenc speexenc.o ${LIB}
	
speexenc.o: speexenc.cpp
	g++ -c ${CFLAGS} speexenc.cpp
	
speexdec.o: speexdec.cpp
	g++ -c ${CFLAGS} speexdec.cpp
	
clean:
	rm -f *.o speexdec speexenc
