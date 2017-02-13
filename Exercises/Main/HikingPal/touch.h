/*
 * touch.h
 *
 *  Created on: Feb 13, 2017
 *      Author: Anthony
 */

#ifndef TOUCH_H_
#define TOUCH_H_

#define Touchscreen_Status 		(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_Control 	(*(volatile unsigned char *)(0x84000230))
#define Touchscreen_TxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_RxData 		(*(volatile unsigned char *)(0x84000232))
#define Touchscreen_Baud    	(*(volatile unsigned char *)(0x84000234))

/* a data type to hold a point/coord */
typedef struct { int x, y; } Point ;

int putCharTouch(char c);
int getCharTouch(void);
void Init_Touch(void);
int ScreenTouched(void);
void WaitForTouch();
Point GetPen(void);
Point GetPress(void);
Point GetRelease(void);
void TouchLoop(void);

#endif /* TOUCH_H_ */
