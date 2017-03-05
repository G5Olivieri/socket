LIB=-lpthread -pthread -Wl,--no-as-need

all: thread

thread: thread.o
	g++ -o thread thread.o $(LIB)
thread.o: thread.cpp
	g++ -c thread.cpp
