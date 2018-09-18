#ifndef __ZIMODEM_H__
#define __ZIMODEM_H__

#include <Arduino.h>
#include "board/board.h"


//#define TCP_SND_BUF                     4 * TCP_MSS
#define ZIMODEM_VERSION "3.4"
const char compile_date[] = __DATE__ " " __TIME__;

void s_pinWrite(uint8_t pinNo, uint8_t value);
int getDefaultCtsPin();
bool connectWifi(const char* ssid, const char* password);
void checkBaudChange();
void changeBaudRate(int baudRate);
void changeSerialConfig(SerialConfig conf);
int checkOpenConnections();

#endif
