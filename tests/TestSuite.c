#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <callback.h>
#include <time.h>

#include "TestSuite.h"
#include "Assertions.h"

int requiredIndentation = 0;
char *currentTest;

/**
 * Prints the appropriate amount of indentation for test related output.
 */
void printIndentation() {
  int timesIndented;

  for(timesIndented = 0; timesIndented < requiredIndentation; timesIndented++) {
    fprintf(stderr, "\t");
  }
}

/**
 * Starts a new describe block for all the tests that follow up until endDescribe() is 
 * called.  Nested describe blocks are allowed and will be cause an extra level of
 * indentation.  Below is an example of 3 nested describe blocks:
 *  Description 1
 *    Test 1a
 *    Description 2
 *      Test 2a
 *      Description 3
 *        Test 3a
 *
 * @param suiteDescription (char *) - a string describing the block of tests that follow
 */
void describe(char *suiteDescription) {
  printIndentation();
  fprintf(stderr, "%s\n", suiteDescription);

  requiredIndentation++;
}

/**
 * Ends the last describe block created.  This will descrease the indentation of all
 * testing output.  An error will be thrown if endDescribe() does not have a matching
 * describe().
 *
 * @param fileName (char *) - the name of the file calling endDescribe()
 * @param lineNumber (int) - the lineNumber of the call to endDescribe()
 */
void _endDescribe(char *fileName, int lineNumber) {
  if(--requiredIndentation < 0) {
    _fail("endDescribe() called too many times.", fileName, lineNumber);
  }
}

/**
 * Runs a test with the provided description.  If the test does not fail before finishing,
 * the test is assumed to pass.  If the test passes, the test execution time is recorded
 * and printed along with the test description.
 *
 * @param testDescription (char *) - the description of the test about to run
 * @param testFunc (void (*function)()) - the test to execute
 */
void test(char *testDescription, void (*testFunc)()) {
  struct timespec stopwatchStart;
  struct timespec stopwatchStop;

  currentTest = testDescription;

  clock_gettime(CLOCK_MONOTONIC, &stopwatchStart);
  testFunc();
  clock_gettime(CLOCK_MONOTONIC, &stopwatchStop);

  double startTime = (double) stopwatchStart.tv_sec + (1.0e-9 * stopwatchStart.tv_nsec);
  double endTime = (double) stopwatchStop.tv_sec + (1.0e-9 * stopwatchStop.tv_nsec);

  printIndentation();
  fprintf(stderr, TC_SUCCESS_START "%s (%.5f)" TC_SUCCESS_END "\n", currentTest, (endTime - startTime));
}

/**
 * Fails the currently running test, printing the reason for the failure, and exits the
 * program.
 *
 * @param reason (char *) - the reason for the test failed
 * @param fileName (char *) - the name of the file were the error occurred
 * @param lineNumber (int) - the number of the line that caused the test to fail
 */
void _fail(char *reason, char *fileName, int lineNumber) {
  printIndentation();
  fprintf(stderr, TC_FAIL_START "%s" TC_FAIL_END "\n", currentTest);

  printIndentation();
  fprintf(stderr, "\t%s (%s:%d)\n", reason, fileName, lineNumber);

  exit(-1);
}

/**
 * Creates and returns an assertion that can be used to test a value against the expected
 * value.  For example, to test the return value of a function creatively named
 * funcToTest, one could use the following code `expect(&funcToTest())->toBe->False()`.
 * Instead of calling this function directly, one should prefer the `expect()` macro.
 *
 * @param value (void *) - a pointer to the value to test.  If the value is a string, it
 *  MUST be NULL-terminated.
 * @param fileName (char *) - the name of the file calling this function
 * @param lineNumber (int) - the line number of the call to this function
 *
 * @return (Assertion) an assertion that can be used to compare a value with the expected
 *  value
 */
Assertion *_expect(void *value, char *fileName, int lineNumber) {
  Assertions *positiveAssert = (Assertions *) __real_malloc(2 * sizeof(Assertions));
  Assertions *negativeAssert = positiveAssert + 1;

  struct AssertionClosureData *positiveClosureData = (struct AssertionClosureData *) __real_malloc(2 * sizeof(struct AssertionClosureData));
  positiveClosureData->value = value;
  positiveClosureData->negate = false;
  positiveClosureData->fileName = fileName;
  positiveClosureData->lineNumber = lineNumber;

  struct AssertionClosureData *negativeClosureData = positiveClosureData + 1;
  memcpy(negativeClosureData, positiveClosureData, sizeof(struct AssertionClosureData));
  negativeClosureData->negate = true;

  positiveAssert->False = alloc_callback(&expectFalse, (void *) positiveClosureData);
  positiveAssert->True = alloc_callback(&expectTrue, (void *) positiveClosureData);
  positiveAssert->inRange = alloc_callback(&expectInRange, (void *) positiveClosureData);
  positiveAssert->equal = alloc_callback(&expectEqual, (void *) positiveClosureData);
  positiveAssert->equalStr = alloc_callback(&expectEqualStr, (void *) positiveClosureData);
  positiveAssert->null = alloc_callback(&expectNull, (void *) positiveClosureData);

  negativeAssert->False = alloc_callback(&expectFalse, (void *) negativeClosureData);
  negativeAssert->True = alloc_callback(&expectTrue, (void *) negativeClosureData);
  negativeAssert->inRange = alloc_callback(&expectInRange, (void *) negativeClosureData);
  negativeAssert->equal = alloc_callback(&expectEqual, (void *) negativeClosureData);
  negativeAssert->equalStr = alloc_callback(&expectEqualStr, (void *) negativeClosureData);
  negativeAssert->null = alloc_callback(&expectNull, (void *) negativeClosureData);

  Assertion *assertion = (Assertion *) __real_malloc(sizeof(Assertion));
  assertion->to = positiveAssert;
  assertion->toBe = positiveAssert;
  assertion->notTo = negativeAssert;
  assertion->notToBe = negativeAssert;

  return assertion;
}