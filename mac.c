/**
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#include "includes/mac.h"

char *originalMacAddress = NULL;
char *iface = NULL;

/**
 * Attempts to determine the network interface's MAC address using system files.
 *
 * @param macAddress (char *) - the location where the network interface's MAC address should
 *  be stored.  The location should be large enough to hold a MAC address and a NULL-
 *  terminator.
 *
 * @return (int) On success, 0 will be returned.  Otherwise, a nonzero value will be returned
 */
int getCurrentMacAddressBySystemFile(char *macAddress) {
  char *systemFileName;
  char *fileComponentStart;
  FILE *addressFile;

  // Construct the filepath to the file that should contain the MAC address.
  systemFileName = (char *) malloc((strlen(NETFREE_SYSTEM_NET_DIR) + strlen(iface) + strlen(NETFREE_SYSTEM_ADD_FILE) + 1) * sizeof(char));
  strcpy(systemFileName, NETFREE_SYSTEM_NET_DIR);
  fileComponentStart = systemFileName + strlen(NETFREE_SYSTEM_NET_DIR);
  strcpy(fileComponentStart, iface);
  fileComponentStart = fileComponentStart + strlen(iface);
  strcpy(fileComponentStart, NETFREE_SYSTEM_ADD_FILE);

  // Open and read the file.
  if(access(systemFileName, R_OK)) {
    return -1;
  }

  addressFile = fopen(systemFileName, "r");
  if(!addressFile) {
    return -1;
  }

  char *macChar = macAddress;
  while(fscanf(addressFile, "%2hhx", macChar) == 1) {
    // Eat the colon separator.
    fscanf(addressFile, ":");

    macChar++;
  }

  macAddress[NETFREE_MAC_SIZE] = 0;

  fclose(addressFile);
  free(systemFileName);

  return 0;
}

/**
 * Attempts to determine the network interface's MAC address by opening a socket and
 * the MAC address assigned to that socket.
 *
 * @param macAddress (char *) - the location where the network interface's MAC address should
 *  be stored.  The location should be large enough to hold a MAC address and a NULL-
 *  terminator.
 *
 * @return (int) On success, 0 will be returned.  Otherwise, a nonzero value will be returned
 */
int getCurrentMacAddressBySocket(char *macAddress) {
  struct ifreq ifr;
  int sock;
  int success = 1;

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if(sock == -1) {
    return -1;
  }

  strcpy(ifr.ifr_name, iface);

  if(!ioctl(sock, SIOCGIFHWADDR, &ifr)) {
    int index;
    for(index = 0; index < 6; index++) {
      macAddress[index] = ifr.ifr_addr.sa_data[index];
    }

    success = 0;
  }

  shutdown(sock, SHUT_RDWR);
  close(sock);

  return success;
}

/**
 * Initializes the MAC address system and records the MAC address for the specified
 * network interface.  This function must be called before any other operation within this
 * file is executed.
 *
 * @param netInterface (char *) - a pointer to a NULL-terminated char string that specifies
 *  the network interface that should be used when determining the MAC address
 */
void initMac(char *netInterface) {
  if(!originalMacAddress) {
    iface = (char *) malloc(strlen(netInterface) + 1);
    strcpy(iface, netInterface);

    originalMacAddress = (char *) malloc((NETFREE_MAC_SIZE + 1) * sizeof(char));
    getCurrentMacAddress(originalMacAddress);
  }
}

/**
 * Frees all memory held by the MAC address system.
 */
void destroyMac() {
  free(iface);
  free(originalMacAddress);
}

/**
 * Returns the current MAC address for the specified network interface.
 *
 * @param macAddress (char *) - the location at which to store the network interface's MAC
 *  address.  The location should be large enough to hold a MAC address and a NULL-
 *  terminator.
 *
 * @return (int) On success, 0 will be returned.  Otherwise, a nonzero value will be returned
 */
int getCurrentMacAddress(char *macAddress) {
  int status;

  if(!macAddress) {
    return -1;
  }

  status = getCurrentMacAddressBySystemFile(macAddress);
  if(status) {
    status = getCurrentMacAddressBySocket(macAddress);
  }

  return status;
}

/**
 * Returns the original MAC address, which is determined by the first call to
 * initMacAddress().
 *
 * @param originalMacAddr (char *) - the location at which to store the network interface's
 *  original MAC address.  The location should be large enough to hold a MAC address and a
 *  NULL-terminator.
 *
 * @return (int) On success, returns 0.  If originalMacAddr is NULL -1 is returned.
 */
int getOriginalMacAddress(char *originalMacAddr) {
  if(!originalMacAddress) {
    return -1;
  }

  strcpy(originalMacAddr, originalMacAddress);
  return 0;
}