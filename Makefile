SOURCES = pinger.cpp
OBJS = $(SOURCES:.cpp=.o)
CXXFLAGS = -Wall -g -std=c++11 
LDDFLAGS = -ltins -lpthread

all: pinger

pinger: $(OBJS)
	g++ -o pinger $(CXXFLAGS) $(OBJS) $(LDDFLAGS)

.cpp.o:
	g++ -c $(CXXFLAGS) $<

test: test.o
	g++ -o test $(CXXFLAGS) test.o $(LDDFLAGS)
	./test
