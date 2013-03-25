#include "parse_data.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int hex2int(int i){
	char c[255];
	sprintf(c,"%x",i);
	return atoi(c);
}

int parseUserData(unsigned char *buf, int bufsize){

	if(!(buf[2]==0x13 && buf[3]==0x0e)){
		//printf("Buffer does not consist of user data! %x  %x \n",buf[2],buf[3]);
		return 1;
	}

	//printf("weight=%.1f kg\n", (double)((buf[6]<<8)+buf[5])*0.045359 );
	//printf("height=%d cm\n", buf[7]);
	//printf("birthdate=%d.%d.%d\n",buf[8],buf[9],1900+buf[10]);
	//printf("sex=%s\n", buf[14]?"male":"female");
	//printf("Activity (Top=0,High=1,Mid=2)=%d\n",buf[15]);
	//printf("VO2max=%d\n",buf[12]);
    //printf("HRmin=%d\n",buf[13]);
	//printf("HRmax=%d\n",buf[11]);
	//printf("Information last edited=%d.%d.%d %x:%x:%x\n",buf[19],buf[20],buf[21],buf[18],buf[17],buf[16]);
	printf("'usersettings':{'maxhr':%d,'resthr':%d,'weight':%d,'height':%d,'vo2max':%d,'sex':'%s','activity':'middle','birthdate':'%d-%02d-%02d'}",buf[11],buf[13],(int)(((buf[6]<<8)+buf[5])*0.045359),buf[7],buf[12],buf[14]?"male":"female",1900+buf[10],buf[9],buf[8]);
//{'usersettings':{'maxhr':196,'resthr':58,'weight':70,'height':179,'vo2max':44,'sex':'male','activity':'middle','birthdate':'1977-08-13'}}
	return 0;
}

int parseCommand1(unsigned char *buf, int bufsize){
	if(buf[2]!=0x3c && buf[3]!=0x00){
		//printf("Buffer does not consist of training data! %x  %x \n",buf[2],buf[3]);
		return -1;
	}

	//printf("Number of saved trainings=%d\n",buf[11]);
	return buf[11];
}



int parseTrainingData(unsigned char *buf, int bufsize,bool end){

	if(!(buf[2]==0x3D && buf[3]==0x06)){
		//printf("Buffer does not consist of training data! %x  %x \n",buf[2],buf[3]);
		return 1;
	}
	
	//printf("training_id=%d\n",buf[5]);
	//printf("training duration=%d:%d:%d\n",buf[25],decode(buf[24]),decode(buf[23]));
	//printf("start time=%d-%d-%d %x:%x:%x.000\n",2013,buf[18]/32, buf[18]%32, buf[22],buf[21],buf[20]);
	//printf("Calories burned=%d\n",(buf[30]<<8)+buf[29]);
	//printf("Average HR=%d\n", buf[26]);
	//printf("Max HR=%d\n",buf[27]);
    printf("{'id':%d,'time':'%d-%02d-%02d %02x:%02x:%02x.000','duration':%d,'note':'','exercicecal':%d,'exercicehrmin':'','exercicehravg':%d,'exercicehrmax':%d%s",buf[5],2013,buf[18]/32, buf[18]%32, buf[22],buf[21],buf[20],buf[25]*3600+decode(buf[24])*60+decode(buf[23]),((buf[30]<<8)+buf[29]),buf[26],buf[27],(end)?",":"");
	return 0;
}
int parseSportZones(unsigned char *buf, int bufsize){
    //printf("Time in Sport Zone 1=%x:%x:%x ",buf[34],buf[33],buf[32]);
	//printf("in Sport Zone 2=%x:%x:%x ",buf[37],buf[36],buf[35]);
	//printf("in Sport Zone 3=%x:%x:%x'\n",buf[40],buf[39],buf[38]);
	//printf("in Sport Zone 4=%x:%x:%x ",buf[43],buf[42],buf[41]);
	//printf("in Sport Zone 5=%x:%x:%x'\n",buf[46],buf[45],buf[44]);
	printf(",'zones':[");
    printf("{'id':0,'zonetime':%d},",buf[34]*3600+buf[33]*60+buf[32]);
    printf("{'id':1,'zonetime':%d},",buf[37]*3600+buf[36]*60+buf[35]);
    printf("{'id':2,'zonetime':%d},",buf[40]*3600+buf[39]*60+buf[38]);
    printf("{'id':3,'zonetime':%d},",buf[43]*3600+buf[42]*60+buf[41]);
    printf("{'id':4,'zonetime':%d}",buf[46]*3600+buf[45]*60+buf[44]);
	printf("]");
	return buf[52];
}

int parseLap14(unsigned char *buf, int bufsize,int nb){
    //printf("Sport Zone1 HR limits (%d,%d), Zone2 HR limits (%d,%d), Zone3 HR limits (%d,%d) , Zone4 HR limits (%d,%d), Zone5 HR limits (%d,max)\n",buf[12],buf[13],buf[13],buf[14],buf[14],buf[15],buf[15],buf[16],buf[16]);
    if(nb>1){
        int i=0;
        printf(",'laps':[");
	    for(i=0;i<nb;i++){
			//printf("Lap %d \n",nb-i);
	        //printf("lap duration = %d:%d:%d\n",buf[12+6*i],decode(buf[11+6*i]),decode(buf[10+6*i]));
	       // printf("HR = %d, HR moy = %d ,HR max = %d \n",buf[13+6*i],buf[14+6*i],buf[15+6*i]);
	        printf("{'id':%d,'laptime':%d,'hr':%d,'hravg':%d,'hrmax':%d}%s",nb-i-1,(buf[12+6*i]*3600+decode(buf[11+6*i])*60+decode(buf[10+6*i]))*10,buf[13+6*i],buf[14+6*i],buf[15+6*i],(i>=nb-1)?"]}":",");
		}
    }else{
    	printf(",'laps':[]}");
    }
	return 0;
}

int parseLap59(unsigned char *buf, int bufsize,int nb){
return 1;
}


int decode(int coded){
    int j = coded;
    if(j>=0x80){ j= j-0x80;}
    if(j>=0x40){ j= j-0x40;}
    return j;
}

int parseVO2maxMeasurements(unsigned char *buf, int bufsize){
	int i;
	if(buf[2]!=0x3D && buf[3]!=0x01){
		//printf("Buffer does not consist of VO2max data!\n");
		return 1;
	}
	//printf("Number of measurements: %d\n", buf[5]);

	for(i=0;i<buf[5];i++){
	//printf("%d.%d.%d :: VO2max=%d\n", buf[6+4*i], buf[7+4*i], buf[8+4*i], buf[9+4*i]);
	}
	return 1;

}

int parseActiveProgram(unsigned char *buf, int bufsize){

	return 1;
}
