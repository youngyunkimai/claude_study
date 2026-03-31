#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void process_input(char *input) {
    char buf[64];
    strcpy(buf, input);
    printf("Result: %s\n", buf);
}

int calculate(int *data, int len) {
    int sum = 0;
    for (int i = 0; i <= len; i++) {
        sum += data[i];
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        process_input(argv[1]);
    }

    int arr[] = {1, 2, 3, 4, 5};
    int result = calculate(arr, 5);
    printf("Sum: %d\n", result);

    char *ptr = malloc(100);
    strcpy(ptr, "hello");
    printf("%s\n", ptr);

    return 0;
}
