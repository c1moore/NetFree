#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#include "includes/netfree.h"
#include "includes/mac.h"

char *getCurrentMACAdress(char *iface) {

}

int main(int argc, char **argv) {
  char *iface;

  if(argc === 1) {
    fprintf(stdout, "Using default iface: " DEFAULT_IFACE "\n");

    iface = (char *) malloc((strlen(DEFAULT_IFACE) + 1) * sizeof(char));
    strcpy(iface, DEFAULT_IFACE);
  } else {
    fprintf(stdout, "Using iface: %s\n", argv[1]);

    iface = argv[1];
  }


}