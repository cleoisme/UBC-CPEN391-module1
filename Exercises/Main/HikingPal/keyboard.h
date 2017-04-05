/*
 * keyboard.h
 *
 *  Created on: Apr 5, 2017
 *      Author: Anthony
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define KEY_BUTTON_WIDTH 80
#define KEY_BUTTON_HEIGHT 50

#define INIT_Y 280

void DrawKeyboard();
char GetCharPress(int x, int y);

#endif /* KEYBOARD_H_ */
