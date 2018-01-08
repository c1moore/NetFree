#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "scanner.h"
#include "iptcpdata.h"
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

  if(pcap_datalink(pcapDevHandle) != DLT_EN10MB) {
    // This program requires Ethernet headers, but this device does not support these headers.
    fprintf(stderr, "Ethernet headers not supported (Required: %d; Actual: %d).  Quitting.\n", DLT_EN10MB, pcap_datalink(pcapDevHandle));

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

  deviceMacAddress = (char *) malloc(NETFREE_MAC_SIZE);
  routerMacAddress = (char *) malloc(NETFREE_MAC_SIZE);

  getOriginalMacAddress(deviceMacAddress);
  getRouterMacAddress(routerMacAddress);

  initMacQueue();

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
  EthernetHeader *ethernetHeader;
  IpHeader *ipHeader;
  TcpHeader *tcpHeader;

  ethernetHeader = (EthernetHeader *) packet;
  ipHeader = (IpHeader *) IP_START(packet);
  tcpHeader = (TcpHeader *) TCP_START(packet, ipHeader);

  if(!strncmp(deviceMacAddress, ethernetHeader->destination, NETFREE_MAC_SIZE) && !strncmp(routerMacAddress, ethernetHeader->destination, NETFREE_MAC_SIZE)) {
    enqueueMac(ethernetHeader->destination, 0);
  }

  if(!strncmp(deviceMacAddress, ethernetHeader->source, NETFREE_MAC_SIZE) && !strncmp(routerMacAddress, ethernetHeader->source, NETFREE_MAC_SIZE)) {
    enqueueMac(ethernetHeader->source, 0);
  }

  if(!strncmp(deviceMacAddress, ethernetHeader->destination, NETFREE_MAC_SIZE) && !strncmp(deviceMacAddress, ethernetHeader->source, NETFREE_MAC_SIZE)) {
    fprintf(stderr, "Received packet:\n");

    fprintf(stderr, "\tDestination MAC Address:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(ethernetHeader->destination));
    fprintf(stderr, "\tSource MAC Address:\t" NETFREE_MAC_REGEX "\n", NETFREE_ARR_TO_MAC(ethernetHeader->source));
  }
}

/**
 * The start routine for the thread that will be responsible for listening to the
 * promiscuous port opened earlier indefinitely.  This thread will be cancelable at any
 * time.
 */
void *scanNetwork(void *ptr) {
  int oldValue;

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