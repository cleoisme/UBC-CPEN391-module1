#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <altera_up_sd_card_avalon_interface.h>
#include "SDCard_Test_Program.h"
#include <altera_up_avalon_character_lcd.h>
#include "wifi.h"

void Wifi_Init(void) {
    // Reset
    WifiDongle_Control = 0x03;
    WifiDongle_Control = 0x15;
    WifiDongle_Baud = 0x01;
}
char getcharWifi(void) {
    while((WifiDongle_Status & 0x01) != 0x01) {}
    return WifiDongle_RxData;
}
void putcharWifi(char val) {
    while((WifiDongle_Status & 0x02) != 0x02) {}
    WifiDongle_TxData = val;
}
void WifiWait(void) {
    char val = getcharWifi();
    while(val != '>') {
        val = getcharWifi();
        //printf("%c", val);
    }
    //printf("\n");
}
void Wifi_SendCommand(const char * command) {
    int i;
    int length =strlen(command);
    for(i=0; i<length;i++) {
        putcharWifi(command[i]);
    }
}

// main(char*start, char *path)
void Wifi(char path1[], int countNum)
{
    //TestSDCard();
    //
    printf("Start\n");
    char bitmap[10000];
    char data[100];
    int i=0;
    int data_len;
    int count=1;
    int count_l=1;
    bool reach_start = false;
    bool reach_end = false;
    //----------------------------------------------
//  char loc1[] = "49.2606050,-123.2459940";
//  char loc2[] = "49.2606,-123.24";
//  char path[500];
//  strcat(path,"main(\"");
//  strcat(path, loc1);
//  strcat(path, "\",\"|");
//  strcat(path, loc2);
//  strcat(path, "\")\r\n");
//  printf("path is %s\n",path);
    //----------------------------------------------
//  char path[500];
//  strcat(path,"main(\"");
//  strcat(path, path1);
//  strcat(path, "\")\r\n");
//  printf("path is %s\n",path);
    Wifi_Init();
    Wifi_Init();
    Wifi_SendCommand("\r\n");
    Wifi_SendCommand("\r\n");
    Wifi_SendCommand("\r\n");
    Wifi_SendCommand("dofile(\"Connect.lua\")\r\n");
    WifiWait();
    printf("dofile done1 \n");
    Wifi_SendCommand("dofile(\"httpDLExample1.lua\")\r\n");
    WifiWait();
    printf("dofile done2 \n");
    //main("49.2606050,-123.2459940|49.2606,-123.24")
    //Wifi_SendCommand(path);
    Wifi_SendCommand("main(\"49.2606050,-123.2459940|49.2606,-123.24\")");
    char val;
    while(reach_start!=true){
        val = getcharWifi();
        data[i] = val;
        if(data[i-5]=='s'&&data[i-4]=='t'&&data[i-3]=='a'&&data[i-2]=='r'&&data[i-1]=='t'){
            reach_start=true;
            val = getcharWifi();
            break;
        }
        if(i>200){
            printf("1 should be crashed\n");
        }
        i++;
    }
    i=0;
    while(reach_end ==false){
        val = getcharWifi();
        bitmap[i] = val;
        if(bitmap[i-3]=='d'&&bitmap[i-2]=='o'&&bitmap[i-1]=='n'&&bitmap[i]=='e'){
            data_len = i-4;
            //printf("%d", data_len);
            bitmap[i-3]=NULL;
            bitmap[i-2]=NULL;
            bitmap[i-1]=NULL;
            bitmap[i]=NULL;
            reach_end=true;
            break;
        }
        if(i>8000){
                    printf("2 should be crashed\n");
                }
        i++;
    }
       printf("reach here 1\n");
        i=0;
        char* buf_str = (char*) malloc (2*sizeof(bitmap) + 1);
        char* buf_ptr = buf_str;
        for (i = 0; i <= data_len; i++)
        {
            if(count%2==0&&count_l %16!=0){
                buf_ptr += sprintf(buf_ptr, "%02x ", bitmap[i]&0xff);
            }
            else if(count_l %16==0){
                buf_ptr += sprintf(buf_ptr, "%02x\n", bitmap[i]&0xff);
            }
            else{
                buf_ptr += sprintf(buf_ptr, "%02x", bitmap[i]&0xff);
            }
            count++;
            count_l++;
        }
        //sprintf(buf_ptr,"\n");
        //*(buf_ptr + 1) = '\0';
        printf("%s\n", buf_str);
//  int map[data_len];
//  i=0;
//  while(i<=data_len){
//    if(count%2==0){
//      sprintf(map, "%i ", bitmap[i] & 0xff);
//      printf("%i ",map[i]);
//      printf("%02x  ",bitmap[i] & 0xff);
//    }
//    else if(count_l %16==0){
//      sprintf(map[i], "%02x\n", bitmap[i] & 0xff);
//    }
//    else{
//        sprintf(map[i], "%02x", bitmap[i] & 0xff);
//    }
//    i++;
//    count++;
//    count_l++;
//  }
//  printf("reach here 2\n");
//  WriteToFile(buf_str,data_len, countNum);
    TestSDCard();
    WriteToFile(buf_str,data_len, '0');
    printf("reach here 3\n");
    usleep(100000);
}

void Test(){
	printf("start\n");
	    char path[] = "49.2606050,-123.2459940|49.2606,-123.24";
	    Wifi(path, 0);
	//  TestSDCard();
	//
	//  int myFileHandle;
	//  printf("Reached level 1");
	//  myFileHandle = alt_up_sd_card_fopen("gps.txt", false);
	//  printf("%d", myFileHandle);
	//  if(myFileHandle != -1){
	//      printf("File Opened");
	//      short int readChar =  alt_up_sd_card_read(myFileHandle);
	//      char path[500];
	//      int countNum = 0;
	//      while(readChar >= 0){
	//          if(readChar == ' '){
	//              countNum++;
	//                Wifi(path, countNum);
	//              strcat(path, "|");
	//              readChar = alt_up_sd_card_read(myFileHandle);
	//          }
	//          else{
	//            strcat(path, readChar);
	//            readChar = alt_up_sd_card_read(myFileHandle);
	//          }
	//      }
	//
	//      printf("Done");
	//  }
	//  else{
	//      printf("File NOT OPENED");
	//  }
}
