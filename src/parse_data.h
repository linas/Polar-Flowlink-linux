#ifndef __PARSE_DATA_H
#define __PARSE_DATA_H
#define true 1 
#define false 0 
typedef int bool;

typedef struct {
	int id;
	char md5[15];
	int duration;
	long starttime;
	int timeinzone1;
	int timeinzone2;
	int timeinzone3;
	int z1hr[2];
	int z2hr[2];
	int z3hr[2];
	int calories;
	int fatprocent;
	int avgHr;
	int maxHr;
	int HRMax;
	
} sTraining;

typedef struct {
	float weight;
	int height;
	long birthdate;
	int sex;
	int activity;
	int vo2max;
	int HRMax;
	long editdate;
} sUserData;

#include "database.h"

int hex2int(int i);
int decode(int i);
int parseCommand1(unsigned char *buf, int bufsize);
int parseUserData(unsigned char *buf, int bufsize);
int parseTrainingData(unsigned char *buf, int bufsize,bool end);
int parseLap14(unsigned char *buf, int bufsize,int nb);
int parseSportZones(unsigned char *buf, int bufsize);
int parseLap59(unsigned char *buf, int bufsize,int nb);
int parseVO2maxMeasurements(unsigned char *buf, int bufsize);
int parseActiveProgram(unsigned char *buf, int bufsize);
#endif
