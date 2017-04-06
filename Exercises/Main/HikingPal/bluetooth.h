/*
 * bluetooth2.h
 *
 *  Created on: Mar 13, 2017
 *      Author: Anthony
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

int setup_all_bluetooth2();
void send_string(char string[], int length);
char getchar_poll(void);
void set_user_pass(void);
void init_btport();
void receive_string(char buffer[], int maxlen);
int test_getchar(void);
void removeBtBuffer(void);
char getchar_btport(void);

#endif /* BLUETOOTH2_H_ */
