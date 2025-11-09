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

run: $(BIN)
	./$(BIN) tests/test.tf

clean:
	rm -f $(OBJS) $(BIN)