CFLAGS = -g -c -Wall -Wextra -Wwrite-strings
LFLAGS = -g

procInfo: procInfo.o syscalls.o
	g++ $(LFLAGS) -o procInfo procInfo.o syscalls.o

procInfo.o: procMemListing.cpp syscalls.h
	g++ $(CFLAGS) procMemListing.cpp -o procInfo.o

syscalls.o: syscalls.cpp
	g++ $(CFLAGS) syscalls.cpp -o syscalls.o

clean:
	rm -f *.out *.o *.diff procInfo
	clear
