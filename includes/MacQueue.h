#ifndef _NETFREE_MAC_QUEUE
  #define _NETFREE_MAC_QUEUE

  extern void initMacQueue();
  extern void destroyMacQueue();
  extern void enqueueMac(char *, double);
  extern char *macQueuePeek(char *);
  extern int  macQueueLength();
  extern char *dequeueMac(char *);
#endif