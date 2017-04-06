/*
 * keyboard.c
 *
 *  Created on: Apr 5, 2017
 *      Author: Anthony
 */


#include "keyboard.h"
#include "graphics.h"
#include "Colours.h"

const char Row1[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
const char Row2[] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
const char Row3[] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'B'};
const char Row4[] = {'z', 'x', 'c', 'v', 'b', 'n', 'm', ' ', '.', 'E'};

void DrawKeyboard(){
	char str[3];
	str[1] = '\0';
	int i;
	for(i = 0; i < sizeof(Row1) / sizeof(Row1[0]); ++i){
		str[0] = Row1[i];
		DrawRectangle(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 280, 280 + KEY_BUTTON_HEIGHT, BLACK);
		DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 305, BLACK, WHITE, str, 0);
	}
	for(i = 0; i < sizeof(Row2) / sizeof(Row2[0]); ++i){
		str[0] = Row2[i];
		DrawRectangle(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 330, 330 + KEY_BUTTON_HEIGHT, BLACK);
		DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 355, BLACK, WHITE, str, 0);
	}
	for(i = 0; i < sizeof(Row3) / sizeof(Row3[0]); ++i){
		str[0] = Row3[i];
		DrawRectangle(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 380, 380 + KEY_BUTTON_HEIGHT, BLACK);
		if(Row3[i] != 'B'){
			DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 405, BLACK, WHITE, str, 0);
		}
		else{
			DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 405, BLACK, WHITE, "<-", 0);
		}
	}
	for(i = 0; i < sizeof(Row4) / sizeof(Row4[0]); ++i){
		str[0] = Row4[i];
		DrawRectangle(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 430, 430 + KEY_BUTTON_HEIGHT, BLACK);
		if(Row4[i] != 'E'){
			DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 455, BLACK, WHITE, str, 0);
			}
		else{
			DrawString2CenterRange(i * KEY_BUTTON_WIDTH, (i + 1) * KEY_BUTTON_WIDTH, 455, BLACK, WHITE, "->", 0);
		}
	}
}

char GetCharPress(int x, int y){
	if(y < INIT_Y){
		return '\0';
	}

	int row = (y - INIT_Y) / KEY_BUTTON_HEIGHT;
	int col = x / KEY_BUTTON_WIDTH;

	if(row == 0){
		return Row1[col];
	}
	else if (row == 1){
		return Row2[col];
	}
	else if (row == 2){
		return Row3[col];
	}
	else{
		return Row4[col];
	}
}
