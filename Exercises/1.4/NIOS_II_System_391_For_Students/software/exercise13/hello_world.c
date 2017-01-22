#include <stdio.h>

#define RS232_Control (*(volatile unsigned char *)(0x84000200))
#define RS232_Status (*(volatile unsigned char *)(0x84000200))
#define RS232_TxData (*(volatile unsigned char *)(0x84000202))
#define RS232_RxData (*(volatile unsigned char *)(0x84000202))
#define RS232_Baud (*(volatile unsigned char *)(0x84000204))

/**************************************************************************
/* Subroutine to initialise the RS232 Port by writing some data
** to the internal registers.
** Call this function at the start of the program before you attempt
** to read or write to data via the RS232 port
**
** Refer to 6850 data sheet for details of registers and
***************************************************************************/
void Init_RS232(void)
{
 // set up 6850 Control Register to utilise a divide by 16 clock,
 // set RTS low, use 8 bits of data, no parity, 1 stop bit,
 // transmitter interrupt disabled
 // program baud rate generator to use 115k baud

	//The 6850 Control Register write only
	// RS232_Control(7 DOWNTO 0) = |X|1|0|1|0|1|0|1| = 0b01010101 = 0x55 rts high & interrupt disable
	// RS232_Control(7 DOWNTO 0) = |X|0|0|1|0|1|0|1| = 0b00010101 = 0x15
	RS232_Control = 0x15;
	RS232_Baud = 0x01; // program for 115k baud

}
int putcharRS232(int c)
{
// poll Tx bit in 6850 status register. Wait for it to become '1'

// write 'c' to the 6850 TxData register to output the character

	while (!(RS232_Status & 0x2));
	RS232_TxData = c;
}

int getcharRS232( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 // read received character from 6850 RxData register.
	while (!(RS232_Status & 0x1));
	return RS232_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int RS232TestForReceivedData(void)
{
 // Test Rx bit in 6850 serial comms chip status register
 // if RX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit0: 0000 0001
	return (RS232_Status & 0x1);
}

int RS232TestForTransmitData(void) {
	// Test Tx bit in 6850 serial communications chip status register
	// if TX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit1: 0000 0010
	return (RS232_Status & 0x2);
}

int main() {
	printf("Test RS232\n");
	Init_RS232();


//	printf("Hello World!\n");
//
//	  return 0;

}
