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
#include <time.h>

int main()
{
  printf("Hello from Nios II!\n");
  DrawHorizontalLine(0, 800, 400, CYAN);
  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
  DrawButtons();
  //Init_Touch();

  TestShapes();
  //TestSDCard();
  int currMap = 0;
  int savedMaps[10];
  MapButton mapButtons[10];
  int startTime = time(NULL);
  int endTime = time(NULL);
  int state = 0;

  int press = 0;
  while(0){
	 endTime = time(NULL);
	 if (endTime - startTime >= 10){
		 startTime = endTime;
		 currMap++;
		 // Draw the next map on the screen

	 }

	  if (CheckForTouch()){
		  press++;
		  Point p = GetPen();
		  int button = CheckButtonPress(p.x, p.y);
		  if(press == 1){
			  // Press
			  DrawButtonPress(button);
		  }
		  else{
			  // Release

			  // View Saved Maps
			  if(button == 1){
				  state = 1;
				  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
				  DrawSavedMaps(savedMaps, mapButtons);
			  }

			  // Save Map
			  else if(button == 2){
				  int i;

				  // Add to saved maps array
				  for(i = 0; i < 10; ++i){
					  if (savedMaps[i] != 0){
						  if (savedMaps[i] != currMap){
							  savedMaps[i] = currMap;
						  }
						  else{
							  break;
						  }
					  }
				  }
			  }

			  // Pause/Resume
			  else if(button == 3){

			  }
			  press = 0;

			  // release
			  //DrawButtons();
			  //press = 0;
			  //continue;
		  }
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
