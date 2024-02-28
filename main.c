#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <getopt.h>

#include "pcap.h"
#include "setup.h"
#include "linkedlist.h"

#ifdef DEBUG
#include "debug.h"
#endif

#define BUFFER_SIZE     65536
#define LIMITINTERFACE  (1<<0)
#define LIMITFILESIZE   (1<<1)

// Function to set a flag
void setFlag(uint8_t *flags, uint8_t flag) {
    *flags |= flag;
}

// Function to clear a flag
void clearFlag(uint8_t *flags, uint8_t flag) {
    *flags &= ~flag;
}

// Function to check if a flag is set
int isFlagSet(const uint8_t *flags, uint8_t flag) {
    return (*flags & flag) != 0;
}

typedef struct {
    uint8_t flags;
    LLNode* interfaces;
    char filename[36];
    int interfaceCount;
    int fileSize;
    unsigned int packetsCollected;
} pOpts;

pOpts userOptions = {0};


int stop = 0;
void sigint_handler(int signum){
    printf("\nShutting down...\n");
    stop = 1;
}

#define MAX_INTERFACE_LENGTH 256
int parseArgs(int argc, char* argv[]){
    int opt;
    char interfaceList[MAX_INTERFACE_LENGTH] = {0};
    while((opt = getopt(argc, argv, "i:l:")) != -1){
        switch(opt){
            case 'i':{
                setFlag(&userOptions.flags, LIMITINTERFACE);
                strncpy(interfaceList, optarg, MAX_INTERFACE_LENGTH);
                interfaceList[MAX_INTERFACE_LENGTH-1] = '\0';
                break;
            }
            case 'l':{
                setFlag(&userOptions.flags, LIMITFILESIZE);
                userOptions.fileSize = atoi(optarg);
                break;
            }
            default:
                break;
        }
    }
    if(!isFlagSet(&userOptions.flags, LIMITINTERFACE)){
        return 1;
    }

    LLNode* node    = NULL;
    LLNode* parent  = userOptions.interfaces;
    char* token     = strtok(interfaceList, ",");
    while(token != NULL){
        userOptions.interfaceCount++;
        // Create the link
        node        = LLCreateNode();
        // Allocate memory
        int tokenLen= strlen(token)+1;//(strlen(token)+1)>8?strlen(token)+1:8;
        node->data  = malloc(tokenLen);
        // Copy data
        strncpy(node->data, token, tokenLen-1);
        ((char*)node->data)[tokenLen-1] = '\0';
        // Assign to the correct header
        if(userOptions.interfaces == NULL){
            userOptions.interfaces = node;
            parent = node;
        } else{
            parent->next = node;
            parent = node;
        }
        // Move the pointer
        token = strtok(NULL, ",");
    }
    return 1;

}
void printInterfaces(){
    LLNode* n = userOptions.interfaces;
    if(!n) return;
    do{
        printf("Interface: %s\n", (char*)n->data);
        n = n->next;
    }while(n != NULL);
}
int LinkInterfacesToSocket(int sockfd){
    LLNode *n = userOptions.interfaces;
    for(int i=0; i<userOptions.interfaceCount; i++){
        int err =0;
        if((err = LimitCaptureToInterface((char*)n->data, sockfd))==0){
            close(sockfd);
            return 0;
        }
        #ifdef DEBUG
        printf("[%d] %s -> %d\n", i, (char*)n->data, GetInterfaceIndex((char*)n->data));
        #endif
        n = n->next;
    }
    return 1;
}
int CapturePackets(FILE* fp, int sockfd){
    unsigned char buffer[BUFFER_SIZE];
    // Current size of the file as the header was already written.
    int currentFileSize = PCAP_GLOBAL_HEADER_SIZE;
    // Receive and process packets indefinitely
    while (!stop) {
        // Calculate the remaining space left in the file
        int diskSpaceRemaining  = userOptions.fileSize-currentFileSize;
        // Calculate the best recv size for packets on the network
        int recvSize            = diskSpaceRemaining > BUFFER_SIZE?BUFFER_SIZE:diskSpaceRemaining;
        // Yoink any pending data from the kernel
        int recvLength          = recvfrom(sockfd, buffer, recvSize, 0, NULL, NULL);
        // Break if the recvfrom call fails for whatever reason
        if (recvLength < 0) 
            break;
        // Break if there's not enough space remaining in the file for some reason
        if(currentFileSize + recvLength + PCAP_PACKET_HEADER_SIZE > userOptions.fileSize) 
            break;
        // Write the packet contents to disk
        int err                 = PCAP_WritePacket(fp, buffer, recvLength);
        // Update the current file size tracker
        currentFileSize         += PCAP_PACKET_HEADER_SIZE + recvLength;
        // Update packet count tracking data
        userOptions.packetsCollected++;
        #ifdef DEBUG
        // Dump debug info to screen
        //debug_dumpCapture(buffer, recvLength);
        #endif
        // Break if the write attempt failed for whatever reason
        if(!err) 
            break;
        #ifdef DEBUG
        // Splash to screen the current status of the file.
        printf("Wrote  %d [%.2f](%d | %d) bytes...\r", recvLength, ((float)currentFileSize/(float)userOptions.fileSize)*100, currentFileSize, userOptions.fileSize);
        #endif
        // Stop capture after reaching the size limit
        if(currentFileSize >= userOptions.fileSize) {
            stop=1;
            break;
        }
    }

    #ifdef DEBUG
    // Pump out the final statistic
    printf("\nWrote %d bytes, or %d packets\n", currentFileSize, userOptions.packetsCollected);
    #endif
    return 1;
}
int main(int argc, char* argv[]){
    FILE* fp = PCAP_OpenPCAP("test.pcap");
    if(!fp) return -1;

    // Handle Ctrl+C
    signal(SIGINT, sigint_handler);

    // Parse user provided arguments
    parseArgs(argc, argv);
    
    // Create the socket to listen on
    int sockfd = CreateRawSocket();

    // Bind the socket to all user provided interfaces
    int err = LinkInterfacesToSocket(sockfd);
    if(err==0) return -2;

    CapturePackets(fp, sockfd);
    
    // Clean up the argument list as it's no longer used
    LLFreeList(userOptions.interfaces);

    // Close the socket as it's no longer rused
    close(sockfd);

    // Close the pcap file
    PCAP_ClosePCAP(fp);
    return 0;
}