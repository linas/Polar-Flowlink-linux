/**
 * polar-ft7 -- simple polar FT7 HRM FlowLink transfer for basic html analysis
 * Copyright (C) 2012 Samo Penic <samo.penic@opensarm.si>,
 * Copyright (C) 2012 Cedric Lecuret <clecuret@gmail.com>
 * Copyright (C) 2015 Linas Vepstas <linasvepstas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
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
	int i, n;
	bool present;

	// Mystery commands ... hardware ??
	// unsigned char hrdwr[]      = {0x04, 0x42, 0x20};

	/* Commands for the polar HRM FlowLink */
	unsigned char cmd1[]     = {0x01,0x00,0x02,0x00,0x00};
	unsigned char cmd2[]     = {0x01,0x00,0x02,0x10,0x00};
	unsigned char cmdusr[]   = {0x01,0x00,0x02,0x0E,0x00};
	unsigned char cmdtrain[] = {0x01,0x00,0x04,0x06,0x00,0x00};
	unsigned char cmd3[]     = {0x01,0x00,0x02,0x01,0x00};
	unsigned char cmd4[]     = {0x01,0x00,0x02,0x49,0x00};

	/* Magic numbers for USB ... */
	handle = openHID(0x0da4, 0x0003);
	if (handle == NULL) {
		fprintf(stderr, "Error: Could not connect to Polar FlowLink.\n"
		                "Is Flowlink connected and do you have privileges?\n"
		                "Maybe you should suid the program?\n");
		exit(1);
	}

	present = pollPresence(handle);
	if (!present)
	{
		closeHID(handle);
		exit(1);
	}

	// executeCommand1(handle, buf, 256, hrdwr, sizeof(hrdwr), true);
	fprintf(stderr, "WARNING: This code does not yet work for the FT7!!!!\n");

	executeCommand1(handle, buf, 256, cmd1, sizeof(cmd1), false);
	printf("{");
	n = parseCommand1(buf, 256);
	fprintf(stderr, "Great, found %d exercise records!\n", n);

	executeCommand1(handle, buf, 256, cmd2, sizeof(cmd2), false);
	readData(handle, buf, 256, false, 0);

	executeCommand1(handle, buf, 256, cmdusr, sizeof(cmdusr), false);
	parseUserData(buf, 256);
	executeCommand1(handle, buf, 256, cmd3, sizeof(cmd3), false);
	printf(",\n'exercises':[\n");
	fflush(stdout);
	for (i=0; i<n; i++) {
		cmdtrain[5] = i;
		fprintf(stderr,
			"Great, let's get personal data for record %d of %d\n", i, n);
		executeCommand1(handle, buf, 256, cmdtrain, sizeof(cmdtrain), false);
		if (i==0) {
			readData(handle, buf, 256, false,0);
		}
		parseTrainingData(buf, 256, i>=n);
		int l = readData(handle,buf, 256, false,0);
		int nb = parseSportZones(buf, 256);
		if (0 == nb)
			fprintf(stderr, "No lap data found!\n");
		else
			fprintf(stderr, "Great, I found lap data for %d laps!\n", nb);
		l = readData(handle, buf, 256, false, 0);
		memcpy (&data, &buf, l);

		if (nb > 4) {
			readData(handle, buf, l, false,0);
			memmove(buf+l-3, buf, l);
			memmove(buf, data, l);
			parseLap14(buf, 256, nb);
			executeCommand1(handle,buf,256,cmd4,6, false);
		} else {
			parseLap14(buf,256,nb);
		}
		printf("%s", (i>=n-1) ? "]\n" : ",\n");
		fflush(stdout);
	}

	executeCommand1(handle,buf,256,cmd4,5, false);
	while (buf[1]) {
		readData(handle,buf,256, false,0);
	}
	executeCommand1(handle,buf,256,cmd4,5, false);
	while (buf[1]) {
		readData(handle,buf,256, false,0);
	}
	printf("}\n");

	closeHID(handle);
	fprintf(stderr, "Done!\n");

	return 0;
}
