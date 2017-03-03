#ifndef _C1MOORE_TEST_SUITE
  #define _C1MOORE_TEST_SUITE 

  /* Testing colors. */
  #define TC_RESET          "\033[0m"
  #define TC_SUCCESS_START  "\033[1;32mOK\t\033[0;32m"
  #define TC_FAIL_START     "\033[1;31mX\t\033[0;31m"
  #define TC_SUCCESS_END    TC_RESET
  #define TC_FAIL_END       TC_RESET

  /* Memory Tracking */
  typedef struct memoryRefStruct MemoryRef;
  struct memoryRefStruct {
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
#endif