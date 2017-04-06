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
#include "switches.h"
#include "io.h"
#include "keyboard.h"
#include "HikingPal.h"

char testData[] = "XQU1030900670U0U0U1UU2017-04-05 02:29:12UQU1030911502U0U0U3UU2017-04-05 02:29:23UQU1032221705U1U0U6UU2017-04-05 02:51:13UQX";

void FreeAllMaps(SavedMapButton** maps, size_t num_maps){
	int i;
	for(i = 0; i < num_maps; ++i){
		FreeMap(maps[i]);
	}
}

void FreeMap(SavedMapButton* map){
	if(map == NULL){
		return;
	}

	free(map->name);
	free(map->date);
	free(map->locations);
	free(map->distance);
	free(map->duration);
	free(map->rating);
	free(map);
}


// Parse the map data sent by bluetooth into structs
int ParseMapData(char mapData[], SavedMapButton** maps, size_t num_maps){
	int i;
	int currMap = 0;
	char data[MAX_DATA_SIZE];

	// Clear all map data first
	FreeAllMaps(maps, num_maps);

	for(i = 0; mapData[i] != '\0'; i++){
		// Indicates start of a new map
		if(mapData[i] == BT_MAP_DELIMITER){
			// Check if it's the last map delimiter. If so, we're done.
			if(mapData[i + 1] == '\0'){
				break;
			}

			currMap++;
			maps[currMap - 1] =  malloc(sizeof(SavedMapButton));
			maps[currMap - 1]->date = NULL;
			maps[currMap - 1]->distance = NULL;
			maps[currMap - 1]->duration = NULL;
			maps[currMap - 1]->locations = NULL;
			maps[currMap - 1]->name = NULL;
			maps[currMap - 1]->rating = NULL;
			maps[currMap - 1]->x = 50;
			maps[currMap - 1]->y = 20 + (currMap - 1) * (BUTTON_HEIGHT * 1.2);
			continue;
		}
		// Parse each data of the map until we hit the delimiter again
		// j keeps track of property (name, rating, distance, duration, locations, date respectively)
		// k keeps track of data index
		int j = 0;
		int k = 0;
		while(mapData[i] != BT_MAP_DELIMITER){
			// Indicates start of a new property
			if(mapData[i] == BT_MAP_FIELD_DELIMITER){
				// Check if empty property
				if(mapData[i + 1] == BT_MAP_FIELD_DELIMITER){
					i++;
					j++;
					continue;
				}
				// Check if last property
				if(mapData[i + 1] == BT_MAP_DELIMITER){
					i++;
					continue;
				}
				memset(&data[0], 0, sizeof(data));
				k = 0;
				i++;
				continue;
			}
			// Parse the property data
			while(mapData[i] != BT_MAP_FIELD_DELIMITER){
				data[k++] = mapData[i++];
			}

			printf("%s\n", data);

			switch(j){
				case 0:
					maps[currMap - 1]->name = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->name, data);
					break;
				case 1:
					maps[currMap - 1]->rating = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->rating, data);
					break;
				case 2:
					maps[currMap - 1]->distance = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->distance, data);
					break;
				case 3:
					maps[currMap - 1]->duration = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->duration, data);
					break;
				case 4:
					maps[currMap - 1]->locations = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->locations, data);
					break;
				case 5:
					maps[currMap - 1]->date = malloc(sizeof(char) * k);
					strcpy(maps[currMap - 1]->date, data);
					break;
			}

			j++;
		}

		i--;
	}

	return currMap;
}

// Reset the entire screen and re-draw weather data
void ResetScreenWithWeatherMessage(int hasWeather, char weatherData[], char weatherIcon[], int hasMessage, char message[]){
	ResetScreen();

	if(hasWeather){
		char weatherBuffer[8];
		sprintf(weatherBuffer, "%s.BMP", weatherIcon);

		char weatherData1[100];
		char weatherData2[100];
		int i;
		for(i = 0; weatherData[i] != '\n' && weatherData[i] != '\0' && i < 100; ++i){
			weatherData1[i] = weatherData[i];
		}
		weatherData1[i] = '\0';
		int j;
		for(j = 0; weatherData[i] != '\0' && j < 100; ++j){
			weatherData2[j] = weatherData[i++];
		}
		weatherData2[j] = '\0';

		DrawString2Center(400, BLACK, WHITE, weatherData1, 0);
		DrawString2Center(430, BLACK, WHITE, weatherData2, 0);

		DrawRectangle(18, 22 + 72, 438 -  72, 442, BLACK);
		DrawRectangle(XRES - 22 - 72, XRES - 18, 438 - 72, 442, BLACK);
		DrawHorizontalLine(0, XRES, 430 - 72, BLACK);
		DrawMapSDCard(weatherBuffer, 20, 440, 72, 72, 1, false);
		DrawMapSDCard(weatherBuffer, XRES - 20 - 72, 440, 72, 72, 1, true);
	}

	DrawMessage(hasMessage, message);
}

void DrawMessage(hasMessage, message){
	if(hasMessage){
		DrawFilledRectangle(0, XRES, 0, 30, WHITE);
		DrawString2Center(10, BLACK, WHITE, message, 0);
	}
}

// Reset just the upper screen (ignore weather portion)
void ResetUpperScreen(hasMessage, message){
	DrawFilledRectangle(0, XRES, 0, 350, WHITE);
	DrawMessage(hasMessage, message);
}

void ResetScreenWithMessage(){
	ResetScreen();
	DrawString2Center(100, BLACK, WHITE, "Connect a bluetooth device!", 0);
}

int main(){

	printf("Hello from Nios II!\n");
	//set_user_pass();

	// Initialize hardware
	init_btport();
	removeBtBuffer();
	Init_Touch();
	InitializeLCD();
	ProgramAllPalette();
	TestSDCard();

	// Initialize helper variables
	char weatherData[150];
	char weatherIcon[4];
	char gpsData[20];
	char mapData[500];
	char keyInput[100];
	char message[100];
	int keyIndex = 0;
	SavedMapButton** maps = malloc(sizeof(SavedMapButton*) * MAX_MAPS);
	size_t num_maps = 0;
	int selectedMap = -1;
	int hasWeather = 0;
	int hasMessage = 0;
	volatile int switches = IORD_16DIRECT(SWITCHES, 0);
	int i = 0;
	State state = None;
	State prevState = None;

	ResetScreenWithMessage();

	while(1){
		while(1){
			char c = getchar_btport();

			// Check for keyboard button press
			if(state == None && IORD_8DIRECT(BUTTONS, 0) == BUTTON1_ON){
				state = Keyboard;
//				memset(&keyInput, 0, sizeof(keyInput));
//				keyIndex = 0;
				DrawKeyboard();
				DrawMessage(hasMessage, message);
				if(keyIndex > 0){
					DrawString2Center(250, BLACK, WHITE, keyInput, 0);
				}
				break;
			}

			if(c == '\0'){
				continue;
			}

			printf("%c\n", c);

			// Rate trail init
			if((state == None || state == Rating) && c == BT_RATE_TRAIL){
				if (state == Keyboard){
					ResetScreen();
				}
				else{
					ResetUpperScreen(hasMessage, message);
				}

				DrawString2Center(100, BLACK, WHITE, "Rate the trail!", 0);
				DrawRatings(5, BLACK);
				state = Rating;
				break;
			}

			// Weather data init
			else if((state == None || state == Weather) && c == BT_WEATHER){
				// First time we see BT_WEATHER, go to Weather state
				if(state != Weather){
					state = Weather;
					memset(&weatherData, 0, sizeof(weatherData));
					memset(&weatherIcon, 0, sizeof(weatherIcon));
					i = 0;
				}
				// Done parsing weather data if we see BT_WEATHER again
				else{
					state = None;
					hasWeather = true;
					printf(weatherData);
					printf(weatherIcon);

					ResetScreenWithWeatherMessage(hasWeather, weatherData, weatherIcon, hasMessage, message);
				}
			}

			// GPS data init
			else if((state == None || state == Gps) && c == BT_GPS){
				if(state != Gps){
					state =  Gps;
					memset(&gpsData, 0, sizeof(gpsData));
					i = 0;
				}
				// Done parsing GPS data if we see BT_GPS again
				else{
					state = None;
					printf(gpsData);
					WriteStringToLCD(gpsData);
				}
			}

			// Saved map data init
			else if((state == None || state == Map) && c == BT_MAP_INIT){
				if(state != Map){
					state = Map;
					memset(&mapData, 0, sizeof(mapData));
					i = 0;
					printf("Got it\n");
				}
				else{
					num_maps = ParseMapData(mapData, maps, num_maps);
					ResetUpperScreen(hasMessage, message);
					DrawAllSavedMapButtons(maps, num_maps);
					//printf(mapData);
					break;
				}
			}

			// Message sent
			else if((state == None || state == Message) && c == BT_MESSAGE){
				if(state != Message){
					state = Message;
					i = 0;
					printf("Message being sent\n");
				}
				else{
					hasMessage = true;
					if(prevState == Keyboard){
						DrawKeyboard();
					}
					DrawMessage(hasMessage, message);
					state = prevState;
					break;
				}
			}

			// Start parsing weather data
			else if(state == Weather){
				if(i < 3){
					weatherIcon[i++] = c;
				}
				else{
					weatherData[i++ - 3] = c;
				}
			}

			// Start parsing gps data
			else if(state == Gps){
				gpsData[i++] = c;
			}

			// Start parsing map data
			else if(state == Map){
				mapData[i++] = c;
				printf("%c", mapData[i]);
			}
			else if(state == Message){
				message[i++] = c;
				printf("%c", message[i]);
			}
		}

		switches = IORD_16DIRECT(SWITCHES, 0);

		// Done receiving data for now. Check for touch/switch/button input.
		while(1){
			int switchesNew = IORD_16DIRECT(SWITCHES, 0);
			int bit = -1;
			// Find first switch that is different. That will be the input switch.
			if(switches != switchesNew){
				for(bit = 0; bit < 16; ++bit){
					if((switches & (1 << bit)) != (switchesNew & (1 << bit))){
						break;
					}
				}
				if(bit == 16){
					bit = -1;
				}
				switches = switchesNew;
				printf("%d\n", bit);
			}

			Point p = {.x = -1, .y = -1};
			if (CheckForTouch()){
				p = GetPen();
			}

			if (p.x != -1 || bit != -1){
				// Rate the trail
				if(state == Rating){
					int star = CheckRatingPress(p.x, p.y);
					// If no touch input, check switches
					if(star == -1 && bit >= 0 && bit <= 4){
						star = bit;
					}
					printf("%d\n", star);
					if(star != -1){
						char send[4];
						sprintf(send, "%c%d%c", BT_RATE_TRAIL, star + 1, BT_RATE_TRAIL);
						printf(send);
						send_string(send, 3);

						ResetUpperScreen(hasMessage, message);

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
					int button = CheckSavedMapButtonPress(maps, num_maps, p.x, p.y);

					// If no touch input, check switches
					if(button == -1 && bit >= 0 && bit < num_maps){
						button = bit;
					}

					if(button != -1 && button != selectedMap){
						printf("%d", button);
						selectedMap = button;
						DrawSavedMapData(maps[button]);
						HighlightSavedMapButton(maps, maps[button], num_maps);
					}

					char mapName[100];
					sprintf(mapName, "%c%s%c", BT_MAP_ID, maps[button]->name, BT_MAP_ID);
					send_string(mapName, 100);
					printf("ID Sent: %s", mapName);
				}

				else if(state == Keyboard && p.x != -1){
					char c = GetCharPress(p.x, p.y);
					if(c != '\0'){
						printf("%c\n", c);

						// Handle regular character
						if(c != 'B' && c != 'E'){
							keyInput[keyIndex++] = c;
							keyInput[keyIndex + 1] = '\0';
						}
						// Handle backspace
						if(c == 'B' && keyIndex > 0){
							keyInput[keyIndex - 1] = '\0';
							keyIndex--;
						}
						// Handle enter key
						else if (c == 'E'){
							char message[100];
							sprintf(message, "%c%s%c", BT_MESSAGE, keyInput, BT_MESSAGE);
							send_string(message, keyInput + 2);
							printf("Message: %s\n", message);
						}

						DrawFilledRectangle(0, XRES, 250, 275, WHITE);
						DrawString2Center(250, BLACK, WHITE, keyInput, 0);
					}

					int lastTime = time(NULL);
					// Skip check for release (otherwise double input
					while(!CheckForTouch()){
						if(time(NULL) - lastTime >= 1){
							break;
						}
					}

					if(CheckForTouch()){
						GetPen();
					}
				}
			}
			else{
				// No input, check for incoming bluetooth data
				if(test_getchar()){
					printf("Bluetooth inc\n");
					if(state == Keyboard){
						ResetScreen();
						DrawMessage(hasMessage, message);
					}

					prevState = state;
					state = None;
					selectedMap = -1;
					break;
				}
				// Check for close keyboard button
				if(IORD_8DIRECT(BUTTONS, 0) == BUTTON1_ON){
					// Close keyboard if its on
					if(state == Keyboard){
						state = None;
						if(hasWeather){
							ResetScreenWithWeatherMessage(hasWeather, weatherData, weatherIcon, hasMessage, message);
						}
						// Otherwise, no bluetooth
						else{
							ResetScreenWithMessage();
						}
						// Wait until button is released
						while(IORD_8DIRECT(BUTTONS, 0) == BUTTON1_ON);
						break;
					}
					// Turn keyboard on if its off
					else{
						DrawKeyboard();
						DrawMessage(hasMessage, message);
						state = Keyboard;
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

  DrawMapSDCard("MAP1.BMP", 0, 400, 160, 80, 5, true);

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
		 DrawMapSDCard(currMapName, 0, 400, 160, 80, 5, true);
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
					  DrawMapSDCard(mapButtons[mapButton].mapName, 450, 280, 160, 80, 2, 1);
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
