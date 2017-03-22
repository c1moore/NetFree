#ifndef _NETFREE_IP_TCP_DATA
  #define _NETFREE_IP_TCP_DATA
  
  #include <netinet/in.h>
  #include "mac.h"

  #define IP_START(packetPtr)         packetPtr + sizeof(EthernetHeader)
  #define IP_VERSION(ipHeader)        ipHeader->versionAndHeaderLength >> 4
  #define IP_HEADER_LENGTH(ipHeader)  ipHeader->versionAndHeaderLength & 0x0f
  #define IP_FRAG_OFFSET(ipHeader)    ipHeader->flagsAndFragOffset & 0x01fff
  #define IP_RESERVED_FLAG(ipHeader)  ipHeader->flagsAndFragOffset & 0x08000
  #define IP_DONT_FRAG(ipHeader)      ipHeader->flagsAndFragOffset & 0x04000
  #define IP_MORE_FRAG(ipHeader)      ipHeader->flagsAndFragOffset & 0x02000

  #define TCP_START(packetPtr, ipHeader)  packetPtr + sizeof(EthernetHeader) + (IP_HEADER_LENGTH(ipHeader) * 4)
  #define TCP_DATA_OFFSET(tcpHeader)  tcpHeader->dataOffsetAndReserved >> 4
  #define TCP_FLAG_CWR(tcpHeader)     tcpHeader->flags & 0x080
  #define TCP_FLAG_ECE(tcpHeader)     tcpHeader->flags & 0x040
  #define TCP_FLAG_URG(tcpHeader)     tcpHeader->flags & 0x020
  #define TCP_FLAG_ACK(tcpHeader)     tcpHeader->flags & 0x010
  #define TCP_FLAG_PSH(tcpHeader)     tcpHeader->flags & 0x008
  #define TCP_FLAG_RST(tcpHeader)     tcpHeader->flags & 0x004
  #define TCP_FLAG_SYN(tcpHeader)     tcpHeader->flags & 0x002
  #define TCP_FLAG_FIN(tcpHeader)     tcpHeader->flags & 0x001

  typedef struct EthernetHeaderStruct EthernetHeader;
  struct EthernetHeaderStruct {
    u_char  destination[NETFREE_MAC_SIZE];
    u_char  source[NETFREE_MAC_SIZE];
    u_short type;
  };

  typedef struct IpHeaderStruct IpHeader;
  struct IpHeaderStruct {
    u_char          versionAndHeaderLength;
    u_char          typeOfService;
    u_short         dataLength;
    u_short         id;
    u_short         flagsAndFragOffset;
    u_char          ttl;
    u_char          protocol;
    u_char          checksum;
    struct in_addr  source;
    struct in_addr  destination;
  };

  typedef struct TcpHeaderStruct TcpHeader;
  struct TcpHeaderStruct {
    u_short sourcePort;
    u_short destinationPort;
    u_int   sequenceNumber;
    u_int   ackNumber;
    u_char  dataOffsetAndReserved;
    u_char  flags;
    u_char  windowSize;
    u_char  checksum;
    u_char  urgentPoint;
  };
#endif