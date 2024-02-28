#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

#include "pcap.h"



// Define the global pcap header structure
struct pcap_global_header {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
};

// Define the packet header structure
struct pcap_packet_header {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t caplen;
    uint32_t len;
};

uint16_t PCAP_WritePacket(FILE* fp, const uint8_t* data, uint16_t dataLength){
    if(fp==NULL) return 0;
    struct pcap_packet_header packet_header;

    // Get current timestamp
    struct timeval current_time;
    gettimeofday(&current_time, NULL);

    // Fill packet header
    packet_header.ts_sec = current_time.tv_sec;
    packet_header.ts_usec = current_time.tv_usec;
    packet_header.caplen = dataLength;
    packet_header.len = dataLength;

    // Write packet header and packet data to the file
    fwrite(&packet_header, sizeof(packet_header), 1, fp);
    fwrite(data, dataLength, 1, fp);

    return dataLength;
}

void PCAP_ClosePCAP(FILE* fp){
    fflush(fp);
    fsync(fileno(fp));
    // Close the pcap file
    fclose(fp);
}
FILE* PCAP_OpenPCAP(const char* filename) {
    struct pcap_global_header global_header;
    FILE *pcap_file;

    // Open the pcap file for writing
    pcap_file = fopen(filename, "wb");
    if (!pcap_file) {
        perror("Error opening pcap file");
        return NULL;
    }

    // Write global header to the file
    global_header.magic_number = 0xa1b2c3d4;
    global_header.version_major = 2;
    global_header.version_minor = 4;
    global_header.thiszone = 0;
    global_header.sigfigs = 0;
    global_header.snaplen = 65535; // Maximum packet length
    global_header.network = 1; // Ethernet

    fwrite(&global_header, 1, PCAP_GLOBAL_HEADER_SIZE, pcap_file);

    return pcap_file;
}