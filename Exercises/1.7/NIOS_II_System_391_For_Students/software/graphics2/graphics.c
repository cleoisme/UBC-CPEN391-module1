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
	GraphicsColourReg = Colour;
	GraphicsCommandReg = DrawHLine;
}

void DrawVerticalLine(int y1, int y2, int x, int Colour)
{
	WAIT_FOR_GRAPHICS;

	GraphicsY1Reg = y1;
	GraphicsY2Reg = y2;
	GraphicsX1Reg = x;
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


int main()
{
	int i ;
	printf("Hello from Nios II!\n");

	// draw a line across the screen in RED at y coord 100 and from x = 0 to 799
	//for(i = 0; i < 800; i ++)
		//WriteAPixel(i, 100, RED);

	DrawHorizontalLine(0, 800, 400, CYAN);
	DrawVerticalLine(0, 480, 400, MAGENTA);
	DrawBresenhamLine(0, 400, 0, 300, YELLOW);
	DrawBresenhamLine(0, 400, 300, 0, YELLOW);
	DrawBresenhamLine(600, 300, 0, 300, YELLOW);
	DrawBresenhamLine(600, 300, 300, 0, YELLOW);

	// read the pixels back and make sure we read 2 (RED) to prove it's working
	for(i = 0; i < 800; i ++)
		printf("Colour value (i.e. pallette number) = %d at [%d, 100]\n", ReadAPixel(i, 100), i);

	return 0;
}
