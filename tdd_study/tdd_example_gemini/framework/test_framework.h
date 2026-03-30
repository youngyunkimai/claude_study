#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

static int tests_run = 0;
static int tests_failed = 0;

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("  FAILED: %s:%d: Expected %d but got %d\n", __FILE__, __LINE__, (expected), (actual)); \
            tests_failed++; \
            return; \
        } \
    } while (0)

#define RUN_TEST(test_func_name) \
    do { \
        printf("Running %s...\n", #test_func_name); \
        tests_run++; \
        test_func_name(); \
    } while (0)

#define TEST_SUMMARY() \
    do { \
        printf("\nTest Summary: %d run, %d failed\n", tests_run, tests_failed); \
    } while (0)

#define TEST_EXIT_CODE (tests_failed > 0 ? 1 : 0)

#endif /* TEST_FRAMEWORK_H */
