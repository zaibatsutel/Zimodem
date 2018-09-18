/*
   Copyright 2016-2017 Bo Zimmerman

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __FILELOG_H__
#define __FILELOG_H__

//#include <Arduino.h>
#include <cstdint>
#include <FS.h>

enum LogMode
{
  NADA=0,
  SocketIn=1,
  SocketOut=2,
  SerialIn=3,
  SerialOut=4
};

static unsigned long expectedSerialTime = 1000;

static bool logFileOpen = false;
static File logFile; 

void logSerialOut(const uint8_t c);
void logSocketOut(const uint8_t c);
void logSerialIn(const uint8_t c);
void logSocketIn(const uint8_t c);
void logSocketIn(const uint8_t*, int);
void logPrint(const char* msg);
void logPrintln(const char* msg);
void logPrintf(const char* format, ...);
void logPrintfln(const char* format, ...);
char *TOHEX(long a);
char *TOHEX(int a);
char *TOHEX(unsigned int a);
char *TOHEX(unsigned long a);
char *tohex(uint8_t a);
char *TOHEX(uint8_t a);
uint8_t FROMHEX(uint8_t a1, uint8_t a2);

#endif