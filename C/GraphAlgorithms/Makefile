CC = gcc
CFLAGS = -std=c17 -Wall -Iinclude

SRC = graph.c is_connected.c \
      include/array_2d.c include/dlist.c include/queue.c include/list.c

OBJ = $(SRC:.c=.o)

EXEC = is_connected

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
