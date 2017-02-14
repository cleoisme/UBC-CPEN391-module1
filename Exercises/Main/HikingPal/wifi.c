#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "wifi.h"

void Wifi_Init(void) {
	// Reset
	WifiDongle_Control = 0x03;
	WifiDongle_Control = 0x15;
	WifiDongle_Baud = 0x01;
}


char getcharWifi(void) {
	while((WifiDongle_Status & 0x01) != 0x01) {}
	return WifiDongle_RxData;
}

void putcharWifi(char val) {
	while((WifiDongle_Status & 0x02) != 0x02) {}
	WifiDongle_TxData = val;
}

void WifiWait(void) {
	char val = getcharWifi();
	printf (val);
	while(val != '>') {
		val = getcharWifi();
		printf("%c", val);
	}

	printf("\n");
}

void Wifi_SendCommand(const char * command) {
	int i;
	int length =strlen(command);
	for(i=0; i<length;i++) {
		putcharWifi(command[i]);
	}
}
