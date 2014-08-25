#Makefile

CFLAGS = -g -c -march=native -std=c++0x -Wall -Weffc++ -O3
FINAL_CFLAGS = -g -march=native -std=c++0x -Wall -Weffc++ -O3
BINARY = happy
SIMPLE_BINARY = simple
GREEDY_BINARY = greedy
SHARED_OBJS = common.o
OBJS = main.o arrange.o $(SHARED_OBJS)
SIMPLE_OBJS = simple.o $(SHARED_OBJS)
GREEDY_OBJS = greedy.o $(SHARED_OBJS)
CC = g++

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(BINARY) $(OBJS)

main.o: main.cpp
	$(CC) $(CFLAGS) $<

arrange.o: arrange.cpp
	$(CC) $(CFLAGS) $<

common.o: common.cpp
	$(CC) $(CFLAGS) $<

simple.o: simple.cpp
	$(CC) $(CFLAGS) $<

greedy.o: greedy.cpp
	$(CC) $(CFLAGS) $<

simple: $(SIMPLE_OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(SIMPLE_BINARY) $(SIMPLE_OBJS)

greedy: $(GREEDY_OBJS)
	$(CC) $(FINAL_CFLAGS) -o $(GREEDY_BINARY) $(GREEDY_OBJS)

main.o: node.h arrange.h common.h
arrange.o: arrange.h
common.o: common.h
simple.o: node.h common.h
greedy.o: node.h common.h

clean:
	rm -r $(BINARY) *.o *~
