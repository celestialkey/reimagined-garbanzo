#include <ctype.h>
#include <stdio.h>
#include "debug.h"

#ifdef DEBUG
void debug_dumpCapture(uint8_t* pData, int dataLength){
    for(int i=0; i< dataLength; i+=16){
        int space = 40;
        for(int x=0; x<16; x++){
            if(x+i > dataLength) break;
            printf("%.2x", pData[i+x]);
            space -= 2;
        }
        for(int x=0; x<space; x++){
            printf(" ");
        }
        for(int x=0; x<16; x++){
            if(x+i > dataLength) break;
            if(isprint(pData[x+i]))
                printf("%c", pData[i+x]);
            else printf(".");
        }
        printf("\n");
    }
}
#endif