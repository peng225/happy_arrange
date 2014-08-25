#Makefile

CFLAGS = -g -c -march=native -std=c++0x -Wall -Weffc++ -O3
FINAL_CFLAGS = -g -march=native -std=c++0x -Wall -Weffc++ -O3
BINARY = happy
SIMPLE_BINARY = simple
GREEDY_BINARY = greedy
OBJS = main.o
SIMPLE_OBJS = simple.o
GREEDY_OBJS = greedy.o
ALL_CPP_FILES = main.cpp
CC = g++

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(BINARY) $<

main.o: main.cpp
	$(CC) $(CFLAGS) $<

simple.o: simple.cpp
	$(CC) $(CFLAGS) $<

greedy.o: greedy.cpp
	$(CC) $(CFLAGS) $<

simple: $(SIMPLE_OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(SIMPLE_BINARY) $<

greedy: $(GREEDY_OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(GREEDY_BINARY) $<

main.o: node.h arrange.h common.h
simple.o: node.h common.h
greedy.o: node.h common.h

clean:
	rm -r $(BINARY) *.o *~
