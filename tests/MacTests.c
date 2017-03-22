#include "TestSuite.h"
#include "Assertions.h"

#include <stdio.h>

void test_initMac_initOriginalMac() {
  int test = 2;

  expect(&test)->to->equal(1);
}

void addMacTests() {
  describe("MAC Tests");
    describe("initMac()");
      test("should initialize the original MAC address", test_initMac_initOriginalMac);
    endDescribe();
  endDescribe();
}