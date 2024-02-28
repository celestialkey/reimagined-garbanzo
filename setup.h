#ifndef __SETUP_H__
#define __SETUP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>

int CreateRawSocket();
unsigned int GetInterfaceIndex(char* interface);
int LimitCaptureToInterface(char* interface, int sockfd);


#endif