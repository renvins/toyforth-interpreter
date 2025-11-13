CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -g
SRCS = main.c mem.c parser.c list.c stack.c primitives.c dict.c
OBJS = $(SRCS:.c=.o)
BIN  = toyforth

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $<

test: $(BIN)
	./run_tests.sh

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all run test clean