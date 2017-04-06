/*
 * SDCard_Test_Program.h
 *
 *  Created on: 2017-02-10
 *      Author: Yueyue Zhang
 */

#ifndef SDCARD_TEST_PROGRAM_H_
#define SDCARD_TEST_PROGRAM_H_

#define true 1
#define false 0

void TestSDCard();
void WriteToFile();
void GetBitmap(char* fileName, int bitmap[]);
void ReadFromFile(char* fileName, short int bitmap[], int close);

#endif /* SDCARD_TEST_PROGRAM_H_ */
