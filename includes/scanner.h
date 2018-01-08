#ifndef _NETFREE_SCANNER
  #define _NETFREE_SCANNER

  #define NETFREE_MIN_ADDRESSES 5

  #ifndef PCAP_BUF_SIZE
    #define PCAP_BUF_SIZE 5000
  #endif

  #ifndef PCAP_TIMEOUT_MS
    #define PCAP_TIMEOUT_MS 5000
  #endif

  extern int  initScanner(char *);
  extern void destroyScanner();
  extern void scan();
#endif