#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>

#ifdef DEBUG
void debug_dumpCapture(uint8_t* pData, int dataLength);
#endif

#endif