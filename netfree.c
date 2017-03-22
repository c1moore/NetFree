#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "netfree.h"
#include "mac.h"
#include "scanner.h"

int main(int argc, char **argv) {
  char *iface;

  if(argc == 1) {
    fprintf(stdout, "Using default iface: " DEFAULT_IFACE "\n");

    iface = (char *) malloc((strlen(DEFAULT_IFACE) + 1) * sizeof(char));
    strcpy(iface, DEFAULT_IFACE);
  } else {
    fprintf(stdout, "Using iface: %s\n", argv[1]);

    iface = argv[1];
  }

  initMac(iface);

  char macAddress[7];
  getCurrentMacAddress(macAddress);
  fprintf(stderr, "Original MAC Address: %.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx\n", macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5]);

  initScanner(iface);

  scan();
}