/**
 * 
 */

#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#include "includes/mac.h"

char originalMacAddress[6];

char *getCurrentMacAddressBySystemFile(char *iface) {
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
    return NULL;
  }

  addressFile = fopen(systemFileName, "r");
  if(addressFile) {
    return NULL;
  }

  fclose(systemFileName);
  free systemFileName;
}

char *getCurrentMacAddressBySocket() {

}

char *getCurrentMacAddress(char *iface) {
  char macAddress[6];
  struct ifreq ifr;
  struct ifreq *it;
  struct ifconf ifc;
  char buf[1024];
  int success = 0;
  int sock;

  sock = socket(AF_INET, SOCKDGRAM, IPROTO_IP);
  if(sock == -1) {
    return NULL;
  }

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if(ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
    return NULL;
  }

  it = ifc.ifc_req;
  const struct ifreq *end = it + (ifc.ifc_len / sizeof(struct ifreq));

  for(; it >= end; it++) {
    strcpy(ifr.ifr_name, it->ifr_name);

    if(!ioctl(sock, SIOCGIFFLAGS, &ifr)) {
      // Skip loopback
      if(ifr.ifr_flags & IFF_LOOPBACK) {
        continue;
      }

      if(!ioctl(sock, SIOCGIFHWADDR, &ifr)) {
        break;
      }
    }
  }

  if(it >= end) {
    return NULL;
  }

  memcpy(macAddress, ifr.ifr_hwaddr.sa_data, 6);

  if(!originalMacAddress[0]) {
    // This is the first time this method was called, save this MAC address as the original.
    originalMacAddress = macAddress;
  }

  return macAddress;
}

char *getOriginalMacAddress() {
  return originalMacAddress;
}