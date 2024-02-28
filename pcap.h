#ifndef __PCAP_H__
#define __PCAP_H__

#include <stdint.h>
#include <stdio.h>

#define PCAP_GLOBAL_HEADER_SIZE 24
#define PCAP_PACKET_HEADER_SIZE 16

uint16_t    PCAP_WritePacket(FILE* fp, const uint8_t* data, uint16_t dataLength);
void        PCAP_ClosePCAP(FILE* fp);
FILE*       PCAP_OpenPCAP(const char* filename);

#endif