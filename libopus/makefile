CFLAGS=-g -Wall
LIB=-lopus

all: opusdec.o opusenc.o
	g++ ${CFLAGS} -o opusdec opusdec.o ${LIB}
	g++ ${CFLAGS} -o opusenc opusenc.o ${LIB}
	
opusenc.o: opusenc.cpp
	g++ -c ${CFLAGS} opusenc.cpp
	
opusdec.o: opusdec.cpp
	g++ -c ${CFLAGS} opusdec.cpp
	
clean:
	rm -f *.o opusdec opusenc
