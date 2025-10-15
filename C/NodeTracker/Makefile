CC = gcc
CFLAGS = -std=c17 -I./Hashtable -I./Hash

SRC = node.c Hashtable/hashtable.c
EXEC = cnode

OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(EXEC) $(OBJ)