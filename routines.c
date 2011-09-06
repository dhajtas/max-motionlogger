
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include "hw.h"
#include "rtc.h"
#include "sd_routines.h"
#include "routines.h"
#include "FAT32.h"

extern uint8_t Resets;
extern uint8_t Log_num;

//PGM_P DoubleDot = ":";
//PGM_P NewLine = {10,0}; 

//uint8_t readCfgFile(DATE *adate,TIME *atime, uint8_t *filename_dat, uint8_t atomic)
uint8_t readCfgFile(char *filename_dat)				//, uint8_t atomic)
{
	DIR *dir;
	uint32_t cluster, fileSize, firstSector;
	uint16_t k;
	uint8_t i,j,ld,lt;
	uint8_t filename[]= "CONFIG  CFG";
	uint8_t filename2[]="_CONFIG CFG";
	
	
	dir = findFiles (GET_FILE, filename, filename);		//, atomic); 	//get the file location
	if(dir == 0) 
		return(0);

	cluster = (((uint32_t) dir->firstClusterHI) << 16) | dir->firstClusterLO;
	fileSize = dir->fileSize;
	
	ld = 0;
	lt = 0;
	
	while(1)
	{
		ClusterNumber = cluster;
		firstSector = getFirstSector ();
		for(j=0; j<SectorPerCluster; j++)
		{
			SD_readSingleBlock(firstSector + j);			//, atomic);
			for(k=0; k<512; k++)
			{
				switch(buffer[k])
				{
					case	'S':
								k++;
								if(buffer[k]=='T')
								{
									k++;
									Time.h = getDec((uint8_t*)buffer+k);
									//k +=2;
									Time.m = getDec((uint8_t*)buffer+k+2);
									//k +=2;
									Time.s = getDec((uint8_t*)buffer+k+4);
								}
								else if(buffer[k]=='D')
								{	
									k++;
									Date.d = getDec(&buffer[k]);
									//k +=2;
									Date.m = getDec(&buffer[k+2]);
									//k +=2;
									Date.y = 20 + getDec(&buffer[k+4]);
								}
								else if(buffer[k]=='F')
								{
									k = k+2;
									for(i=0;i<8;i++)
									{
										if(buffer[k+i] < 0x21)		//if any control code and space
										{
											filename_dat[i] = 0x00;
											break;
										}
										filename_dat[i] = buffer[k+i];
									}
									filename_dat[i+1] = 0x00;

//									i = strlcpy(filename_dat,&(buffer[k]),9);
									k = k+i;
								}
								else if(buffer[k]=='I')
								{
									k +=2;
									for(i=0;i<13;i++)
									{
										if(buffer[k+i] < 0x20)		//if any control code
											break;
										inset[i] = buffer[k+i];
									}
								}
								break;
					case	'A':
								k++;
								if(buffer[k]=='D')
								{	
									k++;
									ADate[ld].d = getDec((uint8_t*)buffer+k);
									//k +=2;
									ADate[ld].m = getDec((uint8_t*)buffer+k+2);
									//k +=2;
									ADate[ld].y = 20 + getDec((uint8_t*)buffer+k+4);
									ld++;
								}
								else if(buffer[k]=='T')
								{	
									k++;
									ATime[lt].h = getDec((uint8_t*)buffer+k);
									//k +=2;
									ATime[lt].m = getDec((uint8_t*)buffer+k+2);
									//k +=2;
									ATime[lt].s = getDec((uint8_t*)buffer+k+4);
									lt++;
								}
								break;
					
				}
				
				if (k >= fileSize ) 
				{
					findFiles(RENAME, filename, filename2);			//, atomic);
					return(lt);
				}
			}
		}
		ClusterNumber = cluster;
		cluster = getSetNextCluster (GET);						//, 0, atomic);
		if(cluster == 0) 
		{
//#if BIGAVR == 1
//			printf_P(PSTR("ERR GETTING CLUSTERb\n")); 
//#endif		//BIGAVR
			return(0);
		}
	}
	return(0);
}

//-------------------------------------------------------------

uint8_t getDec(uint8_t *data)
{
	uint8_t out = 0;
	
	out = ((uint8_t)*data-0x30)*10+((uint8_t)*(data+1)-0x30);
	return(out);
}


char* CreateFilename(char *dest, char *src, uint8_t number)
{
//	char s_num[3];
	sprintf(dest,"aaa.d%02d",number);
//	dest = strcpy(dest,src);
//	dest = strcat(dest,".d");
//	dest = strcat(dest, itoa(number,s_num,10));
	return(dest);
}

void SaveLog(char *filename, char *dest, uint16_t batt)
{
//	char s_num[5];

	sprintf_P(dest, PSTR("log_%03d.txt"), Log_num);
//	dest = strcpy_P(dest, PSTR("log_"));
//	dest = strcat(dest, itoa(Log_num,s_num,10));
//	dest = strcat_P(dest, PSTR(".txt"));
	openFile(dest);

	sprintf_P(dest, PSTR("File: %s\n"),filename);
	writeFile_tst(dest,0);
	sprintf_P(dest, PSTR("D: %02d.%02d.%02d\n"), Date.d, Date.m, Date.y - 20);
	writeFile_tst(dest,0);				

	sprintf_P(dest, PSTR("T: %02d:%02d:%02d\n"), Time.h, Time.m, Time.s);
//	dest = strcpy(dest, itoa(Time.h,s_num,10));
//	dest = strcat_P(dest, DoubleDot);
//	dest = strcat(dest, itoa(Time.m,s_num,10));
//	dest = strcat_P(dest, DoubleDot);
//	dest = strcat(dest, itoa(Time.s,s_num,10));
//	dest = strcat_P(dest, NewLine);
	writeFile_tst(dest,0);				

	sprintf_P(dest, PSTR("BATT: %04d\n"), batt);
//	dest = strcpy_P(dest, PSTR("BATT: "));
//	dest = strcat(dest, itoa(batt, s_num, 10));
//	dest = strcat_P(dest, NewLine);
	writeFile_tst(dest,0);	

	sprintf_P(dest, PSTR("RST: %02d\n"), Resets);
//	dest = strcpy_P(dest, PSTR("RST: "));
//	dest = strcat(dest, itoa(Resets, s_num, 10));
//	dest = strcat_P(dest, NewLine);
	writeFile_tst(dest,0);	

	if(Status & STAT_BATT_LOW)
	{
		sprintf_P(dest,PSTR("Battery low!\n"));
//		dest = strcpy_P(dest, PSTR("Battery low!"));
//		dest = strcat_P(dest, NewLine);
		writeFile_tst(dest,0);			//,0);
	}
					
	writeFile_tst(dest,2);				//,0);	//last write has to have end_of_file atribute!
	closeFile();
}