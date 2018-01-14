#ifndef _NETFREE_IP_TCP_DATA
  #define _NETFREE_IP_TCP_DATA

  #include <netinet/in.h>
  #include <stdint.h>
  #include "mac.h"

  #define WIFI_START(radioTapHeader)              (int) radioTapHeader->headerLength + radioTapHeader
  #define WIFI_FLAG_PROTOCOL_VERSION(wifiHeader)  wifiHeader->frameControl & 0xc000
  #define WIFI_FLAG_TYPE(wifiHeader)              wifiHeader->frameControl & 0x3000
  #define WIFI_FLAG_SUBTYPE(wifiHeader)           wifiHeader->frameControl & 0x0f00
  #define WIFI_FLAG_AP_TO                         wifiHeader->frameControl & 0x0080
  #define WIFI_FLAG_AP_FROM                       wifiHeader->frameControl & 0x0040
  #define WIFI_FLAG_MORE_FRAG                     wifiHeader->frameControl & 0x0020
  #define WIFI_FLAG_RETRY                         wifiHeader->frameControl & 0x0010
  #define WIFI_FLAG_POWER_MGT                     wifiHeader->frameControl & 0x0008
  #define WIFI_FLAG_MORE_DATA                     wifiHeader->frameControl & 0x0004
  #define WIFI_FLAG_WEP                           wifiHeader->frameControl & 0x0002
  #define WIFI_FLAG_RSVD                          wifiHeader->frameControl & 0x0001

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

  typedef struct RadioTapHeaderStruct RadioTapHeader;
  struct RadioTapHeaderStruct {
    u_int8_t  version;
    u_int8_t  pad;
    u_int16_t headerLength;
    u_int32_t dataFieldsPresent;
  };

  typedef struct EthernetHeaderStruct EthernetHeader;
  struct EthernetHeaderStruct {
    u_char  destination[NETFREE_MAC_SIZE];
    u_char  source[NETFREE_MAC_SIZE];
    u_short type;
  };

  typedef struct WiFiHeaderStruct WiFiHeader;
  struct WiFiHeaderStruct {
    u_int16_t frameControl;
    u_int16_t connectionId;
    u_char    addr1[NETFREE_MAC_SIZE];
    u_char    addr2[NETFREE_MAC_SIZE];
    u_char    addr3[NETFREE_MAC_SIZE];
    u_int16_t sequenceNumber;
    u_char    addr4[NETFREE_MAC_SIZE];
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