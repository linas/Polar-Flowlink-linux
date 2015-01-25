#ifndef __PARSE_DATA_H
#define __PARSE_DATA_H
#define true 1 
#define false 0 

#include <stdbool.h>


int hex2int(int i);
int decode(int i);
int parseCommand1(unsigned char *buf, int bufsize);
int parseUserData(unsigned char *buf, int bufsize);
int parseTrainingData(unsigned char *buf, int bufsize,bool end);
int parseLap14(unsigned char *buf, int bufsize,int nb);
int parseSportZones(unsigned char *buf, int bufsize);
#endif
