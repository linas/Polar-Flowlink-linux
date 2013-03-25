#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include "hidapi.h"
#include <unistd.h>
#include "protocol.h"
#include "parse_data.h"

int main(int argc, char* argv[])
{
	unsigned char buf[256];
    unsigned char data[256];
	hid_device *handle;
	int i, n,present;
	//int j;
/**
01 00 02 00 00
01 00 02 10 00
01 00 02 0E 00
01 00 04 06 00 00
01 00 04 06 00 01
...
01 00 04 06 00 0E
01 00 04 06 00 0F
01 00 02 01 00
01 00 02 49 00
01 00 02 49 00
01 00 17 17 00 07 0B 0C ...
*/
	unsigned char cmd1[256]={0x01,0x00,0x02,0x00,0x00};
	unsigned char cmd2[256]={0x01,0x00,0x02,0x10,0x00};
	unsigned char cmdusr[256]={0x01,0x00,0x02,0x0E,0x00};
	unsigned char cmdtrain[256]={0x01,0x00,0x04,0x06,0x00,0x00};
	unsigned char cmd3[256]={0x01,0x00,0x02,0x01,0x00};
	unsigned char cmd4[256]={0x01,0x00,0x02,0x49,0x00};
	/**unsigned char cmd5[256]={0x01,0x00,0x02,0x14,0x10};
	unsigned char cmd6[256]={0x01,0x00,0x02,0x14,0x20};
	unsigned char cmd7[256]={0x01,0x00,0x02,0x14,0x30};
    
printf("polar -- simple polar RS300X HRM FlowLink transfer for basic html analysis\n\n\
    Copyright (C) 2012  Samo Penic <samo.penic@opensarm.si>,Cedric Lecuret <clecuret@gmail.com>\n\n\
    This program is free software: you can redistribute it and/or modify\n\
    it under the terms of the GNU General Public License as published by\n\
    the Free Software Foundation, either version 3 of the License, or\n\
    (at your option) any later version.\n\n\
    This program is distributed in the hope that it will be useful,\n\
    but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
    GNU General Public License for more details.\n\n\
    You should have received a copy of the GNU General Public License\n\
    along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n\n\
");
*/
	handle=openHID(0x0da4,0x0003);
	if(handle==NULL){
		printf("Error, could not connect to Polar FlowLink. Is Flowlink connected and do you have privileges? Maybe you should suid the program?\n");
		exit(1);
	}

	present=poolPresence(handle);
	if(present){
printf("{");
		executeCommand1(handle,buf,256,cmd1,5, FALSE);
		n=parseCommand1(buf,256);
	
        executeCommand1(handle,buf,256,cmd2,5, FALSE);
		readData(handle,buf,256, FALSE,0);

	//	printf("Great, let's get personal data!");
		executeCommand1(handle,buf,256,cmdusr,5, FALSE);
		parseUserData(buf,256);
        executeCommand1(handle,buf,256,cmd3,5, FALSE);
		parseVO2maxMeasurements(buf,256);
		printf(",'exercices':[");
  //      printf("Let's get training data!");	
		for(i=0;i<n;i++){
			cmdtrain[5]=i;	
            //if(i==15){cmdtrain[4]=0x20;}
//			printf("Great, let's get personal data!  %d\n",i);
			executeCommand1(handle,buf,256,cmdtrain,6, FALSE);
			
			if(i==0){            
				readData(handle,buf,256, FALSE,0);
			}
			parseTrainingData(buf,256,i>=n);
//printf("Now , read  sport zones data!!!! \n");
            int l =readData(handle,buf,256, FALSE,0);           
            int nb=parseSportZones(buf,256);
			//printf("Great, I found %d lap data! \n",nb);
            
            l = readData(handle,buf,256, FALSE,0);
			memcpy (&data, &buf, l);
            
            if(nb>4){
				readData(handle,buf,l, FALSE,0);
				memmove(buf+l-3,buf,l);
				memmove(buf,data,l);
                parseLap14(buf,256,nb);
//if(i==15){readData(handle,buf,256, TRUE,0);}
				executeCommand1(handle,buf,256,cmd4,6, FALSE);
                
			}else{
parseLap14(buf,256,nb);
}			
		printf("%s",(i>=n-1)?"]":",");
	
		}

		executeCommand1(handle,buf,256,cmd4,5, FALSE);
		while(buf[1]){
		readData(handle,buf,256, FALSE,0);
		}
		executeCommand1(handle,buf,256,cmd4,5, FALSE);
		while(buf[1]){
		readData(handle,buf,256, FALSE,0);
		}
		/**executeCommand1(handle,buf,256,cmd5,5, TRUE);
		while(buf[1]){
		readData(handle,buf,256, FALSE);
		}
		executeCommand1(handle,buf,256,cmd6,5, TRUE);
		while(buf[1]){
		readData(handle,buf,256, FALSE);
		}
		executeCommand1(handle,buf,256,cmd7,5,TRUE);
		while(buf[1]){
		readData(handle,buf,256,FALSE);
		}
        */
	}	
	printf("}");
	closeHID(handle);

	return 0;
}
