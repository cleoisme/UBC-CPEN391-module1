#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define WifiDongle_Control (*(volatile unsigned char *)(0x84000240))
#define WifiDongle_Status (*(volatile unsigned char *)(0x84000240))
#define WifiDongle_TxData (*(volatile unsigned char *)(0x84000242))
#define WifiDongle_RxData (*(volatile unsigned char *)(0x84000242))
#define WifiDongle_Baud (*(volatile unsigned char *)(0x84000244))

const char* DOFILE = "dofile(\"send_text_message.lua\")\n";
const char* RUN = "check_wifi()\n";

const char* start = "$$$";
const char* end = "\r\n";

void init(){
	//set baud rate to 115k
	WifiDongle_Baud = 0x01;
	WifiDongle_Control = 0x15;
	printf("init done\n");
}

int putcharWifi(int c)
{
	while(1){
		if(WifiDongle_Status & 0x02){
			WifiDongle_TxData= c;
			break;
		}
	}
	 return c ;
}

int getcharWifi( void )
{
	while(1){
		if(WifiDongle_Status & 0x01){
			return WifiDongle_RxData;
			break;
		}
	}
}


void getCommand(char* c, int length){
	int i;
	usleep(1000000);
	for(i=0;i<strlen(start);i++){
		putcharWifi((int)start[i]);
	}
	usleep(1000000);
	for(i=0;i<strlen(end);i++){
		putcharWifi((int)end[i]);
	}
	usleep(1000000);
	for(i=0;i<strlen(end);i++){
		putcharWifi((int)end[i]);
	}
	usleep(1000000);
	for(i=0;i<strlen(end);i++){
		putcharWifi((int)end[i]);
	}
	usleep(1000000);
	for(i=0;i<length;i++){
		putcharWifi((int)c[i]);
	}
}

int main()
{
  printf("start wifi test\n");
      init();
      int i;
      getCommand("\n",strlen("\n"));
      getCommand(DOFILE,strlen(DOFILE));
      printf("dofile done\n");
      getCommand(RUN,strlen(RUN));
      printf("check_wifi done\n");
      return 0;
}
