EXECUTABLES = bin/sb-read bin/sb-analyze bin/sb-play 
CPP = g++
PDS = plank-disjoint-sets
INCLUDE = -I$(PDS)/include
CFLAGS = -Wall -Wextra

all: $(EXECUTABLES)

clean:
	rm -f bin/*

bin/sb-read: sb-read.cpp
	$(CPP) -o bin/sb-read sb-read.cpp

bin/sb-analyze: sb-analyze.cpp $(PDS)/obj/disjoint_set.o
	$(CPP) $(CFLAGS) $(INCLUDE) -o bin/sb-analyze sb-analyze.cpp $(PDS)/obj/disjoint_set.o

bin/sb-play: sb-play.cpp $(PDS)/obj/disjoint_set.o
	$(CPP) $(CFLAGS) $(INCLUDE) -o bin/sb-play sb-play.cpp $(PDS)/obj/disjoint_set.o
