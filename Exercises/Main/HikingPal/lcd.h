/*
 * lcd.h
 *
 *  Created on: Feb 16, 2017
 *      Author: Anthony
 */

#ifndef LCD_H_
#define LCD_H_

void InitializeLCD();
void WriteToLCD(char* latitude, char* longitude, char lat_direction, char lon_direction);

#endif /* LCD_H_ */
