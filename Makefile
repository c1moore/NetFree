CC = gcc

INCLUDES = $(wildcard ./includes/*.h)
FILES = $(wildcard ./*.c)
CFLAGS = -I ./includes/ -lpthread -lpcap

all: $(FILES) $(INCLUDES)
	$(CC) $(FILES) -o ./netfree $(CFLAGS)

clean:
	rm -f ./*.o ./adhawk