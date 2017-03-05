LIB=-libpthread.a -Wl,--no-as-need -std=c++11

all: ppm.exe

ppm.exe: ppm.o ppm_01.o
	g++ -o ppm.exe ppm.o ppm_01.o $(LIB)
ppm.o: ppm.cpp
	g++ -c ppm.cpp
ppm_01.o: ppm.cpp
	g++ -c ppm_01.cpp