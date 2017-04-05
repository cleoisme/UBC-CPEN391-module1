#include <stdio.h>
#include "altera_up_avalon_character_lcd.h"

alt_up_character_lcd_dev * char_lcd_dev;

void InitializeLCD() {

	char_lcd_dev = alt_up_character_lcd_open_dev("/dev/character_lcd_0");
	if ( char_lcd_dev == NULL)
		printf("Error: could not open character LCD device\n");
	else
		printf("Opened character LCD device\n");

	alt_up_character_lcd_init(char_lcd_dev);
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0,0);
	alt_up_character_lcd_string(char_lcd_dev, "Hiking Pal App");

}

void WriteStringToLCD(char* str){
	alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
	alt_up_character_lcd_string(char_lcd_dev, str);
}

void WriteToLCD(char* latitude, char* longitude, char lat_direction, char lon_direction){
   alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, 1);
   char * location_str = "";
   sprintf(location_str,"%.6s%c,%.7s%c",latitude, lat_direction, longitude, lon_direction);
   alt_up_character_lcd_string(char_lcd_dev, location_str);
}
