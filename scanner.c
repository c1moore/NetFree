#include <pcap.h>
#include <stdio.h>
#include <pthread.h>

#include "scanner.h"
#include "iptcpdata.h"

pcap_t     *pcapDevHandle;
pthread_t   scannerThread;

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

  status = pcap_set_promisc(pcapDevHandle, 1);
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

    return -3;
  }

  status = pcap_compile(pcapDevHandle, &pcapFilter, "tcp", 1, netMask);
  if(status) {
    // An error occurred compiling the filter.
    fprintf(stderr, "An error occurred compiling the pcap filter.\n");

    return -4;
  }

  status = pcap_setfilter(pcapDevHandle, &pcapFilter);
  if(status) {
    fprintf(stderr, "An error occurred setting the pcap filter.\n");

    return -5;
  }
}

/**
 * Releases all resources used by the scanner.
 */
void destroyScanner() {
  if(scannerThread) {
    pthread_cancel(scannerThread);
  }

  pcap_close(pcapDevHandle);
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

  fprintf(stderr, "Received packet:\n");

  ethernetHeader = (EthernetHeader *) packet;
  ipHeader = (IpHeader *) IP_START(packet);
  tcpHeader = (TcpHeader *) TCP_START(packet, ipHeader);

  if(ethernetHeader->destination[1] != 0x0a && ethernetHeader->source[1] != 0x0a) {
    fprintf(stderr, "Destination MAC Address:\t%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx\n", ethernetHeader->destination[0], ethernetHeader->destination[1], ethernetHeader->destination[2], ethernetHeader->destination[3], ethernetHeader->destination[4], ethernetHeader->destination[5]);
    fprintf(stderr, "Source MAC Address:\t%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx\n", ethernetHeader->source[0], ethernetHeader->source[1], ethernetHeader->source[2], ethernetHeader->source[3], ethernetHeader->source[4], ethernetHeader->source[5]);
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
  pthread_create(&scannerThread, NULL, destroyScanner, NULL);

  // Give the system to populate.
  while(macQueueLength() < NETFREE_MIN_ADDRESSES) {
    sleep(1);
  }
}