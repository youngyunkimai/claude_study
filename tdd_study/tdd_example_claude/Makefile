CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g
TARGET  = test_runner
SRCS    = test_calculator.c calculator.c
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
