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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "graphics.h"
#include "gps.h"
#include "wifi.h"
#include "touch.h"
#include "Colours.h"
#include "SDCard_Test_Program.h"
#include "lcd.h"
#include "bluetooth.h"
#include "bluetooth2.h"

int main(){

	printf("Hello from Nios II!\n");
	setup_all_bluetooth2();

	return 0;
}

int run_hiking_pal()
{
  printf("Hello from Nios II!\n");

  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
  DrawButtons();
  Init_Touch();
  InitializeLCD();
  ProgramAllPalette();

  // Set up LCD
  char* lo = malloc(sizeof(char) * 10);
  char* la = malloc(sizeof(char) * 10);
  AssignGPSData(1, lo, la);
  printf("Lo:%s", lo);
  printf("La: %s", la);
  WriteToLCD(la, lo, 'N', 'W');

  TestSDCard();

  DrawMapSDCard("MAP1.BMP", 0, 400, 160, 80, 5);

  int currMap = 1;
  char* currMapName = "MAP1.BMP";
  int savedMaps[10];
  MapButton mapButtons[10];
  int startTime = time(NULL);
  int endTime = time(NULL);
  int state = 0;
  int stop = 0;
  int press = 0;
  int refresh = 0;

  int i;
  for(i = 0; i < 10; ++i){
	  savedMaps[i] = 0;
	  mapButtons[i].x = -100;
	  mapButtons[i].y = -100;
	  mapButtons[i].mapName = NULL;
	  mapButtons[i].distance = 0;
	  mapButtons[i].time = 0;
	  mapButtons[i].latitude =0;
	  mapButtons[i].longitude = 0;
  }

  while(1){
	 endTime = time(NULL);
	 if (state == 0 && (endTime - startTime) >= 25){
		 if(stop && !refresh){
			 startTime = endTime;
			 printf("NOPE");
			 continue;
		 }

		 startTime = endTime;
		 currMap++;
		 refresh = 0;

		 // Draw the next map on the screen

		 // Create the string map name

		 char *num[2];
		 sprintf(num, "%d", currMap);
		 currMapName = concat("MAP", num);
		 currMapName = concat(currMapName, ".BMP");

		 printf(currMapName);
		 DrawMapSDCard(currMapName, 0, 400, 160, 80, 5);
	 }

	  if (CheckForTouch()){
		  press = press == 0 ? 1 : 0;
		  Point p = GetPen();

		  int button = CheckButtonPress(p.x, p.y);
		  int mapButton = CheckMapButtonPress(mapButtons, p.x, p.y);

		  if(press == 1){
			  // Press
			  printf("Press");

			  // Check if button pressed in main mode
			  if(state == 0){
				  DrawButtonPress(button);
			  }

			  // Check map button pressed in 'View Trails' mode
			  if(state == 1 && mapButton != -1){

				  // Check if back button pressed
				  if(mapButton == 11){
					  DrawBackButtonPress();
				  }
				  // Map button was pressed
				  else{
					  printf(mapButtons[mapButton].mapName);
					  DrawMapButtonPress(mapButtons[mapButton]);
				  }
			  }
		  }
		  else{
			  printf("Release");
			  // Release

			  if(state == 0){
				  ReleaseButtonPress(button);
			  }

			  // Check if map button released in 'View Trails' mode
			  if(state == 1 && mapButton != -1){
				  if(mapButton == 11){
					  // Back button released, go back to main mode
					  state = 0;
					  stop = 0;
					  startTime = endTime - 25;
					  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
					  DrawButtons();
				  }
				  else{
					  ReleaseMapButtonPress(mapButtons[mapButton]);
					  printf("HERE:");
					  printf(mapButtons[mapButton].mapName);
					  DrawMapData(mapButtons[mapButton], currMap);
					  DrawMapSDCard(mapButtons[mapButton].mapName, 450, 280, 160, 80, 2);
				  }
			  }

			  // View Saved Maps
			  if(state == 0 && button == 1){
				  state = 1;
				  int i;
				  for(i = 0; i < 10; ++i){
					  if(savedMaps[i] == 0){
						  break;
					  }
					  printf("%d", savedMaps[i]);
				  }

				  DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
				  DrawSavedMaps(savedMaps, mapButtons);
			  }

			  // Save Map
			  else if(state == 0 && button == 2){
				  int i;
				  printf("Save\n");
				  // Add to saved maps array
				  for(i = 0; i < 10; ++i){
					  if (savedMaps[i] == 0){
						  if (i == 0 || (i > 0 && savedMaps[i - 1] != currMap)){
							  savedMaps[i] = currMap;
							  printf("Save Index: %d", i);
						  }
						  break;
					  }
					  else{
						  //printf(savedMaps[i]);
					  }
				  }
			  }

			  // Pause/Resume
			  else if(state == 0 && button == 3){
				  stop = stop == 0 ? 1 : 0;
				  ReleaseStopOrPause(stop);
				  if(stop == 0){
					  startTime = time(NULL);
				  }
			  }

			  // Refresh map
			  else if(state == 0 && button == 4){
				  currMap++;
				  startTime = endTime - 25;
				  refresh = 1;
			  }
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
