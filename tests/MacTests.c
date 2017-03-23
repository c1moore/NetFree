#include "TestSuite.h"
#include "Assertions.h"
#include "mac.h"
#include "netfree.h"

void test_initMac_initOriginalMac() {
  int test = 1;

  expect(&test)->to->equal(1);
}

void beforeEach_destroyMac() {
  resetMemoryTracking();
}

void test_destroyMac_releaseMemory() {
  initMac(DEFAULT_IFACE);
  destroyMac();

  int totalMemoryLeaked = totalUnfreedMemory();
  expect(&totalMemoryLeaked)->to->equal(0);
}

void test_macEquals_equals() {
  #define _NETFREE_TEST_MAC_ADDRESS "abcdef"
  char *originalMacAddress = _NETFREE_TEST_MAC_ADDRESS;
  char *duplicateMacAddress = _NETFREE_TEST_MAC_ADDRESS;
  #undef _NETFREE_TEST_MAC_ADDRESS

  bool equals = macEquals(originalMacAddress, duplicateMacAddress);
  expect(&equals)->toBe->True();
}

void test_macEquals_doesNotEqual() {
  char *originalMacAddress = "abcdef";
  char *differentMacAddress = "123456";

  bool equals = macEquals(originalMacAddress, differentMacAddress);
  expect(&equals)->toBe->False();
}

void addMacTests() {
  describe("MAC Tests");
    describe("initMac()");
      test("should initialize the original MAC address", test_initMac_initOriginalMac);
    endDescribe();

    describe("destroyMac()");
      beforeEach(beforeEach_destroyMac);

      test("should release all memory allocated when initializing", test_destroyMac_releaseMemory);
    endDescribe();

    describe("macEquals()");
      test("should return true when both MAC address have the same MAC address", test_macEquals_equals);
      test("should return false when the MAC addresses do not have the same MAC address", test_macEquals_doesNotEqual);
    endDescribe();
  endDescribe();
}