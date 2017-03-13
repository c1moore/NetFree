#ifndef _C1MOORE_TEST_ASSERTIONS
  #define _C1MOORE_TEST_ASSERTIONS

  extern void expectFalse(void *data);
  extern void expectTrue(void *data);
  extern void expectInRange(void *data, va_alist args);
  extern void expectEqual(void *data, va_alist args);
  extern void expectEqualStr(void *data, va_alist args);
  extern void expectNull(void *data);

#endif