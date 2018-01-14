#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "scanner.h"
#include "HeaderParser.h"
#include "MacQueue.h"
#include "mac.h"

pcap_t     *pcapDevHandle;
pthread_t   scannerThread;

char *deviceMacAddress;
char *routerMacAddress;

/**
 * Initializes the scanner and prepares it for use later.
 */
int initScanner(char *iface) {
  int                 status;
  char                pcapError[PCAP_ERRBUF_SIZE];

  struct bpf_program  pcapFilter;

  bpf_u_int32         netAddr = 0,
                      netMask = 0;

  scannerThread = (pthread_t) 0;

  deviceMacAddress = (char *) malloc(NETFREE_MAC_SIZE);
  getOriginalMacAddress(deviceMacAddress);

  routerMacAddress = (char *) malloc(NETFREE_MAC_SIZE);
  getRouterMacAddress(routerMacAddress);

  pcapDevHandle = pcap_create(iface, pcapError);
  if(pcapDevHandle == NULL) {
    fprintf(stderr, "Error while creating packet capture handle:\n");
    fprintf(stderr, "%s\n", pcapError);

    return -6;
  }

  status = pcap_set_rfmon(pcapDevHandle, 1);
  if(status) {
    // An error occurred setting the device in promiscuous mode.
    fprintf(stderr, "Could not start promiscuous mode. Make sure you have root (admin) privileges.\n");

    return -1;
  }

  status = pcap_lookupnet(iface, &netAddr, &netMask, pcapError);
  if(status) {
    // An error occurred looking up the IPv4 network number and netmask.
    fprintf(stderr, "Could not determine the IPv4 number or netmask:\n\t%s\n", pcapError);

    return -2;
  }

  status = pcap_activate(pcapDevHandle);
  if(status) {
    // An error occurred activating the device.
    fprintf(stderr, "An error occurred trying to activate the device:\n\t%s\n", pcapError);

    return -7;
  }

  if(pcap_datalink(pcapDevHandle) != DLT_IEEE802_11_RADIO) {
    // This program requires Ethernet headers, but this device does not support these headers.
    fprintf(stderr, "Header type not supported (Required: %d; Actual: %d).  Quitting.\n", DLT_IEEE802_11_RADIO, pcap_datalink(pcapDevHandle));

    return -5;
  }

  status = pcap_compile(pcapDevHandle, &pcapFilter, "tcp or udp", 1, netMask);
  if(status) {
    // An error occurred compiling the filter.
    fprintf(stderr, "An error occurred compiling the pcap filter.\n");

    return -3;
  }

  status = pcap_setfilter(pcapDevHandle, &pcapFilter);
  if(status) {
    fprintf(stderr, "An error occurred setting the pcap filter.\n");

    return -4;
  }

  initMacQueue();

  fprintf(stderr, "Device MAC:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(deviceMacAddress));
  fprintf(stderr, "Router MAC:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(routerMacAddress));

  return 0;
}

/**
 * Releases all resources used by the scanner.
 */
void destroyScanner() {
  if(scannerThread) {
    pthread_cancel(scannerThread);
  }

  pcap_close(pcapDevHandle);

  free(deviceMacAddress);
  free(routerMacAddress);

  destroyMacQueue();
}

/*=============================================================================
 *=============================================================================
 * Private Methods
 *=============================================================================
 *=============================================================================*/

/**
 * Receives and parses a packet from pcap.  The MAC addresses of the packet's sending and
 * receiving devices are read and added to the MAC queue as appropriate.
 *
 * @param args (u_char *) - unused
 * @param header (const struct pcap_pkthdr) - the header for the packet that was received
 * @param packet (const u_char *) - the packet that was received
 */
void receivePacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
  RadioTapHeader *radioTapHeader;
  WiFiHeader *wifiHeader;
  IpHeader *ipHeader;

  radioTapHeader = (RadioTapHeader *) radioTapHeader;
  wifiHeader = (WiFiHeader *) (WIFI_START(radioTapHeader));

  fprintf(stderr, "Received packet...\n");

  if(macEquals(deviceMacAddress, wifiHeader->addr1) && macEquals(deviceMacAddress, wifiHeader->addr2) && macEquals(deviceMacAddress, wifiHeader->addr3) && macEquals(deviceMacAddress, wifiHeader->addr4)) {
    fprintf(stderr, "Received Packet:\n");

    fprintf(stderr, "\tControl Code:\t%d\n", WIFI_FLAG_TYPE(wifiHeader));
    fprintf(stderr, "\tAddress 1:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(wifiHeader->addr1));
    fprintf(stderr, "\tAddress 1:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(wifiHeader->addr2));
    fprintf(stderr, "\tAddress 1:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(wifiHeader->addr3));
    fprintf(stderr, "\tAddress 1:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(wifiHeader->addr4));
  }
}

/**
 * The start routine for the thread that will be responsible for listening to the
 * promiscuous port opened earlier indefinitely.  This thread will be cancelable at any
 * time.
 */
void *scanNetwork(void *ptr) {
  int oldValue;

  fprintf(stderr, "Starting scan...\n");

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldValue);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldValue);

  pcap_loop(pcapDevHandle, -1, receivePacket, NULL);
}

/*=============================================================================
 *=============================================================================
 * Public Methods
 *=============================================================================
 *=============================================================================*/

/**
 * Starts scanning for possible MAC addresses to spoof.  This method starts a new thread
 * that will continue populating a list of MAC addresses until the scanner is destroyed (by
 * calling destroyScanner()).  This method is guaranteed not to return until at least
 * NETFREE_MIN_ADDRESSES, which is defined in scanner.h, addresses are found.
 */
void scan() {
  pthread_create(&scannerThread, NULL, scanNetwork, NULL);

  // Give the system to populate.
  while(macQueueLength() < NETFREE_MIN_ADDRESSES) {
    sleep(1);
  }
}