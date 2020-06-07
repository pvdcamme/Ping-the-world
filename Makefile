SOURCES = pinger.cpp
OBJS = $(SOURCES:.cpp=.o)
CXXFLAGS = -Wall -std=c++11 -g 
LDDFLAGS = -ltins -lpthread

.PHONY: test

all: pinger

pinger: $(OBJS)
	g++ -o pinger $(CXXFLAGS) $(OBJS) $(LDDFLAGS)

.cpp.o:
	g++ -c $(CXXFLAGS) $<

test: test.o
	g++ -o test $(CXXFLAGS) test.o $(LDDFLAGS)
	./test
