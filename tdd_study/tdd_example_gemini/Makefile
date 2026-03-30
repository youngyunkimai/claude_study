CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -I. -Iframework

SRC = calculator.c
TEST_SRC = tests/test_calculator.c
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)

TARGET = test_runner

all: $(TARGET)

$(TARGET): $(OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET)

.PHONY: all test clean
