#ifndef _NETFREE_MAC_INTERFACE
  #define _NETFREE_MAC_INTERFACE

  #define NETFREE_MAC_SIZE          6   // Number of bytes in used to store a MAC address
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
  extern int getCurrentMacAddress(char *macAddress);
  extern int getOriginalMacAddress(char *originalMacAddr);
#endif