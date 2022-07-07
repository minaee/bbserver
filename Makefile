CXX = g++
CXXFLAGS = -g -pedantic -Wall -Werror -pthread

all: triv_client cserv

## Common:
tcp_utils.o: tcp_utils.h tcp_utils.cc
	$(CXX) $(CXXFLAGS) -c -o tcp_utils.o tcp_utils.cc

## Client:
client.o: tcp_utils.h client.cc
	$(CXX) $(CXXFLAGS) -c -o client.o client.cc

triv_client: client.o tcp_utils.o
	$(CXX) $(CXXFLAGS) -o triv_client client.o tcp_utils.o



## (c) true concurrency
cserv.o: tcp_utils.h cserv.cc
	$(CXX) $(CXXFLAGS) -c -o cserv.o cserv.cc

cserv: cserv.o tcp_utils.o
	$(CXX) $(CXXFLAGS) -o cserv cserv.o tcp_utils.o

clean:
	rm -f cserv triv_client *~ *.o *.bak core \#*

