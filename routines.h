#ifndef _ROUTINES_H_
#define _ROUTINES_H_

#include "rtc.h"

//uint8_t readCfgFile(DATE *adate,TIME *atime, uint8_t *filename, uint8_t atomic);
uint8_t readCfgFile(char *filename);			//, uint8_t atomic);

uint8_t getDec(uint8_t *data);

char* CreateFilename(char *dest, char *src, uint8_t number);

void SaveLog(char *filename, char *dest, uint16_t battery);

#endif //_ROUTINES_H_
