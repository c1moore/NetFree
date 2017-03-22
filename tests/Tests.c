#include "TestSuite.h"
#include "MacTests.h"

int main() {
  initTests();

  addMacTests();

  executeTests();
}