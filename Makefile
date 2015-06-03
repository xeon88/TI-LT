CC=g++
CFLAGS=-O3
LIBS=-lm


SRCS=test.cpp random.cpp
OBJS = $(SRCS:.cpp=.o)	

all:
	make test


test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)


.o.cpp:
	$(CC) $(CFLAGS) -c $*.c

clean: 
	rm *.o
