#include <pcap.h>
#include <stdio.h>

#include "scanner.h"
#include "iptcpdata.h"

pcap_t *pcapDevHandle;

int initScanner(char *iface) {
  int                 status;
  char                pcapError[PCAP_ERRBUF_SIZE];

  struct bpf_program  pcapFilter;

  bpf_u_int32         netAddr = 0,
                      netMask = 0;

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

void destroyScanner() {
  pcap_close(pcapDevHandle);
}

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

void scan() {
  pcap_loop(pcapDevHandle, 500, receivePacket, NULL);
}