CC = gcc

INCLUDES = $(wildcard ./includes/*.h)
TEST_INCLUDES = $(wildcard ./tests/includes/*.h)
FILES = $(wildcard ./*.c)
TEST_FILES = $(filter-out ./netfree.c, $(wildcard ./tests/*.c) $(FILES))
CFLAGS = -I ./includes/ -lpthread -lpcap -lcurl
TEST_CFLAGS = -I ./tests/includes/ -Wl,-wrap,malloc -Wl,-wrap,calloc -Wl,-wrap,realloc -Wl,-wrap,free -lcallback -ltrampoline -lavcall -lvacall

all: $(FILES) $(INCLUDES)
	$(CC) $(FILES) -o ./bin/netfree $(CFLAGS)

test: $(TEST_FILES) $(INCLUDES) $(TEST_INCLUDES)
	$(CC) $(TEST_FILES) -o ./bin/test_netfree $(CFLAGS) $(TEST_CFLAGS)

clean:
	rm -f ./bin/*