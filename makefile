SRCS = $(wildcard src/*.cpp)
INCS = $(wildcard include/*.h)
OBJS = $(SRCS:.c=.o)
#CC = clang
CC = g++
DEBUG = 
LIBS = -lstdc++
CXXFLAGS = -Ofast -Wall -Wno-deprecated-declarations $(DEBUG) -Iinclude
LFLAGS = -Wall $(LIBS) $(DEBUG)

fig : $(OBJS) $(INCS)
	$(CC) $(CXXFLAGS) $(LFLAGS) $(OBJS) -o fig

.PHONY: clean
clean:
	rm fig

