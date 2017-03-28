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
#include "io.h"

#define BT_RATE_TRAIL 'Q'
#define BT_WEATHER 'Z'
#define BT_MAP 'X'
#define MAX_MAPS 20

#define SWITCHES (volatile char *) 0x00001050

typedef enum{
	None,
	Weather,
	Rating,
	Map,
} State;

int main(){

	printf("Hello from Nios II!\n");
	//set_user_pass();

	// Initialize hardware
	init_btport();
	Init_Touch();
	ResetScreen();
	DrawString2Center(100, BLACK, WHITE, "Connect a bluetooth device!", 0);

	// Initialize helper variables
	char weatherData[150];
	SavedMapButton** maps = malloc(sizeof(SavedMapButton*) * MAX_MAPS);
	volatile int switches = IORD_16DIRECT(SWITCHES, 0);
	int i = 0;
	State state = None;

	SetMockedMapData(maps);
	DrawAllSavedMapButtons(maps);

	state = Map;

	while(1){
		while(0){
			char c = getchar_poll();
			printf("%c\n", c);

			// Rate trail init
			if(c == BT_RATE_TRAIL){
				ResetScreen();
				DrawString2Center(400, BLACK, WHITE, weatherData, 0);
				DrawString2Center(100, BLACK, WHITE, "Rate the trail!", 0);
				DrawRatings(5, BLACK);
				state = Rating;
				break;
			}

			// Weather data init
			else if(c == BT_WEATHER){
				if(state == None){
					state = Weather;
					i = 0;
				}
				// Done parsing weather data
				else{
					state = None;
					printf(weatherData);
					DrawFilledRectangle(0, XRES, 390, YRES, WHITE);
					DrawString2Center(400, BLACK, WHITE, weatherData, 0);
				}
			}

			// Saved map data init
			else if(c == BT_MAP){

			}

			// Start parsing weather data
			else if(state == Weather){
				weatherData[i++] = c;
			}
		}

		switches = IORD_16DIRECT(SWITCHES, 0);
		printf("lala");

		// Done receiving data for now. Check for touch/switch/button input.
		while(1){
			int switchesNew = IORD_16DIRECT(SWITCHES, 0);
			printf("%d, %d\n", switchesNew, switches);
			int bit = 0;
			// Find first switch that is different. That will be the input switch.
			if(switches != switchesNew){
				printf("lol");
				for(bit = 0; bit < 16; ++bit){
					if((switches & (1 << bit)) != (switchesNew & (1 << bit))){
						break;
					}
				}
				if(bit == 16){
					bit = -1;
				}
				switches = switchesNew;
			}

			//printf("%d\n", bit);

			if (CheckForTouch()){
				Point p = GetPen();
				// Rate the trail
				if(state == Rating){
					int star = CheckRatingPress(p.x, p.y);
					printf("%d\n", star);
					if(star != -1){
						char send[1];
						send[0] = star + 1 + '0';
						send_string(send, 1);

						ResetScreen();
						DrawString2Center(400, BLACK, WHITE, weatherData, 0);
						DrawRatings(star + 1, YELLOW);
						char msg[50];
						sprintf(msg, "You rated %d stars!", star + 1);
						DrawString2Center(100, BLACK, WHITE, msg, 0);

						state = None;
						break;
					}
				}
				// Display map trail history
				else if(state == Map){
					int button = CheckSavedMapButtonPress(maps, p.x, p.y);
					if(button != -1){
						printf("%d", button);
						DrawSavedMapData(maps[button]);
						HighlightSavedMapButton(maps, maps[button]);
					}
				}
			}
		}
	}
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
