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
#include <unistd.h>

#define BLUETOOTH_Control (*(volatile unsigned char *)(0x84000220))
#define BLUETOOTH_Status (*(volatile unsigned char *)(0x84000220))
#define BLUETOOTH_TxData (*(volatile unsigned char *)(0x84000222))
#define BLUETOOTH_RxData (*(volatile unsigned char *)(0x84000222))
#define BLUETOOTH_Baud (*(volatile unsigned char *)(0x84000224))

/*
 * Set up 6850 Control register to utilise a divided by 16 clock
 * Slave mode
 * Serial port: Baud = 115 kbps use 8 bits of data, no parity, one stop bit
 * Serial port flow control disabled
 */
void init_Bluetooth(void){
	printf("Start initialising the bluetooth...");
	BLUETOOTH_Control = 0x15;
	BLUETOOTH_Baud = 0x01;
	usleep(100000); //wait for 100ms
	printf("Done initialisation");
	printf("\n");
}

/*
 * Poll Tx bit in 6850 status register. Wait for it to become "1", and then
 * write c to the 6850 TxData register to output the character
 */
//char putChar_Bluetooth(char c){
//	while(1){
//		if(BLUETOOTH_Status & 0x2){
//			BLUETOOTH_TxData = c;
//			break;
//		}
//	}
//	return c;
//}
char putChar_Bluetooth(char c){
	while(!(BLUETOOTH_Status & 0x2))
		BLUETOOTH_TxData = c;

	return c;
}

/*
 * Poll Rx bit in 6850 status register. Wait for it to become "1", and then
 * read received character from 6850 RxData register
 */
char getChar_Bluetooth(void){
	while(1){
		if(BLUETOOTH_Status & 0x1){
			break;
		}
	}
	return BLUETOOTH_RxData;
}


/*
 * Send the command to the bluetooth dongle through the serial port.
 */
void send_command(char string[], int length){
	printf("Start sending the commands...");
	int i = 0;
	for(; i < length; i++){
		usleep(100000); //wait for 100ms
		putChar_Bluetooth(string[i]);
	}
	printf("Commands sent");
	printf("\n");
}

/*
 * Enter the command mode
 */
void start_command(){
	usleep(100000); //wait for 100ms
	send_command("$$$", 3); //Enter the command mode
	usleep(100000); //wait for 100ms
	printf("Start the command mode here");
	printf("\n");
}

/*
 * Quit the command mode
 */
void end_command(){
	usleep(100000); //wait for 100ms
	send_command("---\r\n", 5); //Exit the command mode
	usleep(100000); //wait for 100ms
	printf("Exit the command mode here");
	printf("\n");
}

/*
 * Reset the bluetooth dongle
 */
void dongle_reset(){
	printf("Start reseting the dongle...");
	start_command();
	send_command("SF,", 3);
	send_command("1\r\n", 3);
	end_command();
	printf("Done reset");
	printf("\n");
}

int main()
{
  printf("Hello from Nios II!\n");

  const char name[] = "2016W-CPEN391-G9";
  const char password[] = "PaulDavies";

  init_Bluetooth();

  printf("Start setting up the Bluetooth name: '%s'", name);
  start_command();
  send_command("SN,", 3);
  send_command(name, 16);
  send_command("\r\n", 2);
  printf("Done with the name setup...");

  printf("Start setting up the Bluetooth password: '%s'", password);
  send_command("SP,", 3);
  send_command(password, 10);
  send_command("\r\n", 2);
  end_command();
  printf("Done with the password setup");


  return 0;
}
