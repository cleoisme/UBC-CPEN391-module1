/*
 * bluetooth2.h
 *
 *  Created on: Mar 13, 2017
 *      Author: Anthony
 */

#ifndef BLUETOOTH2_H_
#define BLUETOOTH2_H_

int setup_all_bluetooth2();
void send_string(char string[], int length);
char getchar_poll(void);
void set_user_pass();
void init_btport();
void receive_string(char buffer[], int maxlen);

#endif /* BLUETOOTH2_H_ */
