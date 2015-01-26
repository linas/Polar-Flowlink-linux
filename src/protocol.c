#include "protocol.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

hid_device *openHID(int vid, int pid)
{
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int res;
	unsigned char buf[256];
	// Set up the command buffer.
	memset(buf, 0x0, sizeof(buf));
	buf[0] = 0x01;
	buf[1] = 0x81;

	handle = hid_open(vid, pid, NULL);
	if (!handle) {
		fprintf(stderr, "Error: unable to open device\n");
 		return NULL;
	}

	// Read the Manufacturer String
	wstr[0] = 0x0000;
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Error: Unable to read manufacturer string\n");
	fprintf(stderr, "Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Error: Unable to read product string\n");
	fprintf(stderr, "Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Error: Unable to read serial number string\n");
	fprintf(stderr, "Serial Number String: (%d) %ls", wstr[0], wstr);
	fprintf(stderr, "\n");

	// Read Indexed String 1
	wstr[0] = 0x0000;
	res = hid_get_indexed_string(handle, 1, wstr, MAX_STR);
	if (res < 0)
		fprintf(stderr, "Error: Unable to read indexed string 1\n");
	fprintf(stderr, "Indexed String 1: %ls\n", wstr);

	// Set the hid_read() function to be non-blocking.
	hid_set_nonblocking(handle, 1);

	// Try to read from the device. There shoud be no
	// data here, but execution should not block.
	res = hid_read(handle, buf, 17);
	// Send a Feature Report to the device
	buf[0] = 0x2;
	buf[1] = 0xa0;
	buf[2] = 0x0a;
	buf[3] = 0x00;
	buf[4] = 0x00;
	res = hid_send_feature_report(handle, buf, 17);
	if (res < 0) {
		fprintf(stderr, "Error: Unable to send a feature report.\n");
	}
	return handle;
}

void closeHID(hid_device *handle)
{
	hid_close(handle);
	/* Free static HIDAPI objects. */
	hid_exit();
}

bool pollPresence(hid_device *handle)
{
	unsigned char buf[256];
	memset(buf,0,sizeof(buf));
	int res;

	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));
	if (res < 0) {
		fprintf(stderr, "Error: Unable to get a feature report.\n");
		fprintf(stderr, "%ls", hid_error(handle));
	}
	if (buf[2]==0x02 && buf[3]==0x0b){
		fprintf(stderr, "Error: HRM not detected. "
		                "Is it correctly aligned on the FlowLink?\n");
		return false;
	}
	fprintf(stderr, "HRM detected.\n");
	return true;
}

/** Send a command to the device */
int executeCommand1(hid_device *handle, unsigned char *buf, int bufsize,
                    unsigned char *command, int commandsize, bool showdata)
{
	int res,i;
	memset(buf, 0, bufsize*sizeof(unsigned char));
	memcpy(buf, command, commandsize*sizeof(unsigned char));
	if (showdata) {
		for (i = 0; i < commandsize; i++)
			printf("%02hhx ", command[i]);
		printf("\n");
	}
	res = hid_write(handle, buf, 17);
	if (res < 0) {
		fprintf(stderr, "Error: Unable to write()\n");
		fprintf(stderr, "Error: %ls\n", hid_error(handle));
		return 0;
	}
	res = readData(handle, buf, bufsize, showdata, 0);
	return res;
}

/** Read response from the device */
int readData(hid_device *handle, unsigned char *buf, int bufsize,
             bool showdata, int offset)
{
	int i,res;

	memset(buf, 0, bufsize*sizeof(unsigned char));

	res = 0;
	for (i=1; i<15; i++) {
		res = hid_read(handle, buf, bufsize*sizeof(unsigned char));
		if (res > 1) break;
		if (res == 0){
			//printf("waiting...\n");
			usleep(500*1000);
		}
		if (res < 0)
			fprintf(stderr, "Error: Unable to read()\n");
	}
	if (res<=0) {
		//printf("No data received!\n");
		return 0;
	}
	if (showdata) {
		for (i = 0; i < res; i++)
			printf("%02hhx ", buf[i]);
		printf("\n");
	}
	return res;
}
