#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/ethernet.h> // For Ethernet header structure
#include <net/if.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include "setup.h"


int CreateRawSocket(){
    // Create a raw socket to receive packets
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_socket < 0) {
        return 0;
    }
    return raw_socket;
}
unsigned int GetInterfaceIndex(char* interface){
    return if_nametoindex(interface);
}
int LimitCaptureToInterface(char* interface, int sockfd){
    if(interface==NULL) return -1;

    // Get interface index
    unsigned int idx = GetInterfaceIndex(interface);
    if(idx==0) return 0;

    // Bind socket to the specified interface
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = idx;
    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        close(sockfd);
        return 0;
    }
    return 1;
}