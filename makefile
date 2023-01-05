EXEC=shell.exe
SOURCES=shell.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-W -Wall -std=gnu99 -g
 
.PHONY: default clean
 
default: $(EXEC)
 
main.o: main.c
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^

clean:
	rm -rf $(EXEC) $(OBJECTS) $(SOURCES:.c=.c~) Makefile~
