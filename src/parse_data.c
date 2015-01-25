#include "parse_data.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int parseUserData(unsigned char *buf, int bufsize)
{
	if (!(buf[2]==0x13 && buf[3]==0x0e)) {
		return 1;
	}
	printf("'usersettings':{'maxhr':%d,'resthr':%d,'weight':%d,"
	       "'height':%d,'vo2max':%d,'sex':'%s','activity':'middle',"
	       "'birthdate':'%d-%02d-%02d'}\n",
		buf[11], buf[13], (int)(((buf[6]<<8)+buf[5])*0.045359),
		buf[7], buf[12], buf[14]?"male":"female",1900+buf[10], buf[9], buf[8]);
	return 0;
}

int parseCommand1(unsigned char *buf, int bufsize)
{
	if (buf[2] != 0x3c && buf[3] != 0x00) {
		return -1;
	}
	return buf[11];
}

int parseTrainingData(unsigned char *buf, int bufsize,bool end)
{
	if (!(buf[2]==0x3D && buf[3]==0x06)) {
		return 1;
	}
	int j = buf[19]*256+buf[18];
	int k = j/32;
	printf("{'id':%d,'time':'%d-%02d-%02d %02x:%02x:%02x.000',"
	       "'duration':%d,'note':'','exercicecal':%d,'exercicehrmin':'',"
	       "'exercicehravg':%d,'exercicehrmax':%d%s\n",
		buf[5], 2000+k/16,k%16, j%32, buf[22], buf[21], buf[20],
		buf[25]*3600+decode(buf[24])*60+decode(buf[23]),
		((buf[30]<<8)+buf[29]),buf[26],buf[27],(end)?",":"");
	return 0;
}

int parseSportZones(unsigned char *buf, int bufsize)
{
	printf(",'zones':[\n");
	printf("\t{'id':0,'zonetime':'P%xH%xM%xS','min':%d,'max':%d},\n",
		buf[34],buf[33],buf[32],buf[22],buf[23]);
	printf("\t{'id':1,'zonetime':'P%xH%xM%xS','min':%d,'max':%d},\n",
		buf[37],buf[36],buf[35],buf[24],buf[25]);
	printf("\t{'id':2,'zonetime':'P%xH%xM%xS','min':%d,'max':%d},\n",
		buf[40],buf[39],buf[38],buf[26],buf[27]);
	printf("\t{'id':3,'zonetime':'P%xH%xM%xS','min':%d,'max':%d},\n",
		buf[43],buf[42],buf[41],buf[28],buf[29]);
	printf("\t{'id':4,'zonetime':'P%xH%xM%xS','min':%d,'max':%d}\n",
		buf[46],+buf[45],buf[44],buf[30],buf[31]);
	printf("]\n");
	return buf[52];
}

int parseLap14(unsigned char *buf, int bufsize,int nb)
{
	if (nb>1) {
		int i=0;
		printf(",'laps':[");
		for(i=0;i<nb;i++){
			printf("{'id':%d,'laptime':%d,'hr':%d,'hravg':%d,'hrmax':%d}%s",nb-i-1,(buf[12+6*i]*3600+decode(buf[11+6*i])*60+decode(buf[10+6*i]))*10,buf[13+6*i],buf[14+6*i],buf[15+6*i],(i>=nb-1)?"]}":",");
		}
	}else{
		printf(",'laps':[]}");
	}
	return 0;
}


int decode(int coded)
{
	int j = coded;
	if(j>=0x80){ j= j-0x80;}
	if(j>=0x40){ j= j-0x40;}
	return j;
}
