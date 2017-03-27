#ifndef _NETFREE_SCANNER
  #define _NETFREE_SCANNER

  #define NETFREE_MIN_ADDRESSES 5

  extern int  initScanner(char *);
  extern void destroyScanner();
  extern void scan();
#endif