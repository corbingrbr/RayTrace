PROG = raytrace
CC = g++

all: 
	$(CC) -std=c++11 -g -o $(PROG) src/*.cpp
clean:
	rm $(PROG)	
