#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "TestSuite.h"
#include "Assertions.h"

#define ERROR_MESSAGE_LENGTH  100

/**
 * Expects the value passed to _expect() to be false.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 */
void expectFalse(void *data) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  if(*((bool *)closureData->value) != closureData->negate) {
    char expected[6];
    strcpy(expected, closureData->negate ? "true" : "false");

    char actual[6];
    strcpy(actual, *((bool *)closureData->value) ? "true" : "false");

    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%s" TC_FAIL_END" to be " TC_SUCCESS_COLOR "%s" TC_SUCCESS_END ".", actual, expected);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}

/**
 * Expects the value passed to _expect() to be true.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 */
void expectTrue(void *data) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  if(*((bool *)closureData->value) == closureData->negate) {
    char expected[6];
    strcpy(expected, closureData->negate ? "false" : "true");
    
    char actual[6];
    strcpy(actual, *((bool *)closureData->value) ? "true" : "false");

    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%s" TC_FAIL_END " to be " TC_SUCCESS_COLOR "%s" TC_SUCCESS_END ".", actual, expected);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}

/**
 * Expects the value passed to _expect() to be within the specified range.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 * @param args (va_alist) - varargs passed to the Assertion->inRange function.  The first
 *  expected arg is the lower bounds of the expected range while the second arg is the
 *  upper bounds of the expected range
 */
void expectInRange(void *data, va_alist args) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  va_start_int(args);
  int lowerBound = va_arg_int(args);
  int upperBound = va_arg_int(args);

  int actualValue = *((int *) closureData->value);
  bool isInRange = actualValue > lowerBound && actualValue < upperBound;

  if(isInRange == closureData->negate) {
    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%d" TC_FAIL_END " to be between " TC_SUCCESS_COLOR "%d" TC_SUCCESS_END " and " TC_SUCCESS_START "%d" TC_SUCCESS_END ".", actualValue, lowerBound, upperBound);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}

/**
 * Expects the value passed to _expect() to equal the specified value.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 * @param args (va_alist) - varargs passed to the Assertion->equal function.  The first
 *  and only arg is the expected value
 */
void expectEqual(void *data, va_alist args) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  va_start_int(args);
  int expectedValue = va_arg_int(args);

  int actualValue = *((int *) closureData->value);

  bool equals = expectedValue == actualValue;
  if(equals == closureData->negate) {
    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%d" TC_FAIL_END " to equal " TC_SUCCESS_COLOR "%d" TC_SUCCESS_END ".", actualValue, expectedValue);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}

/**
 * Expects the value passed to _expect() to equal the specified string.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 * @param args (va_alist) - varargs passed to the Assertion->equal function.  The first
 *  and only arg is a (char *) to the expected null-terminated string
 */
void expectEqualStr(void *data, va_alist args) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  va_start_int(args);
  char *expectedValue = va_arg_ptr(args, char *);

  char *actualValue = (char *) closureData->value;

  bool equals = !strcmp(expectedValue, actualValue);
  if(equals == closureData->negate) {
    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%s" TC_FAIL_END " to equal " TC_SUCCESS_COLOR "%s" TC_SUCCESS_END ".", actualValue, expectedValue);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}

/**
 * Expects the value passed to _expect() to equal NULL.
 *
 * @param data (AssertionClosureData) - closure data necessary passed from the trampoline
 *  or callback functions
 */
void expectNull(void *data) {
  struct AssertionClosureData *closureData = (struct AssertionClosureData *) data;

  bool isNull = (NULL == closureData->value);

  if(isNull == closureData->negate) {
    char errorMessage[ERROR_MESSAGE_LENGTH];
    sprintf(errorMessage, "Expected " TC_FAIL_COLOR "%p" TC_FAIL_END " to equal " TC_SUCCESS_COLOR "NULL" TC_SUCCESS_END ".", closureData->value);

    _fail(errorMessage, closureData->fileName, closureData->lineNumber);
  }
}