/*
 * Exercise 1.6 for CPEN 391.
 * Connects to the bluetooth dongle and sets its name and password.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include "bluetooth2.h"

#define DONGLENAME "TEAM9BLUETOOTH"
#define NAMELEN 15
#define DONGLEPASS "9999"
#define PASSLEN 4

#define BT_CONTROL (*(volatile unsigned char *)(0x84000220))
#define BT_STATUS (*(volatile unsigned char *)(0x84000220))
#define BT_TXDATA (*(volatile unsigned char *)(0x84000222))
#define BT_RXDATA (*(volatile unsigned char *)(0x84000222))
#define BT_BAUD (*(volatile unsigned char *)(0x84000224))

#define BT_STATUS_TX_MASK 0x02
#define BT_STATUS_RX_MASK 0x01

//call this function at the start of the program before
//attempting to read or write via BT port
void init_btport(void)
{
	//set up 6850 control register to utilize a divide by 16 clock,
	//set RTS low, use 8 bits of data, no parity, 1 stop bit
	//transmitter interrupt disabled
	//program baud rate generator to use 115k baud
	BT_CONTROL = 0x15;
	BT_BAUD = 0x01;
}

char putchar_btport(char c)
{
	//poll Tx bit in 6850 status register. Wait for it to become '1'
	//write 'c' to the 6850 TxData register to output the character
	while(1)
	{
		if (BT_STATUS & BT_STATUS_TX_MASK)
		{
			BT_TXDATA = c;
			break;
		}
	}
	return c;
}

int test_getchar(void)
{
	return BT_STATUS & BT_STATUS_RX_MASK;
}

char getchar_btport(void)
{
	//poll Rx bit in 6850 status register
	if (BT_STATUS & BT_STATUS_RX_MASK) {
		return BT_RXDATA;
	} else {
		return '-';
	}
}

char getchar_poll(void){
	while(!(BT_STATUS & BT_STATUS_RX_MASK)){

	}

	return BT_RXDATA;
}

void receive_string(char buffer[], int maxlen)
{
	int i = 0;
	while ( i < maxlen)
	{
		if ((buffer[i] = getchar_btport()) != '-')
		{
			//printf("FDFD\n");
			printf("%c", buffer[i]);
			if (buffer[i] == '\0' || buffer[i] == '?')
			{
				printf("boom");
				break;
			}
			usleep(100000);
			i++;
		}
	}
	//DEBUG PRINT
	buffer[i] = '\0';
}

void send_string(char string[], int length)
{
	int i;
	for (i = 0; i < length && string[i] != '\0'; i++)
	{
		if(i != 0){
			usleep(100000); //100ms wait
		}
		putchar_btport(string[i]);
	}
}

// Enters command mode, with proper delays
void command_start()
{
	usleep(1000000); // 1s wait
	send_string("$$$", 3);
	usleep(1000000);
}

// Exits command mode, with proper delays
void command_end()
{
	usleep(1000000); // 1s wait
	send_string("---\r\n", 5);
	usleep(1000000);
}

// Resets the bluetooth dongle to default settings.
void reset_dongle()
{
	command_start();
	send_string("SF,", 3);
	send_string("1\r\n", 3);
	command_end();
}

// Sets the bluetooth dongle's name to the given string.
void set_dongle_name(char name[], int length)
{
	command_start();
	send_string("SN,", 3);
	send_string(name, length);
	send_string("\r\n", 2);
	command_end();
}

// Sets the bluetooth dongle's password to the given string.
void set_dongle_pass(char pass[], int length)
{
	command_start();
	send_string("SP,", 3);
	send_string(pass, length);
	send_string("\r\n", 2);
	command_end();
}

void set_user_pass(){
	const char dongleName[] = DONGLENAME;
	const int nameLen = NAMELEN;
	const char donglePass[] = DONGLEPASS;
	const int passLen = PASSLEN;

	printf("Begin programming Bluetooth dongle...\n");
	init_btport();

	printf("Setting name to '%s'.\n", dongleName);
	set_dongle_name(dongleName, nameLen);

	printf("Setting password to '%s'.\n", donglePass);
	set_dongle_pass(donglePass, passLen);


	printf("Programming finished.\n");

}

int setup_all_bluetooth2()
{
	set_user_pass();

	char str[] = "A";
	char str2[] = "I love you";
	char buf[1024];

	while(1){
		usleep(1000000);
//		send_string(str, 1);
//		usleep(1000000);
//		send_string(str2, 1);
		send_string(str2, sizeof(str2) / sizeof(str[0]));
		//printf("hihi\n");
		//receive_string(buf, 1024);
		//printf(buf);
		//char c = getchar_poll();
		//printf("%c\n", c);
	}


	return 0;
}
