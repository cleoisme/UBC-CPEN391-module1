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
#include <stdint.h>
#include "altera_up_avalon_character_lcd.h"
#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define keys (volatile char *) 0x0002000
#define keys_off 15

#define GPSCHIP_Control (*(volatile unsigned char *)(0x84000210))
#define GPSCHIP_Status (*(volatile unsigned char *)(0x84000210))
#define GPSCHIP_TxData (*(volatile unsigned char *)(0x84000212))
#define GPSCHIP_RxData (*(volatile unsigned char *)(0x84000212))
#define GPSCHIP_Baud (*(volatile unsigned char *)(0x84000214))

#define MAXLINELENGTH 120

volatile char line1[MAXLINELENGTH];
volatile char line2[MAXLINELENGTH];

volatile int lineidx = 0;
volatile char *currentline;
volatile char *lastline;
volatile int receivedFlag;
volatile int inStandbyMode;
volatile int paused;

char* lastNMEA(void);
int newNMEAreceived(void);

void sendCommand(const char *);


int waitForSentence(const char *wait);
int parseData(char *response);
int parseHex(char c);


int wakeup(void);
int standby(void);

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ  "$PMTK220,10000*2F" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ  "$PMTK220,5000*1B"  // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ  "$PMTK220,1000*1F"
#define PMTK_SET_NMEA_UPDATE_5HZ  "$PMTK220,200*2C"
#define PMTK_SET_NMEA_UPDATE_10HZ "$PMTK220,100*2F"
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ  "$PMTK300,10000,0,0,0,0*2C" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ  "$PMTK300,5000,0,0,0,0*18"  // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ  "$PMTK300,1000,0,0,0,0*1C"
#define PMTK_API_SET_FIX_CTL_5HZ  "$PMTK300,200,0,0,0,0*2F"
// Can't fix position faster than 5 times a second!

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

// to generate your own sentences, check out the MTK command datasheet and use a checksum calculator
// such as the awesome http://www.hhhh.org/wiml/proj/nmeaxor.html

#define PMTK_LOCUS_STARTLOG  "$PMTK185,0*22"
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"
#define LOCUS_OVERLAP 0
#define LOCUS_FULLSTOP 1

#define PMTK_ENABLE_SBAS "$PMTK313,1*2E"
#define PMTK_ENABLE_WAAS "$PMTK301,2*2E"

// standby command & boot successful message
#define PMTK_STANDBY "$PMTK161,0*28"
#define PMTK_STANDBY_SUCCESS "$PMTK001,161,3*36"  // Not needed currently
#define PMTK_AWAKE "$PMTK010,002*2D"

// ask for the release and version
#define PMTK_Q_RELEASE "$PMTK605*31"

// request for updates on antenna status
#define PGCMD_ANTENNA "$PGCMD,33,1*6C"
#define PGCMD_NOANTENNA "$PGCMD,33,0*6D"

// how long to wait when we're looking for a response
#define MAXWAITSENTENCE 5

int hour, minute, seconds, year, month, day;
int milliseconds;
// Floating point latitude and longitude value in degrees.
float latitude, longitude;
// Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
// and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
//   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
int32_t latitude_fixed, longitude_fixed;
float latitudeDegrees, longitudeDegrees;
float geoidheight, altitude;
float speed, angle, magvariation, HDOP;
char lat, lon, mag;
volatile int fix;
int fixquality, satellites;
int LOCUS_serial, LOCUS_records;
int LOCUS_type, LOCUS_mode, LOCUS_config, LOCUS_interval, LOCUS_distance, LOCUS_speed, LOCUS_status, LOCUS_percent;




void Init_GPSCHIP(void)
{
 // set up 6850 Control Register to utilise a divide by 16 clock,
 // set RTS low, use 8 bits of data, no parity, 1 stop bit,
 // transmitter interrupt disabled
 // program baud rate generator to use 115k baud

	//The 6850 Control Register write only
	// RS232_Control(7 DOWNTO 0) = |X|1|0|1|0|1|0|1| = 0b01010101 = 0x55 rts high & interrupt disable
	// RS232_Control(7 DOWNTO 0) = |X|0|0|1|0|1|0|1| = 0b00010101 = 0x15
	GPSCHIP_Control = 0x15;
	GPSCHIP_Baud = 0x05; // program for 9600 baud
	printf("Initializing GPS CHIP");
	receivedFlag = 0;
	paused = 0;
	lineidx = 0;
	currentline = line1;
	lastline = line2;

	hour = minute = seconds = year = month = day =
	fixquality = satellites = 0; // int
	lat = lon = mag = 0; // char
	fix = 0; // boolean
	milliseconds = 0; // int
	latitude = longitude = geoidheight = altitude =
	speed = angle = magvariation = HDOP = 0.0; // float
	usleep(10000);
}


int parseData(char *data) {

	if(data[strlen(data) - 4] == '*'){
		int sum = parseHex(data[strlen(data) -3])*16;
		sum += parseHex(data[strlen(data) - 2]);

		//check checksum
		int i;
		int length = strlen(data) -4;
		for(i = 2; i < length; i++) {
			sum ^= data[i];
		}

		if(sum != 0) {
			return 0;
		}

	}

	int32_t degree;
	long minutes;
	char degreebuff[10];

	//GGA format
	if(strstr(data, "$GPGGA")) {
		char *p = data;
		p = strchr(p, ',') + 1;
		float timef = atof(p);
		int time = timef;
		hour = time/10000;
		minute = (time % 10000) / 100;
		seconds = (time % 100);

		milliseconds = fmod(timef, 1.0)*1000;

		//latitude

		p = strchr(p, ',') + 1;
		if(',' != *p)
		{
			strncpy(degreebuff, p ,2);
			p +=2;
			degreebuff[2] = '\0';
			degree = atol(degreebuff)*10000000;
			strncpy(degreebuff, p, 2);
			p += 3;
			strncpy(degreebuff + 2, p , 4);
			degreebuff[6] = '\0';
			minutes = 50*atol(degreebuff) / 3;
			latitude_fixed = degree + minutes;
			latitude = degree/100000 + minutes*0.000006F;
			latitudeDegrees = (latitude-100*(latitude/100))/60.0;
			latitudeDegrees += (latitude/100);
		}

	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      if (p[0] == 'S') latitudeDegrees *= -1.0;
	      if (p[0] == 'N') lat = 'N';
	      else if (p[0] == 'S') lat = 'S';
	      else if (p[0] == ',') lat = 0;
	      else return 0;
	    }

	    p = strchr(p, ',')+1;

	    if (',' != *p)
	    {
	       strncpy(degreebuff, p, 3);
	       p += 3;
	       degreebuff[3] = '\0';
	       degree = atol(degreebuff) * 10000000;
	       strncpy(degreebuff, p, 2); // minutes
	       p += 3; // skip decimal point
	       strncpy(degreebuff + 2, p, 4);
	       degreebuff[6] = '\0';
	       minutes = 50 * atol(degreebuff) / 3;
	       longitude_fixed = degree + minutes;
	       longitude = degree / 100000 + minutes * 0.000006F;
	       longitudeDegrees = (longitude-100*(longitude/100))/60.0;
	       longitudeDegrees += (longitude/100);
	    }

	    p = strchr(p, ',')+1;
	      if (',' != *p)
	      {
	        if (p[0] == 'W') longitudeDegrees *= -1.0;
	        if (p[0] == 'W') lon = 'W';
	        else if (p[0] == 'E') lon = 'E';
	        else if (p[0] == ',') lon = 0;
	        else return 0;
	      }

	      p = strchr(p, ',')+1;
	         if (',' != *p)
	         {
	           fixquality = atoi(p);
	         }

	         p = strchr(p, ',')+1;
	         if (',' != *p)
	         {
	           satellites = atoi(p);
	         }

	         p = strchr(p, ',')+1;
	         if (',' != *p)
	         {
	           HDOP = atof(p);
	         }

	         p = strchr(p, ',')+1;
	         if (',' != *p)
	         {
	           altitude = atof(p);
	         }

	         p = strchr(p, ',')+1;
	         p = strchr(p, ',')+1;
	         if (',' != *p)
	         {
	           geoidheight = atof(p);
	         }
	         return 1;
	}


	if (strstr(data, "$GPRMC")) {
	   // found RMC
	    char *p = data;

	    // get time
	    p = strchr(p, ',')+1;
	    float timef = atof(p);
	    int time = timef;
	    hour = time / 10000;
	    minute = (time % 10000) / 100;
	    seconds = (time % 100);

	    milliseconds = fmod(timef, 1.0) * 1000;

	    p = strchr(p, ',')+1;
	    // Serial.println(p);
	    if (p[0] == 'A')
	      fix = 1;
	    else if (p[0] == 'V')
	      fix = 0;
	    else
	      return 0;

	    // parse out latitude
	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      strncpy(degreebuff, p, 2);
	      p += 2;
	      degreebuff[2] = '\0';
	      long degree = atol(degreebuff) * 10000000;
	      strncpy(degreebuff, p, 2); // minutes
	      p += 3; // skip decimal point
	      strncpy(degreebuff + 2, p, 4);
	      degreebuff[6] = '\0';
	      long minutes = 50 * atol(degreebuff) / 3;
	      latitude_fixed = degree + minutes;
	      latitude = degree / 100000 + minutes * 0.000006F;
	      latitudeDegrees = (latitude-100*(latitude/100))/60.0;
	      latitudeDegrees += (latitude/100);
	    }

	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      if (p[0] == 'S') latitudeDegrees *= -1.0;
	      if (p[0] == 'N') lat = 'N';
	      else if (p[0] == 'S') lat = 'S';
	      else if (p[0] == ',') lat = 0;
	      else return 0;
	    }

	    // parse out longitude
	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      strncpy(degreebuff, p, 3);
	      p += 3;
	      degreebuff[3] = '\0';
	      degree = atol(degreebuff) * 10000000;
	      strncpy(degreebuff, p, 2); // minutes
	      p += 3; // skip decimal point
	      strncpy(degreebuff + 2, p, 4);
	      degreebuff[6] = '\0';
	      minutes = 50 * atol(degreebuff) / 3;
	      longitude_fixed = degree + minutes;
	      longitude = degree / 100000 + minutes * 0.000006F;
	      longitudeDegrees = (longitude-100*(longitude/100))/60.0;
	      longitudeDegrees += (longitude/100);
	    }

	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      if (p[0] == 'W') longitudeDegrees *= -1.0;
	      if (p[0] == 'W') lon = 'W';
	      else if (p[0] == 'E') lon = 'E';
	      else if (p[0] == ',') lon = 0;
	      else return 0;
	    }
	    // speed
	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      speed = atof(p);
	    }

	    // angle
	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      angle = atof(p);
	    }

	    p = strchr(p, ',')+1;
	    if (',' != *p)
	    {
	      int fulldate = atof(p);
	      day = fulldate / 10000;
	      month = (fulldate % 10000) / 100;
	      year = (fulldate % 100);
	    }
	    // we dont parse the remaining, yet!
	    return 1;
	  }

	return 0;
}


int parseHex(char c){

	if(c < '0')
		return 0;
	if(c <= '9')
	{
		int i = c - '0';
		return i;
	}
	if(c < 'A')
		return 0;
	if(c <= 'F') {
		int i = c - 'A';
		int j = i + 10;
		return j;

	}
	return 0;

}


int waitForSentence(const char *data) {
	  char str[20];

	  int i=0;
	  while (i < MAXLINELENGTH) {
	    if (newNMEAreceived() == 1) {
	      char *nmea = lastNMEA();
	      strncpy(str, nmea, 20);
	      str[19] = 0;
	      i++;

	      if (strstr(str, data))
		return 1;
	    }
	  }

	  return 0;
}

int newNMEAreceived(void) {
  return receivedFlag;
}



char* lastNMEA(void) {
  receivedFlag = 0;
  return (char *)lastline;
}

int LOCUS_StartLogger(void) {
  sendCommand(PMTK_LOCUS_STARTLOG);
  receivedFlag = 0;
  return waitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

int LOCUS_StopLogger(void) {
  sendCommand(PMTK_LOCUS_STOPLOG);
  receivedFlag = 0;
  return waitForSentence(PMTK_LOCUS_STARTSTOPACK);
}

int LOCUS_ReadStatus(void) {
  sendCommand(PMTK_LOCUS_QUERY_STATUS);

  if (! waitForSentence("$PMTKLOG"))
    return 0;

  char *response = lastNMEA();
  int parsed[10];
  int i;

  for (i=0; i<10; i++) parsed[i] = -1;

  response = strchr(response, ',');
  for (i=0; i<10; i++) {
    if (!response || (response[0] == 0) || (response[0] == '*'))
      break;
    response++;
    parsed[i]=0;
    while ((response[0] != ',') &&
	   (response[0] != '*') && (response[0] != 0)) {
      parsed[i] *= 10;
      int c = response[0];
      if (isdigit(c))
        parsed[i] += c - '0';
      else
        parsed[i] = c;
      response++;
    }
  }
  LOCUS_serial = parsed[0];
  LOCUS_type = parsed[1];
  if (isalpha(parsed[2])) {
    parsed[2] = parsed[2] - 'a' + 10;
  }
  LOCUS_mode = parsed[2];
  LOCUS_config = parsed[3];
  LOCUS_interval = parsed[4];
  LOCUS_distance = parsed[5];
  LOCUS_speed = parsed[6];
  LOCUS_status = !parsed[7];
  LOCUS_records = parsed[8];
  LOCUS_percent = parsed[9];

  return 1;
}


int standby(void) {
  if (inStandbyMode == 1) {
    return 0;  // Returns false if already in standby mode, so that you do not wake it up by sending commands to GPS
  }
  else {
    inStandbyMode = 1;
    sendCommand(PMTK_STANDBY);
    //return waitForSentence(PMTK_STANDBY_SUCCESS);  // don't seem to be fast enough to catch the message, or something else just is not working
    return 1;
  }
}

int wakeup(void) {
  if (inStandbyMode) {
   inStandbyMode = 0;
    sendCommand("");  // send byte to wake it up
    return waitForSentence(PMTK_AWAKE);
  }
  else {
      return 0;  // Returns false if not in standby mode, nothing to wakeup
  }
}




char *FloatToLatitudeConversion(int x) //output format is xx.yyyy
{
	static char buff[100] ;
	float *ptr = (float *)(&x) ; // cast int to float
	float f = *ptr ; // get the float
	sprintf(buff, "%2.4f", f); // write in string to an array
	return buff ;
}
char *FloatToLongitudeConversion(int x) // output format is (-)xxx.yyyy
{
	static char buff[100] ;
	float *ptr = (float *)(&x) ;
	float f = *ptr ;
	sprintf(buff, "%3.4f", f);
	return buff ;
}


char putcharGPSCHIP(char c)
{
// poll Tx bit in 6850 status register. Wait for it to become '1'

// write 'c' to the 6850 TxData register to output the character

	while (!(GPSCHIP_Status & 0x2));
	GPSCHIP_TxData = c;

	return c;
}

void sendCommand(const char *str){

	int length = sizeof(str);

	int i = 0;
		for(i = 0; i < length; i++){
			usleep(100000); //wait for 100ms
			putcharGPSCHIP(str[i]);
		}
}



char getcharGPSCHIP( void )
{
 // poll Rx bit in 6850 status register. Wait for it to become '1'
 // read received character from 6850 RxData register.
	while (!(GPSCHIP_Status & 0x1));

	if(GPSCHIP_RxData == '\n') {

		currentline[lineidx] = 0;

		if(currentline == line1) {
			currentline = line2;
			lastline = line1;
		} else {
			currentline = line1;
			lastline = line2;
		}

		lineidx = 0;
		receivedFlag = 1;

	}

	currentline[lineidx++] = GPSCHIP_RxData;
	if(lineidx >= MAXLINELENGTH)
		lineidx = MAXLINELENGTH - 1;


	return GPSCHIP_RxData;
}

// the following function polls the 6850 to determine if any character
// has been received. It doesn't wait for one, or read it, it simply tests
// to see if one is available to read
int GPSCHIPTestForReceivedData(void)
{
 // Test Rx bit in 6850 serial comms chip status register
 // if RX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit0: 0000 0001
	return (GPSCHIP_Status & 0x1);
}

int GPSCHIPTestForTransmitData(void) {
	// Test Tx bit in 6850 serial communications chip status register
	// if TX bit is set, return TRUE, otherwise return FALSE

	// RS232_Status: XXXX XXXX
	// We want bit1: 0000 0010
	return (GPSCHIP_Status & 0x2);
}

int main() {
	printf("Test GPSCHIP\n");
	Init_GPSCHIP();

	while(1){
		getcharGPSCHIP();

		if(newNMEAreceived() == 1) {
			if(parseData(lastNMEA())
					== 1){
				printf("Time:" );
				printf("Hour: %d", hour );
				printf("Minute: %d", minute );
				printf("milliseconds: %d", milliseconds );
				printf("Date: ");
				printf("Day: %d", day);
				printf("Month: %d", month);
				printf("Year: %d", year);
				printf("Latitude: %.2f", latitude);
				printf("Longitude: %.2f", longitude);
			}
		}

	}
//	printf("Hello World!\n");
//
//	  return 0;

}
