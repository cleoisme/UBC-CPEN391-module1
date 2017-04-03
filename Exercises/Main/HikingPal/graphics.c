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
#include <stddef.h>
#include <time.h>
#include "graphics.h"
#include "Colours.h"
#include "touch.h"
#include "SDCard_Test_Program.h"

extern const unsigned int ColourPalletteData[256];

/**********************************************************************
* This function writes a single pixel to the x,y coords specified in the specified colour
* Note colour is a palette number (0-255) not a 24 bit RGB value
**********************************************************************/
void WriteAPixel (int x, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsColourReg = Colour;		// set pixel colour with a palette number
	GraphicsCommandReg = PutAPixel;		// give graphics a "write pixel" command
}

/*****************************************************************************************
* This function read a single pixel from x,y coords specified and returns its colour
* Note returned colour is a palette number (0-255) not a 24 bit RGB value
******************************************************************************************/
int ReadAPixel (int x, int y)
{
	WAIT_FOR_GRAPHICS;			// is graphics ready for new command

	GraphicsX1Reg = x;			// write coords to x1, y1
	GraphicsY1Reg = y;
	GraphicsCommandReg = GetAPixel;		// give graphics a "get pixel" command

	WAIT_FOR_GRAPHICS;			// is graphics done reading pixel
	return (int)(GraphicsColourReg) ;		// return the palette number (colour)
}

void DrawHorizontalLine(int x1, int x2, int y, int Colour)
{
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y;
	GraphicsY2Reg = y;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawHLine;
}

void DrawVerticalLine(int y1, int y2, int x, int Colour)
{
	WAIT_FOR_GRAPHICS;

	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsX1Reg = x;
	GraphicsX2Reg = x;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawVLine;
}

void DrawBresenhamLine(int x1, int x2, int y1, int y2, int Colour)
{
	WAIT_FOR_GRAPHICS;

	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawLine;
}

void DrawString1(int x, int y, int colour, int background, char* string, int erase){
	int i;
	for (i = 0; string[i] != '\0'; ++i){
		OutGraphicsCharFont1(x, y, colour, background, string[i], erase);
		x += 10;
	}
}

void DrawString2(int x, int y, int colour, int background, char* string, int erase){
	int i;
	for (i = 0; string[i] != '\0'; ++i){
		OutGraphicsCharFont2(x, y, colour, background, string[i], erase);
		x += 14;
	}
}

void DrawString2Center(int y, int colour, int background, char* string, int erase){
	size_t length = strlen(string) * 14;
	size_t offset = (XRES - length)/2;
	DrawString2(offset, y, colour, background, string, erase);
}

void DrawString2CenterRange(int x1, int x2, int y, int colour, int background, char * string, int erase){
	size_t length = strlen(string) * 14;
	size_t offset = x1 + (x2 - x1 - length)/2;
	DrawString2(offset, y, colour, background, string, erase);
}

void DrawRectangle(int x1, int x2, int y1, int y2, int colour){
	WAIT_FOR_GRAPHICS;
	DrawHorizontalLine(x1, x2, y1, colour);
	DrawHorizontalLine(x1, x2, y2, colour);
	DrawVerticalLine(y1, y2, x1, colour);
	DrawVerticalLine(y1, y2, x2, colour);
}

void DrawFilledRectangle(int x1, int x2, int y1, int y2, int colour){
	WAIT_FOR_GRAPHICS;
	GraphicsX1Reg = x1;
	GraphicsX2Reg = x2;
	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsColourReg = colour;
	GraphicsCommandReg = DrawFilledRect;
}

void DrawBresenhamCircle(int x1, int y1, int radius, int colour){
	WAIT_FOR_GRAPHICS;
	GraphicsX1Reg = x1;
	GraphicsY1Reg = y1;
	GraphicsX2Reg = radius;
	GraphicsColourReg = colour;
	GraphicsCommandReg = DrawCircle;
}

int MapToColour(int r, int g, int b){
	int r2 = ((r + ColourDiff / 2) / ColourDiff) * ColoursInRGB * ColoursInRGB;
	int g2 = ((g + ColourDiff / 2) / ColourDiff) * ColoursInRGB;
	int b2 = ((b + ColourDiff / 2) / ColourDiff);
	return r2 + g2 + b2 + CustomColorIndex;
}

int GetClosetColour(int r, int g, int b){
	Colour c = 0;
	int minDiff = 256 * 3;
	Colour minC = 0;
	for (c = BLACK; c <= WHITE_REPEAT; ++c){
		int currR = ColourPalletteData[c] >> 16;
		int currG = (ColourPalletteData[c] & 0x00FF00) >> 8;
		int currB = ColourPalletteData[c] & 0x0000FF;
		int currDiff = abs(r - currR) + abs(g - currG) + abs(b - currB);
		if (currDiff < minDiff){
			minDiff = currDiff;
			minC = c;
		}
		if (minDiff <= 70){
			return minC;
		}
	}

	return minC;
}

// Draws a map from a 24-bit bitmap where the lower left is at (x,y) and option to scale the image (stretch)
void DrawMap(char *fileName, int x, int y, int length, int width, int scale){
	 // super-simplified BMP read algorithm to pull out RGB data
	 // read image for coloring scheme
	 //int image[400*400][3]; // first number here is 1024 pixels in my image, 3 is for RGB values
	 FILE *streamIn;
	 streamIn = fopen(fileName, "r");
	 if (streamIn == (FILE *)0){
	   perror("File opening error ocurred. Exiting program.\n");
	   exit(0);
	 }

	 int byte;
	 int i;
	 for(i=0; i<54; i++) byte = getc(streamIn);  // strip out BMP header

	 int currX = x;
	 int currY = y;

	 for (i=0; i < length*width; i++){    // foreach pixel
	    int b = fgetc(streamIn);  // use BMP 24bit with no alpha channel
	    int g = fgetc(streamIn);  // BMP uses BGR but we want RGB, grab byte-by-byte
	    int r = fgetc(streamIn);  // reverse-order array indexing fixes RGB issue...

	    Colour c = GetClosetColour(r, g, b);
	    //printf("C: %x, Pixel %d : [%d,%d,%d]\n", ColourPalletteData[c], i+1,r,g,b);

	    if (i % length == 0){
	    	currX = x;
	    	currY -= scale;
	    }
	    else{
	    	currX += scale;
	    }

	    int scaleX;
	    int scaleY;
	    for (scaleY = currY; scaleY > currY - scale; --scaleY){
	    	for (scaleX = currX; scaleX < currX + scale; ++scaleX){
	    		WriteAPixel(scaleX, scaleY, c);
	    	}
	    }
	 }

	 fclose(streamIn);
}

// Same as DrawMap but with a more efficient bitmap decoding implementation
void DrawMap2(char *fileName, int x, int y, int length, int width, int scale){
	 // super-simplified BMP read algorithm to pull out RGB data
	 // read image for coloring scheme
	 //int image[400*400][3]; // first number here is 1024 pixels in my image, 3 is for RGB values
	 FILE *streamIn;
	 streamIn = fopen(fileName, "rb");
	 printf("aaa");
	 if (streamIn == (FILE *)0){
	   perror("File opening error ocurred. Exiting program.\n");
	   exit(0);
	 }

	 int i;

	 unsigned char info[54];
	 for(i=0; i<54; i++) getc(streamIn);  // strip out BMP header

	 int row_padded = (length*3 + 3) & (~3);
	 unsigned char data[width][row_padded];

	 int row, col;
	 for(row = 0; row < width; ++row){
		 fread(data[width - 1 - row], sizeof(unsigned char), row_padded, streamIn);
	 }
	 printf("boom\n");
	 int currX = x;
	 int currY = y;
	 for(row = 0; row < width; ++row){
		 for(col = 0; col < length * 3; col += 3){
			 //printf("Pixel: %d, R:%d, G:%d, B:%d\n", count++, (int)data[row][col], (int)data[row][col + 1], (int)data[row][col + 2]);
			 int c = MapToColour(data[row][col + 2], data[row][col + 1], data[row][col]);

			 int currX2, currY2;
			 for(currY2 = currY; currY2 < currY + scale; ++currY2){
				 for(currX2 = currX; currX2 < currX + scale; ++currX2){
					 WriteAPixel(currX2, currY2, c);
				 }
			 }
			 currX = currX2;
		 }

		 currX = x;
		 currY += scale;
	 }

	 fclose(streamIn);
}

void DrawMapSDCard(char *fileName, int x, int y, int length, int width, int scale){
	printf("DRAWING NOW");
	printf(fileName);
	short int bitmap[length*width * 3 + 54];
	ReadFromFile(fileName, bitmap);

	int currX = x;
	int currY = y;
	int i;

	printf("currX: %d", currX);
	printf("currY: %d", currY);

	for(i = 0; i < length * width * 3; i += 3){
		int c = MapToColour(bitmap[i + 2 + 54], bitmap[i + 1 + 54], bitmap[i + 54]);
		int currX2, currY2;
		for(currY2 = currY; currY2 > currY - scale; --currY2){
			for(currX2 = currX; currX2 < currX + scale; ++currX2){
				WriteAPixel(currX2, currY2, c);
				//printf("currX: %d, currY: %d\n", currX2, currY2);
			}
		}

		currX = currX2;

		if((i % (length * 3)) == 0){
			currX = x;
			currY -= scale;
		}
	}

	printf("\n%d\n", i);
	printf("currX: %d", currX);
	printf("currY: %d", currY);
}

void TestShapes(){
	// draw a line across the screen in RED at y coord 100 and from x = 0 to 799
	//for(i = 0; i < 800; i ++)
		//WriteAPixel(i, 100, RED);

	// read the pixels back and make sure we read 2 (RED) to prove it's working
		//for(i = 0; i < 800; i ++)
			//printf("Colour value (i.e. pallette number) = %d at [%d, 100]\n", ReadAPixel(i, 100), i);
	//DrawFilledRectangle(0, 800, 0, 480, WHITE);

	DrawHorizontalLine(0, 800, 400, CYAN);
	DrawVerticalLine(0, 480, 400, MAGENTA);
	DrawBresenhamLine(0, 400, 0, 300, YELLOW);
	DrawBresenhamLine(0, 400, 300, 0, YELLOW);
	DrawBresenhamLine(600, 300, 0, 300, YELLOW);
	DrawBresenhamLine(600, 300, 300, 0, YELLOW);

	DrawString1(300, 150, RED, WHITE, "abcderfg", 1);
	DrawString2(400, 150, RED, WHITE, "abcderfg", 0);

	DrawRectangle(30, 200, 50, 150, CYAN);

	DrawFilledRectangle(100, 200, 300, 350, 12);

	DrawBresenhamCircle(400, 400, 50, BLUE);
}


/****************************************************************************************************
** subroutine to program a hardware (graphics chip) palette number with an RGB value
** e.g. ProgramPalette(RED, 0x00FF0000) ;
****************************************************************************************************/

void ProgramPalette(int PaletteNumber, int RGB)
{
    WAIT_FOR_GRAPHICS;
    GraphicsColourReg = PaletteNumber;
    GraphicsX1Reg = RGB >> 16   ;          // program red value in ls.8 bit of X1 reg
    GraphicsY1Reg = RGB ;                	 // program green and blue into 16 bit of Y1 reg
    GraphicsCommandReg = ProgramPaletteColour;	// issue command
}

void ProgramAllPalette(){
	int i;
	for(i = 0; i < 256; ++i){
		ProgramPalette(i, ColourPalletteData[i]);
	}
}

void DrawButtons(){
	DrawFilledRectangle(20, 180, 410, 470, WHITE);
	DrawFilledRectangle(220, 380, 410, 470, WHITE);
	DrawFilledRectangle(420, 580, 410, 470, WHITE);
	DrawFilledRectangle(620, 780, 410, 470, WHITE);
	DrawRectangle(20, 180, 410, 470, BLACK);
	DrawRectangle(220, 380, 410, 470, BLACK);
	DrawRectangle(420, 580, 410, 470, BLACK);
	DrawRectangle(620, 780, 410, 470, BLACK);
	DrawString2(25, 440, BLACK, WHITE, "View Trails", FALSE);
	DrawString2(230, 440, BLACK, WHITE, "Save Trail", FALSE);
	DrawString2(480, 440, BLACK, WHITE, "Stop", FALSE);
	DrawString2(660, 440, BLACK, WHITE, "Refresh", FALSE);
}

void DrawButtonPress(int button){
	if (button == 1){
		DrawFilledRectangle(20, 180, 410, 470, BLUE);
		DrawString2(25, 440, BLACK, WHITE, "View Trails", FALSE);
	}
	if (button == 2){
		DrawFilledRectangle(220, 380, 410, 470, BLUE);
		DrawString2(230, 440, BLACK, WHITE, "Save Trail", FALSE);
	}
	if (button == 3){
		DrawFilledRectangle(420, 580, 410, 470, BLUE);
		DrawString2(480, 440, BLACK, WHITE, "Stop", FALSE);
	}
	if (button == 4){
		DrawFilledRectangle(620, 780, 410, 470, BLUE);
		DrawString2(660, 440, BLACK, WHITE, "Refresh", FALSE);
	}
}

void ReleaseButtonPress(int button){
	if (button == 1){
		DrawFilledRectangle(20, 180, 410, 470, WHITE);
		DrawRectangle(20, 180, 410, 470, BLACK);
		DrawString2(25, 440, BLACK, WHITE, "View Trails", FALSE);
	}
	if (button == 2){
		DrawFilledRectangle(220, 380, 410, 470, WHITE);
		DrawRectangle(220, 380, 410, 470, BLACK);
		DrawString2(230, 440, BLACK, WHITE, "Save Trail", FALSE);
	}
	if (button == 3){
		DrawFilledRectangle(420, 580, 410, 470, WHITE);
		DrawRectangle(420, 580, 410, 470, BLACK);
		DrawString2(480, 440, BLACK, WHITE, "Stop", FALSE);
	}
	if (button == 4){
		DrawFilledRectangle(620, 780, 410, 470, WHITE);
		DrawRectangle(620, 780, 410, 470, BLACK);
		DrawString2(660, 440, BLACK, WHITE, "Refresh", FALSE);
	}
}

void ReleaseStopOrPause(int stop){
	DrawFilledRectangle(420, 580, 410, 470, WHITE);
	DrawRectangle(420, 580, 410, 470, BLACK);
	if(stop == 1){
		DrawString2(450, 440, BLACK, WHITE, "Continue", FALSE);
	}
	else{
		DrawString2(480, 440, BLACK, WHITE, "Stop", FALSE);
	}
}

int CheckButtonPress(int x, int y){
	if (x >= 20 && x <= 180 && y >= 410 && y <= 470){
		return 1;
	}
	if (x >= 220 && x <= 380 && y >= 410 && y <= 470){
		return 2;
	}
	if (x >= 420 && x <= 580 && y >= 410 && y <= 470){
		return 3;
	}
	if (x >= 620 && x <= 780 && y >= 410 && y <= 470){
		return 4;
	}

	return -1;
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void AssignGPSData(int map, char* longitude, char *latitude){
	double lo = 0;
	double lat = 0;
	switch(map){
		case 1:
			lat = 49.26467;
			lo = 123.25272;
			break;
		case 2:
			lat = 48.26453;
			lo = 124.25243;
			break;
		case 3:
			lat = 49.26425;
			lo = 123.25223;
			break;
		case 4:
			lat = 49.26406;
			lo = 123.25206;
			break;
		case 5:
			lat = 49.26365;
			lo = 123.25167;
			break;
		case 6:
			lat = 49.26338;
			lo = 123.25147;
			break;
		case 7:
			lat = 49.26324;
			lo = 123.25193;
			break;
		case 8:
			lat = 49.26346;
			lo = 123.25228;
			break;
		case 9:
			lat = 49.26332;
			lo = 123.25267;
			break;
		case 10:
			lat = 49.26306;
			lo = 123.25302;
			break;
		default:
			lat = 49.26278;
			lo = 123.25327;
			break;
	}

	sprintf(latitude, "%.5f", lat);
	sprintf(longitude, "%.5f", lo);

	printf("Lat: %s", latitude);
	printf("Long: %s", longitude);
}

void DrawSavedMaps(int savedMaps[10], MapButton maps[10]){
	int i = 0;

	for(i = 0; i < 10; ++i){
		// reached end of array
		if(savedMaps[i] == 0){
			break;
		}
		// draw the map
		MapButton map;
		map.x = 100;
		map.y = (i * 70 + 20);

		// set the file name
		char *num[2];
		printf("%d", savedMaps[i]);
		sprintf(num, "%d", savedMaps[i]);
		map.mapName = concat("MAP", num);
		map.mapName = concat(map.mapName, ".BMP");

		// Set path data
		map.distance = (savedMaps[i] - 1) * 3;
		map.time = (savedMaps[i] - 1) * 25;

		// Draw the button
		DrawRectangle(map.x, map.x + 200, map.y, map.y + 50, BLACK);
		DrawString2(map.x + 80, map.y + 30, BLACK, WHITE, map.mapName, 0);

		maps[i] = map;
	}

	// Draw separation pane
	DrawVerticalLine(0, 480, 400, BLACK);

	// Draw back button
	DrawRectangle(510, 710, 400, 450, BLACK);
	DrawString2(580, 420, BLACK, WHITE, "Back", 0);
}

void DrawMapData(MapButton map, int mapIndex){
	char *num[2];

	DrawFilledRectangle(420, 800, 320, 380, WHITE);

	sprintf(num, "%d", map.distance);
	char* distance = concat("Distance: ", num);
	distance = concat(distance, " meters");
	DrawString2(450, 340, BLACK, WHITE, distance, 0);

	sprintf(num, "%d", map.time);
	char* time = concat("Time: ", num);
	time = concat(time, " seconds");
	DrawString2(450, 360, BLACK, WHITE, time, 0);

	char* lat = malloc(sizeof(char) * 10);
	char* lo = malloc(sizeof(char) * 10);
	char* gps;
	AssignGPSData(mapIndex, lat, lo);
	gps = concat(lat, "N");
	gps = concat(gps, " ");
	gps = concat(gps, lo);
	gps = concat(gps, "W");
	printf("GPS: %s", gps);
	DrawString2(450, 320, BLACK, WHITE, gps, 0);
}

int CheckMapButtonPress(MapButton maps[10], int x, int y){
	int i;

	// Check back button press
	if(x >= 510 && x <= 710 && y >= 400 && y <= 450){
		return 11;
	}

	for(i = 0; i < 10; ++i){
		MapButton map = maps[i];
		if(x >= map.x && (x <= (map.x + 200)) && y >= map.y && (y <= (map.y + 50))){
			// pressed a button
			printf(map.mapName);
			return i;
		}
	}

	return -1;
}

void DrawMapButtonPress(MapButton map){
	DrawFilledRectangle(map.x, map.x + 200, map.y, map.y + 50, BLUE);
	DrawRectangle(map.x, map.x + 200, map.y, map.y + 50, BLACK);
	DrawString2(map.x + 80, map.y + 30, BLACK, WHITE, map.mapName, 0);
}

void DrawBackButtonPress(){
	DrawFilledRectangle(510, 710, 400, 450, BLUE);
	DrawRectangle(510, 710, 400, 450, BLACK);
	DrawString2(580, 420, BLACK, WHITE, "Back", 0);
}

void ReleaseMapButtonPress(MapButton map){
	// Draw the button
	DrawFilledRectangle(map.x, map.x + 200, map.y, map.y + 50, WHITE);
	DrawRectangle(map.x, map.x + 200, map.y, map.y + 50, BLACK);
	DrawString2(map.x + 80, map.y + 30, BLACK, WHITE, map.mapName, 0);
}

void DrawStar(int x, int y, int length, int colour){
	DrawHorizontalLine(x, x + length, y, colour);
	DrawBresenhamLine(x + length, (x + length * 1.5), y, y - length, colour);
	DrawBresenhamLine(x + length * 1.5, x + 2*length, y - length, y, colour);
	DrawHorizontalLine(x + 2*length, x + 3*length, y, colour);
	DrawBresenhamLine(x + 3*length, x + 2.2 * length, y, y + length * 0.7, colour);
	DrawBresenhamLine(x, x + 0.8 * length, y, y + length * 0.7, colour);
	DrawBresenhamLine(x + 2.2 * length, x + 2.4 * length, y + length * 0.7, y + length * 1.7, colour);
	DrawBresenhamLine(x + 0.8 * length, x + 0.6 * length, y + length * 0.7, y + length * 1.7, colour);
	DrawBresenhamLine(x + 2.4 * length, x + 1.5 * length, y + length * 1.7, y + length * 1.1, colour);
	DrawBresenhamLine(x + 0.6 * length, x + 1.5 * length, y + length * 1.7, y + length * 1.1, colour);
}

void DrawRatings(int n, int colour){
	int i = 0;
	for(i = 0; i < n; ++i){
		DrawStar(150 + 100 * i, 200, 30, colour);
	}
}

int CheckRatingPress(int x, int y){
	if(y < 200 || y > 280)
		return -1;

	int res = (x - 150) / 100;
	if(res < 0 || res > 4){
		return - 1;
	}

	return res;
}

void ResetScreen(){
	DrawFilledRectangle(0, XRES, 0, YRES, WHITE);
}

// Checks the input buttons array to see if the input x,y touches one of them
// Return the index of the button that it presses. -1 otherwise.
int CheckSavedMapButtonPress(SavedMapButton** buttons, size_t num_maps, int x, int y){
	int i;
	for(i = 0; i < num_maps; ++i){
		if(!(x >= buttons[i]->x && x <= buttons[i]->x + BUTTON_WIDTH))
			continue;
		if(!(y >= buttons[i]->y && y <= buttons[i]->y + BUTTON_HEIGHT))
			continue;

		return i;
	}

	return -1;
}

// Draw the given button onto the screen as a rectangle, with its name as the label
void DrawSavedMapButton(SavedMapButton* button){
	DrawRectangle(button->x, button->x + BUTTON_WIDTH, button->y, button->y + BUTTON_HEIGHT, BLACK);
	DrawString2CenterRange(button->x, button->x + BUTTON_WIDTH, button->y + BUTTON_HEIGHT/2, BLACK, WHITE, button->name, 0);
}

// Refreshes all buttons (to remove previous highlight) and highlights the selected button
void HighlightSavedMapButton(SavedMapButton** map, SavedMapButton* button, size_t num_maps){
	DrawAllSavedMapButtons(map, num_maps);
	DrawRectangle(button->x, button->x + BUTTON_WIDTH, button->y, button->y + BUTTON_HEIGHT, YELLOW);
	DrawString2CenterRange(button->x, button->x + BUTTON_WIDTH, button->y + BUTTON_HEIGHT/2, BLACK, WHITE, button->name, 0);
}

// Draws every button in the map array
void DrawAllSavedMapButtons(SavedMapButton** map, size_t num_maps){
	int i = 0;
	while(i < num_maps){
		DrawSavedMapButton(map[i++]);
	}
}

// Draws the data of the map on the right half of the screen
void DrawSavedMapData(SavedMapButton* button){
	char buffer[50];

	DrawFilledRectangle(XRES / 2, XRES, 0, YRES, WHITE);
	DrawString2CenterRange(XRES / 2, XRES, 150, BLACK, WHITE, button->name, 0);
	sprintf(buffer, "Rating: %s Stars!", button->rating);
	DrawString2CenterRange(XRES / 2, XRES, 175, BLACK, WHITE, buffer, 0);
	sprintf(buffer, "Distance: %s Meters", button->distance);
	DrawString2CenterRange(XRES / 2, XRES, 200, BLACK, WHITE, buffer, 0);
	sprintf(buffer, "Duration: %s Seconds", button->duration);
	DrawString2CenterRange(XRES / 2, XRES, 225, BLACK, WHITE, buffer, 0);
	sprintf(buffer, "Locations: %s", button->locations);
	DrawString2CenterRange(XRES / 2, XRES, 250, BLACK, WHITE, buffer, 0);
	sprintf(buffer, "Date: %s", button->date);
	DrawString2CenterRange(XRES / 2, XRES, 275, BLACK, WHITE, buffer, 0);
}

void SetMockedMapData(SavedMapButton** maps){
	int i;
	for(i = 0; i < 5; ++i){
		SavedMapButton *map = malloc(sizeof(SavedMapButton));
		map->date = "Date";
		map->distance = "10";
		map->duration = "5";
		map->locations = "loc";
		map->name = "ABCD";
		map->rating = "4";
		map->x = 50;
		map->y = 20 + i * (BUTTON_HEIGHT * 1.2);
		maps[i] = map;
	}
}
