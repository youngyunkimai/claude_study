CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -I../framework
TARGET  = test_runner
SRCS    = test_mem_pool.c mem_pool.c
OBJS    = $(SRCS:.c=.o)

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
