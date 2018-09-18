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

#include "filelog.h"
#include "serout.h"
#include "str.h"

static char HD[3];
static char HDL[17];

static long logStartTime = millis();
static long lastLogTime = millis();
static long logCurCount = 0;
static LogMode logMode = NADA;


uint8_t FROMHEXDIGIT(uint8_t a1)
{
  a1 = lc(a1);
  if((a1 >= '0')&&(a1 <= '9'))
    return a1-'0';
  if((a1 >= 'a')&&(a1 <= 'f'))
    return 10 + (a1-'a');
  return 0;
}

uint8_t FROMHEX(uint8_t a1, uint8_t a2)
{
  return (FROMHEXDIGIT(a1) * 16) + FROMHEXDIGIT(a2);
}

char *TOHEX(uint8_t a)
{
  HD[0] = "0123456789ABCDEF"[(a >> 4) & 0x0f];
  HD[1] = "0123456789ABCDEF"[a & 0x0f];
  HD[2] = 0;
  return HD;
}

char *tohex(uint8_t a)
{
  HD[0] = "0123456789abcdef"[(a >> 4) & 0x0f];
  HD[1] = "0123456789abcdef"[a & 0x0f];
  HD[2] = 0;
  return HD;
}

char *TOHEX(unsigned long a)
{
  for(int i=7;i>=0;i--)
  {
    HDL[i] = "0123456789ABCDEF"[a & 0x0f];
    a = a >> 4;
  }
  HDL[8] = 0;
  char *H=HDL;
  if((strlen(H)>2) && (strstr(H,"00")==H))
    H+=2;
  return H;
}

char *TOHEX(unsigned int a)
{
  for(int i=3;i>=0;i--)
  {
    HDL[i] = "0123456789ABCDEF"[a & 0x0f];
    a = a >> 4;
  }
  HDL[4] = 0;
  char *H=HDL;
  if((strlen(H)>2) && (strstr(H,"00")==H))
    H+=2;
  return H;
}

char *TOHEX(int a)
{
  return TOHEX((unsigned int)a);
}

char *TOHEX(long a)
{
  return TOHEX((unsigned long)a);
}

void logInternalOut(const LogMode m, const uint8_t c)
{
  if(logFileOpen)
  {
    if((m != logMode)
    ||(++logCurCount > DBG_BYT_CTR)
    ||((millis()-lastLogTime)>expectedSerialTime))
    {
      logCurCount=0;
      
      logMode = m;
      logFile.println("");
      switch(m)
      {
      case NADA:
        break;
      case SocketIn:
        logFile.printf("%s SocI: ",TOHEX(millis()-logStartTime));
        break;
      case SocketOut:
        logFile.printf("%s SocO: ",TOHEX(millis()-logStartTime));
        break;
      case SerialIn:
        logFile.printf("%s SerI: ",TOHEX(millis()-logStartTime));
        break;
      case SerialOut:
        logFile.printf("%s SerO: ",TOHEX(millis()-logStartTime));
        break;
      }
    }
    lastLogTime=millis();
    logFile.print(TOHEX(c));
    logFile.print(" ");
  }
}

void logSerialOut(const uint8_t c)
{
  logInternalOut(SerialOut,c);
}

void logSocketOut(const uint8_t c)
{
  logInternalOut(SocketOut,c);
}

void logSerialIn(const uint8_t c)
{
  logInternalOut(SerialIn,c);
}

void logSocketIn(const uint8_t c)
{
  logInternalOut(SocketIn,c);
}

 void logSocketIn(const uint8_t *c, int n)
{
  if(logFileOpen)
  {
    for(int i=0;i<n;i++)
      logInternalOut(SocketIn,c[i]);
  }
}

void logPrintfln(const char* format, ...) 
{
  if(logFileOpen)
  {
    if(logMode != NADA)
    {
      logFile.println("");
      logMode = NADA;
    }
    int ret;
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(FBUF,sizeof(FBUF), format, arglist);
    logFile.println(FBUF);
    va_end(arglist);
  }
}

void logPrintf(const char* format, ...) 
{
  if(logFileOpen)
  {
    if(logMode != NADA)
    {
      logFile.println("");
      logMode = NADA;
    }
    int ret;
    va_list arglist;
    va_start(arglist, format);
    vsnprintf(FBUF, sizeof(FBUF), format, arglist);
    logFile.print(FBUF);
    va_end(arglist);
  }
}

void logPrint(const char* msg)
{
  if(logFileOpen)
  {
    if(logMode != NADA)
    {
      logFile.println("");
      logMode = NADA;
    }
    logFile.print(msg);
  }
}

void logPrintln(const char* msg)
{
  if(logFileOpen)
  {
    if(logMode != NADA)
    {
      logFile.println("");
      logMode = NADA;
    }
    logFile.println(msg);
  }
}

