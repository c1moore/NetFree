#ifndef _C1MOORE_TEST_SUITE
  #define _C1MOORE_TEST_SUITE 

  #include <stdbool.h>
  #include <stdlib.h>
  #include <callback.h>

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
  #define endDescribe()     _endDescribe(__FILE__, __LINE__)
  #define before(func)      _before(func, __FILE__, __LINE__)
  #define beforeEach(func)  _beforeEach(func, __FILE__, __LINE__)
  #define after(func)       _after(func, __FILE__, __LINE__)
  #define afterEach(func)   _afterEach(func, __FILE__, __LINE__)

  extern void initTests();
  extern void describe(char *);
  extern void _before(void (*)(), char *, int);
  extern void _beforeEach(void (*)(), char *, int);
  extern void test(char *, void (*)());
  extern void _afterEach(void (*)(), char *, int);
  extern void _after(void (*)(), char *, int);
  extern void _endDescribe(char *, int);
  extern void executeTests();

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