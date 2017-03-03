CC = gcc

INCLUDES = $(wildcard ./includes/*.h)
TEST_INCLUDES = $(wildcard ./tests/includes/*.h)
FILES = $(wildcard ./*.c)
TEST_FILES = $(wildcard ./tests/*.c)
CFLAGS = -I ./includes/ -lpthread -lpcap
TEST_CFLAGS = -I ./includes/ -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,realloc -Wl,-wrap,free

all: $(FILES) $(INCLUDES)
	$(CC) $(FILES) -o ./netfree $(CFLAGS)

test: $(FILES) $(TEST_FILES) $(INCLUDES) $(TEST_INCLUDES)
	$(CC) $(FILES) $(TEST_FILES) -o ./test_netfree $(CFLAGS) $(TEST_CFLAGS)

clean:
	rm -f ./*.o ./netfree