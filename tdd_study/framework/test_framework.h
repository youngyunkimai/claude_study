/*
 * test_framework.h - Minimal TDD Framework for C
 *
 * ============================================================
 * DESIGN PHILOSOPHY
 * ============================================================
 * A test framework must be invisible. It should get out of the
 * way and let the tests speak. This framework uses only the C
 * preprocessor and stdio.h. No heap allocation. No init function.
 *
 * The entire framework is a single header to eliminate build
 * complexity. When learning TDD, friction in toolchain = enemy.
 *
 * ============================================================
 * THE THREE LAWS OF TDD (Uncle Bob Martin)
 * ============================================================
 * Law 1: Do not write production code unless it is to make a
 *        failing test pass.
 * Law 2: Write only enough test code to fail. Compilation
 *        failure counts as failing.
 * Law 3: Write only enough production code to make the failing
 *        test pass.
 *
 * ============================================================
 * RED - GREEN - REFACTOR CYCLE
 * ============================================================
 *   RED      Write a test that fails (because code doesn't exist yet)
 *   GREEN    Write the minimum code to make the test pass
 *   REFACTOR Improve structure while keeping all tests GREEN
 *
 * ============================================================
 * FIRST PRINCIPLES for good tests
 * ============================================================
 *   Fast        Tests run in milliseconds, not seconds
 *   Independent Each test stands alone; order does not matter
 *   Repeatable  Same result every run, no external state
 *   Self-validating PASS or FAIL, no manual inspection needed
 *   Timely      Written just before the production code
 *
 * ============================================================
 * TEST NAMING CONVENTION
 * ============================================================
 *   test_[unit]_[scenario]_[expected_behavior]
 *
 *   Example: test_add_two_positives_returns_sum
 *            test_rb_push_when_full_returns_error
 *            test_pool_alloc_when_exhausted_returns_null
 *
 * ============================================================
 * USAGE
 * ============================================================
 *   1. #include "../../framework/test_framework.h"
 *   2. Write test functions: void test_something(void) { ... }
 *   3. In main(): RUN_TEST(test_something);
 *   4. At end of main(): TEST_SUMMARY(); return TEST_EXIT_CODE;
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

/* ANSI color codes for terminal output */
#define _TF_RED    "\033[0;31m"
#define _TF_GREEN  "\033[0;32m"
#define _TF_RESET  "\033[0m"

/* Global test counters (one set per test binary via static) */
static int _tests_run    = 0;
static int _tests_passed = 0;
static int _tests_failed = 0;

/* Per-test failure flag: set by assertions, cleared by RUN_TEST.
 * Using a flag (instead of longjmp/setjmp) keeps the framework
 * simple and avoids stack-unwinding surprises. A test with multiple
 * failing assertions will report all of them. */
static int _test_failed_flag = 0;

/* ----------------------------------------------------------
 * ASSERTION MACROS
 * ----------------------------------------------------------
 * Each macro prints file:line on failure so you can jump directly
 * to the failing assertion. This is the "self-validating" F in FIRST.
 * ---------------------------------------------------------- */

/* Generic boolean assertion */
#define TEST_ASSERT(cond)                                              \
    do {                                                               \
        if (!(cond)) {                                                 \
            printf(_TF_RED "    FAIL: %s\n"                           \
                           "    at   %s:%d\n" _TF_RESET,              \
                   #cond, __FILE__, __LINE__);                         \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* Integer equality — prints expected vs actual on failure */
#define TEST_ASSERT_EQUAL_INT(expected, actual)                        \
    do {                                                               \
        int _e = (expected);                                           \
        int _a = (actual);                                             \
        if (_e != _a) {                                                \
            printf(_TF_RED "    FAIL: expected %d, got %d\n"          \
                           "    at   %s:%d\n" _TF_RESET,              \
                   _e, _a, __FILE__, __LINE__);                        \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* Pointer must be NULL */
#define TEST_ASSERT_NULL(ptr)                                          \
    do {                                                               \
        if ((void *)(ptr) != NULL) {                                   \
            printf(_TF_RED "    FAIL: expected NULL, got non-NULL\n"   \
                           "    at   %s:%d\n" _TF_RESET,              \
                   __FILE__, __LINE__);                                \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* Pointer must be non-NULL */
#define TEST_ASSERT_NOT_NULL(ptr)                                      \
    do {                                                               \
        if ((void *)(ptr) == NULL) {                                   \
            printf(_TF_RED "    FAIL: expected non-NULL, got NULL\n"   \
                           "    at   %s:%d\n" _TF_RESET,              \
                   __FILE__, __LINE__);                                \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* String equality */
#define TEST_ASSERT_EQUAL_STR(expected, actual)                        \
    do {                                                               \
        const char *_e = (expected);                                   \
        const char *_a = (actual);                                     \
        if (strcmp(_e, _a) != 0) {                                     \
            printf(_TF_RED "    FAIL: expected \"%s\", got \"%s\"\n"   \
                           "    at   %s:%d\n" _TF_RESET,              \
                   _e, _a, __FILE__, __LINE__);                        \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* size_t equality */
#define TEST_ASSERT_EQUAL_SIZE(expected, actual)                       \
    do {                                                               \
        size_t _e = (size_t)(expected);                                \
        size_t _a = (size_t)(actual);                                  \
        if (_e != _a) {                                                \
            printf(_TF_RED "    FAIL: expected %zu, got %zu\n"         \
                           "    at   %s:%d\n" _TF_RESET,              \
                   _e, _a, __FILE__, __LINE__);                        \
            _test_failed_flag = 1;                                     \
        }                                                              \
    } while (0)

/* ----------------------------------------------------------
 * RUN_TEST — execute one test function and track its result
 *
 * This macro:
 *   1. Clears the per-test failure flag (Independence: I in FIRST)
 *   2. Increments the run counter
 *   3. Prints the test name left-aligned in a fixed-width field
 *   4. Calls the test function
 *   5. Prints PASS (green) or FAIL (red) based on the flag
 * ---------------------------------------------------------- */
#define RUN_TEST(test_func)                                            \
    do {                                                               \
        _test_failed_flag = 0;                                         \
        _tests_run++;                                                  \
        printf("[RUN ] %-52s", #test_func);                            \
        fflush(stdout);                                                \
        test_func();                                                   \
        if (_test_failed_flag) {                                       \
            _tests_failed++;                                           \
            printf(_TF_RED "FAIL\n" _TF_RESET);                       \
        } else {                                                       \
            _tests_passed++;                                           \
            printf(_TF_GREEN "PASS\n" _TF_RESET);                     \
        }                                                              \
    } while (0)

/* ----------------------------------------------------------
 * TEST_SUMMARY — print final pass/fail totals
 * TEST_EXIT_CODE — return 0 if all passed, 1 if any failed
 *   (non-zero exit lets `make test` fail the build on test failure)
 * ---------------------------------------------------------- */
#define TEST_SUMMARY()                                                 \
    do {                                                               \
        printf("\n");                                                  \
        printf("======================================================\n"); \
        if (_tests_failed == 0)                                        \
            printf(_TF_GREEN "  ALL PASSED" _TF_RESET);               \
        else                                                           \
            printf(_TF_RED   "  FAILED" _TF_RESET);                   \
        printf("  |  Total: %2d  |  Passed: %2d  |  Failed: %2d\n",   \
               _tests_run, _tests_passed, _tests_failed);             \
        printf("======================================================\n"); \
    } while (0)

#define TEST_EXIT_CODE (_tests_failed ? 1 : 0)

#endif /* TEST_FRAMEWORK_H */
