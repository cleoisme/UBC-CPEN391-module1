
#ifndef WIFI_H_
#define WIFI_H_

#define WifiDongle_Control (*(volatile unsigned char *)(0x84000240))
#define WifiDongle_Status (*(volatile unsigned char *)(0x84000240))
#define WifiDongle_TxData (*(volatile unsigned char *)(0x84000242))
#define WifiDongle_RxData (*(volatile unsigned char *)(0x84000242))
#define WifiDongle_Baud (*(volatile unsigned char *)(0x84000244))

char getcharWifi(void);
void Wifi_SendCommand(const char * command);
void putcharWifi(char val);

#endif /* WIFI_H_ */
