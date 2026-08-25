/*
 * unity.h - Minimal Unity-compatible test framework for struct2json
 *
 * Provides Unity-style TEST_ASSERT macros and RUN_TEST runner.
 * Self-contained, no external dependencies.
 *
 * For full Unity framework, see https://github.com/ThrowTheSwitch/Unity
 */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* ---- Internal state ---- */

static int unity_tests_run    = 0;
static int unity_tests_failed = 0;
static const char *unity_current_test = NULL;
static int unity_test_failed_flag     = 0;

/* ---- Assertions ---- */

#define TEST_ASSERT_TRUE(condition) do { \
    if (!(condition)) { \
        printf("  FAIL: %s (line %d)\n    Expected TRUE: %s\n", \
               unity_current_test, __LINE__, #condition); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_FALSE(condition) \
    TEST_ASSERT_TRUE(!(condition))

#define TEST_ASSERT_NULL(pointer) do { \
    if ((pointer) != NULL) { \
        printf("  FAIL: %s (line %d)\n    Expected NULL: %s\n", \
               unity_current_test, __LINE__, #pointer); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_NOT_NULL(pointer) do { \
    if ((pointer) == NULL) { \
        printf("  FAIL: %s (line %d)\n    Expected non-NULL: %s\n", \
               unity_current_test, __LINE__, #pointer); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) do { \
    long long _e = (long long)(expected); \
    long long _a = (long long)(actual); \
    if (_e != _a) { \
        printf("  FAIL: %s (line %d)\n    Expected %lld, got %lld\n", \
               unity_current_test, __LINE__, _e, _a); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) do { \
    const char *_e = (expected); \
    const char *_a = (actual); \
    if (_e == NULL || _a == NULL || strcmp(_e, _a) != 0) { \
        printf("  FAIL: %s (line %d)\n    Expected \"%s\", got \"%s\"\n", \
               unity_current_test, __LINE__, \
               _e ? _e : "(null)", _a ? _a : "(null)"); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual, delta) do { \
    double _e = (double)(expected); \
    double _a = (double)(actual); \
    if (fabs(_e - _a) > (double)(delta)) { \
        printf("  FAIL: %s (line %d)\n    Expected %f +/- %f, got %f\n", \
               unity_current_test, __LINE__, _e, (double)(delta), _a); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) do { \
    if (memcmp((expected), (actual), (len)) != 0) { \
        printf("  FAIL: %s (line %d)\n    Memory mismatch (%zu bytes)\n", \
               unity_current_test, __LINE__, (size_t)(len)); \
        unity_test_failed_flag = 1; \
        return; \
    } \
} while (0)

/* ---- Test runner ---- */

#define RUN_TEST(func) do { \
    unity_tests_run++; \
    unity_test_failed_flag = 0; \
    unity_current_test = #func; \
    printf("  [RUN ] %s\n", #func); \
    func(); \
    if (unity_test_failed_flag) { \
        unity_tests_failed++; \
        printf("  [FAIL] %s\n", #func); \
    } else { \
        printf("  [PASS] %s\n", #func); \
    } \
} while (0)

/* ---- Summary ---- */

static inline int unity_end(void) {
    printf("\n========================================\n");
    printf("  Tests: %d  Passed: %d  Failed: %d\n",
           unity_tests_run,
           unity_tests_run - unity_tests_failed,
           unity_tests_failed);
    printf("========================================\n");
    return unity_tests_failed;
}

#endif /* UNITY_FRAMEWORK_H */
