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
#include <stdbool.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <errno.h>

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
    free(systemFileName);
    
    return -1;
  }

  addressFile = fopen(systemFileName, "r");
  if(!addressFile) {
    free(systemFileName);

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
 * Obtains the MAC address of the router to which this device is connected.  
 *
 * @param routerMac (char *) - a pointer to at least NETFREE_MAC_SIZE of memory where the
 *  router's MAC address can be stored.  This string will NOT be NULL-terminated.
 *
 * @return (int) the status of this operation upon completion.  If successful, 0 will be
 *  returned.  Otherwise, a nonzero integer will be returned.
 */
int getRouterMacAddress(char *routerMac) {
  FILE *cmdOutput;
  char letter;

  cmdOutput = popen("/usr/sbin/arp -a", "r");
  if(cmdOutput == NULL) {
    fprintf(stderr, "Could not determine router MAC.\n");

    return -1;
  }

  // Eat the router name.
  do {
    letter = (char) fgetc(cmdOutput);
  } while(letter != ' ');

  // Eat the router IP address.
  do {
    letter = (char) fgetc(cmdOutput);
  } while(letter != ' ');

  // Eat the word "at".
  do {
    letter = (char) fgetc(cmdOutput);
  } while(letter != ' ');

  if(fscanf(cmdOutput, NETFREE_MAC_READ_REGEX, NETFREE_WR_TO_MAC(routerMac)) == EOF) {
    pclose(cmdOutput);

    return -1;
  }

  pclose(cmdOutput);

  return 0;
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

/**
 * Returns whether the 2 MAC addresses are exactly equal.
 *
 * @param leftOperand (char *) - the first MAC address to compare
 * @param rightOperand (char *) - the second MAC address to compare
 *
 * @return true iff the MAC address are equal, false otherwise
 */
int macEquals(char *leftOperand, char *rightOperand) {
  char octetIndex = 0;

  while(octetIndex < NETFREE_MAC_SIZE) {
    if(leftOperand[octetIndex] != rightOperand[octetIndex]) {
      return false;
    }

    octetIndex++;
  }

  return true;
}

/**
 * Sets this device's MAC address to the one specified by newMacAddress.
 *
 * @param newMacAddress (char *) - the new MAC address to which the device's MAC address
 *  should be set
 *
 * @return (int) the status of the operation.  0 is returned on success.  On failure, a
 *  nonzero integer is returned.
 */
int setDeviceMacAddress(char *newMacAddress) {
  struct ifreq ifr;
  int sock;

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock == -1) {
    fprintf(stderr, "Could not set new MAC address.\n");

    return -1;
  }

  strcpy(ifr.ifr_name, iface);
  ifr.ifr_hwaddr.sa_data[0] = newMacAddress[0];
  ifr.ifr_hwaddr.sa_data[1] = newMacAddress[1];
  ifr.ifr_hwaddr.sa_data[2] = newMacAddress[2];
  ifr.ifr_hwaddr.sa_data[3] = newMacAddress[3];
  ifr.ifr_hwaddr.sa_data[4] = newMacAddress[4];
  ifr.ifr_hwaddr.sa_data[5] = newMacAddress[5];
  ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

  errno = 0;
  if(ioctl(sock, SIOCSIFHWADDR, &ifr) == -1) {
    fprintf(stderr, "Could not set new MAC address.\n");
    perror(strerror(errno));
    fprintf(stderr, "%d\n", errno);

    return -2;
  }

  return 0;
}