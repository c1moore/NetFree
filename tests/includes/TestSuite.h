#ifndef _C1MOORE_TEST_SUITE
  #define _C1MOORE_TEST_SUITE 

  #include <stdbool.h>

  /* Testing colors. */
  #define TC_RESET          "\033[0m"
  #define TC_SUCCESS_COLOR  "\033[1;32m"
  #define TC_FAIL_COLOR     "\033[1;31m"
  #define TC_SUCCESS_START  "\033[1;32mOK\t\033[0;32m"
  #define TC_FAIL_START     "\033[1;31mX\t\033[0;31m"
  #define TC_SUCCESS_END    TC_RESET
  #define TC_FAIL_END       TC_RESET

  /* Memory Tracking */
  typedef struct MemoryRefStruct MemoryRef;
  struct MemoryRefStruct {
    void *ptr;
    int bytes;
    MemoryRef *next;
    MemoryRef *prev;
  };

  extern void  resetMemoryTracking();
  extern int   totalUnfreedMemory();
  extern void *__real_malloc  (size_t bytes);
  extern void *__real_realloc (void *ptr, size_t bytes);
  extern void *__real_calloc  (size_t nitems, size_t size);
  extern void  __real_free    (void *ptr);
  extern void *__wrap_malloc  (size_t bytes);
  extern void *__wrap_realloc (void *ptr, size_t bytes);
  extern void *__wrap_calloc  (size_t nitems, size_t size);
  extern void  __wrap_free    (void *ptr);

  /* Test Suite Functions */
  #define endDescribe() _endDescribe(__FILE__, __LINE__)

  extern void describe(char *suiteDescription);
  extern void before(void (*beforeFunc)());
  extern void beforeEach(void (*beforeEachFunc)());
  extern void test(char *testDescription, void (*testFunc)());
  extern void afterEach(void (*afterEachFunc)());
  extern void after(void (*afterFunc)());
  extern void _endDescribe(char *, int);

  /* Test Helpers */
  #define expect(val)   _expect(val, __FILE__, __LINE__)
  #define fail(reason)  _fail(reason, __FILE__, __LINE__)

  struct AssertionClosureData {
    void *value;
    bool  negate;
    char *fileName;
    int   lineNumber;
  };

  typedef struct AssertionsStruct Assertions;
  struct AssertionsStruct {
    int (*False)();
    int (*True)();
    int (*inRange)(int, int);
    int (*equal)(int);
    int (*equalStr)(const char *);
    int (*null)();
  };

  typedef struct AssertionVerbStruct Assertion;
  struct AssertionVerbStruct {
    Assertions *to;
    Assertions *toBe;
    Assertions *notTo;
    Assertions *notToBe;
  };

  extern Assertion  *_expect(void *, char *, int);
  extern void       _fail(char *, char *, int);
#endif