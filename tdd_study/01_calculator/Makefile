CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -I../framework
TARGET  = test_runner
SRCS    = test_calculator.c calculator.c
OBJS    = $(SRCS:.c=.o)

.PHONY: test clean

# 'make test' = 빌드 + 실행
# 테스트가 실패하면 make도 실패 (비-0 exit code)
test: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
