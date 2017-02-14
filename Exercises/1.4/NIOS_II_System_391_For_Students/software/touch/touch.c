/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "touch.h"

#define Touchscreen_Status 		(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_Control 	(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_TxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_RxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_Baud    	(*(volatile unsigned char *)(0x84000234))

int putCharTouch(char c){
	// Wait for TX bit in status register to turn 1 (means transmit is empty)
	while((Touchscreen_Status & 0x02) != 0x02);
	// Send the data to TX (reduce to 8 bits)
	return c & 0xFF;
}

int getCharTouch(void)
{
	// poll Rx bit in 6850 status register. Wait for it to become '1'
	// read received character from 6850 RxData register.
	while ((Touchscreen_Status & 0x01) != 0x01);
	return Touchscreen_RxData;
}

/*****************************************************************************
** Initialise touch screen controller
*****************************************************************************/
void Init_Touch(void)
{
	 // Program 6850 and baud rate generator to communicate with touchscreen
	 // send touchscreen controller an "enable touch" command

	// Divide by 16 clock, RTS Low, 8 bits of data, no parity,
	// 1 stop bit, transmitter interrupt disabled
	Touchscreen_Control = 0x15;
	// 9600 BAUD
	Touchscreen_Baud = 0x05;

	// slight delay to process
	usleep(10000);

	// Send TOUCH_ENABLE command
	putCharTouch(0x55);
	putCharTouch(0x01);
	putCharTouch(0x12);

	usleep(10000);
}

/*****************************************************************************
** test if screen touched
*****************************************************************************/
int ScreenTouched( void )
{
	 // return TRUE if any data received from 6850 connected to touchscreen
	 // or FALSE otherwise

	return (Touchscreen_RxData == 0x80);
}

/*****************************************************************************
** wait for screen to be touched
*****************************************************************************/
void WaitForTouch()
{
	while(!ScreenTouched())
		;
}

Point GetPen(void){
	Point p1;
	int packets[4];
	// wait for a pen down command then return the X,Y coord of the point
	// calibrated correctly so that it maps to a pixel on screen

	// Wait for first packet of touch
	WaitForTouch();

	int i;
	for(i = 0; i < 4; i++){
		packets[i] = getCharTouch();
	}

	// Get x11 : x7 from 2nd packet, and concatenate to x6 : x0 from 1st packet
	p1.x = (packets[1] << 7) | packets[0];
	p1.y = (packets[3] << 7) | packets[2];

	// Map from controller resolution to screen pixel
	p1.x = p1.x * 799 / 4095;
	p1.y = p1.y  * 479 / 4095;

	printf("x = %d ", p1.x);
	printf("y = %d\n", p1.y);

	return p1;
}

/*****************************************************************************
* This function waits for a touch screen press event and returns X,Y coord
*****************************************************************************/
Point GetPress(void)
{
	 Point p1;
	 // wait for a pen down command then return the X,Y coord of the point
	 // calibrated correctly so that it maps to a pixel on screen

	 return GetPen();
}
/*****************************************************************************
* This function waits for a touch screen release event and returns X,Y coord
*****************************************************************************/
Point GetRelease(void)
{
	 Point p1;
	 // wait for a pen up command then return the X,Y coord of the point
	 // calibrated correctly so that it maps to a pixel on screen
	 return GetPen();
}

void TouchLoop(){
	Init_Touch();
	while(1){
		GetPress();
	  	GetRelease();
	}
}

int main()
{
  printf("Hello from Nios II!\n");

  TouchLoop();

  return 0;
}
