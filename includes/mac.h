#ifndef _NETFREE_MAC_INTERFACE
  #define _NETFREE_MAC_INTERFACE

  #define NETFREE_MAC_SIZE          6   // Number of bytes in used to store a MAC address
  #define NETFREE_MAC_REGEX         "%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx"
  #define NETFREE_ARR_TO_MAC(chArr) chArr[0], chArr[1], chArr[2], chArr[3], chArr[4], chArr[5]
  #define NETFREE_MAC_READ_REGEX    "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"
  #define NETFREE_WR_TO_MAC(chArr)  &chArr[0], &chArr[1], &chArr[2], &chArr[3], &chArr[4], &chArr[5]
  /**
   * The following path directs the program to the system directory that could
   * contain the system's MAC address, which will be used before other methods.
   * The path should start and end with a forward slash ("/").
   */
  #define NETFREE_SYSTEM_NET_DIR    "/sys/class/net/"
  /**
   * The file within the particular system network interface directory that
   * contains the system's MAC address.  This string should start with a
   * forward slash ("/");
   */
  #define NETFREE_SYSTEM_ADD_FILE   "/address"

  extern void initMac(char *netInterface);
  extern void destroyMac();
  extern int  getCurrentMacAddress(char *macAddress);
  extern int  getOriginalMacAddress(char *originalMacAddr);
  extern int  getRouterMacAddress(char *routerMac);
  extern int  macEquals(char *, char *);
  extern int  setDeviceMacAddress(char *);
#endif