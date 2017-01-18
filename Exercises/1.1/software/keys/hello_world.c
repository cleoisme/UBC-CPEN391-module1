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
#include "altera_up_avalon_character_lcd.h"
#include "io.h"

#define keys (volatile char *) 0x0002000
#define keys_off 15

int main()
{
	alt_up_character_lcd_dev * char_lcd_dev;
	// open the Character LCD port
	char_lcd_dev = alt_up_character_lcd_open_dev ("/dev/character_lcd_0");
	if ( char_lcd_dev == NULL)
		printf ("Error: could not open character LCD device\n");
	else
		printf ("Opened character LCD device\n");
	/* Initialize the character display */
	alt_up_character_lcd_init (char_lcd_dev);

	while(1){
		int data = IORD_8DIRECT(keys, 0);
		if (data == keys_off){
			continue;
		}

		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 0);

		int i;
		for (i = 0; i < 4; ++i){
			if ((data & (1 << i)) == 0){
				char str[0];
				sprintf(str, "%d", i);
				alt_up_character_lcd_string(char_lcd_dev, str);
				break;
			}
		}
	}

  return 0;
}
