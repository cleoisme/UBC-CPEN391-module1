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
#include "graphics.h"
#include "gps.h"
#include "wifi.h"
#include "touch.h"
#include "Colours.h"

int main()
{
  printf("Hello from Nios II!\n");
  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
  DrawButtons();
  Init_Touch();

  int press = 0;
  while(1){
	  if (CheckForTouch()){
		  press++;
		  if(press == 2){
			  // release
			  DrawButtons();
			  press = 0;
			  continue;
		  }
		  // press
		  Point p = GetPen();
		  int button = CheckButtonPress(p.x, p.y);
		  DrawButtonPress(button);
		  //printf("%d", p.x);
		  //printf("%d", p.y);

	  }
  }

  //getLocationData();
  //ProgramAllPalette();
  //TestSDCard();
  //int bmp[160*80*3 + 54];
  //DrawMapSDCard("MAP.BMP", 0, 300, 160, 80, 5);
  //DrawMap2("/mnt/host/map160x80final.bmp", 0, 0, 200, 100, 4);
  //GetBitmap("MAP.BMP", bmp);
  //ReadFromFile("MAP.BMP", bmp);
  //Wifi_Init();
  //TouchLoop();

  return 0;
}
