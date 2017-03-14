/*
 * bluetooth.h
 *
 *  Created on: Mar 13, 2017
 *      Author: Anthony
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define BLUETOOTH_Control (*(volatile unsigned char *)(0x84000220))
#define BLUETOOTH_Status (*(volatile unsigned char *)(0x84000220))
#define BLUETOOTH_TxData (*(volatile unsigned char *)(0x84000222))
#define BLUETOOTH_RxData (*(volatile unsigned char *)(0x84000222))
#define BLUETOOTH_Baud (*(volatile unsigned char *)(0x84000224))

void init_Bluetooth(void);
char putChar_Bluetooth(char c);
char getChar_Bluetooth(void);
void send_command(char string[], int length);
void start_command();
void end_command();
void dongle_reset();
int setup_all_bluetooth();

#endif /* BLUETOOTH_H_ */
