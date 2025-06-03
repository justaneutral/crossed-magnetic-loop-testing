CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDLIBS = -lm

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

TARGET = iir_test

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -f $(TARGET) $(OBJS)

