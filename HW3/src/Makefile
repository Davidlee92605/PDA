all: ../bin/hw3

CXX = g++

OFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3 -DNDEBUG

LINKFLAGS = -Ofast -march=native -std=c++11 

../bin/hw3: main.o parse.o print.o
	$(CXX) $(LINKFLAGS) main.o parse.o print.o -o ../bin/hw3

main.o: main.cpp
	$(CXX) $(LINKFLAGS) main.cpp -c

parse.o: parse.cpp
	$(CXX) $(LINKFLAGS) parse.cpp -c

print.o: print.cpp
	$(CXX) $(LINKFLAGS) print.cpp -c


clean:
	rm -rf *.o *.gch ../bin/hw3
