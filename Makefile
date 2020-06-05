SOURCES = pinger.cpp
OBJS = $(SOURCES:.cpp=.o)
CXXFLAGS = -Wall -g
LDDFLAGS = -ltins

all: pinger

pinger: $(OBJS)
	g++ -o pinger $(CXXFLAGS) $(OBJS) $(LDDFLAGS)

.cpp.o:
	g++ -c $(CXXFLAGS) $<
