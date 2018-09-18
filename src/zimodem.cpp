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

#include <Arduino.h>

#include "zimodem.h"
#include "modem_state.h"
#include "baudstate.h"
#include "board/board.h"

#include "mode/zmode.h"
#include "mode/zcommand.h"
#include "mode/zconfigmode.h"
#include "mode/zstream.h"


#ifdef INCLUDE_PETSCII
#include "pet2asc.h"
#endif

#include "rt_clock.h"
#include "filelog.h"
#include "serout.h"
#include "connSettings.h"
#include "stringstream.h"
#include "phonebook.h"
#include "wifi/wificlientnode.h"
#include "wifi/wifiservernode.h"
#include "proto/proto_http.h"
#include "proto/proto_ftp.h"

#ifdef INCLUDE_SD_SHELL
#include "proto/proto_xmodem.h"
#include "proto/proto_zmodem.h"
#include "mode/zbrowser.h"
#endif


int getDefaultCtsPin()
{
#ifdef ZIMODEM_ESP32
  return DEFAULT_PIN_CTS;
#else
  if((ESP.getFlashChipRealSize()/1024)>=4096) // assume this is a striketerm/esp12e
    return DEFAULT_PIN_CTS;
  else
    return 0;
#endif 
}


void s_pinWrite(uint8_t pinNo, uint8_t value)
{
  if(pinSupport[pinNo])
  {
    digitalWrite(pinNo, value);
  }
}

bool connectWifi(const char* ssid, const char* password)
{
  while(WiFi.status() == WL_CONNECTED)
  {
    WiFi.disconnect();
    delay(100);
    yield();
  }
  if(hostname.length() > 0)
  {
#ifdef ZIMODEM_ESP32
      tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, hostname.c_str());
#else
      WiFi.hostname(hostname);
#endif
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  bool amConnected = (WiFi.status() == WL_CONNECTED) && (strcmp(WiFi.localIP().toString().c_str(), "0.0.0.0")!=0);
  int WiFiCounter = 0;
  while ((!amConnected) && (WiFiCounter < 30))
  {
    WiFiCounter++;
    if(!amConnected)
      delay(500);
    amConnected = (WiFi.status() == WL_CONNECTED) && (strcmp(WiFi.localIP().toString().c_str(), "0.0.0.0")!=0);
  }
  wifiConnected = amConnected;
  if(!amConnected)
    WiFi.disconnect();
  return wifiConnected;
}

void checkBaudChange()
{
  switch(baudState)
  {
    case BS_SWITCH_TEMP_NEXT:
      changeBaudRate(tempBaud);
      baudState = BS_SWITCHED_TEMP;
      break;
    case BS_SWITCH_NORMAL_NEXT:
      changeBaudRate(baudRate);
      baudState = BS_NORMAL;
      break;
    default:
      break;
  }
}

void changeBaudRate(int baudRate)
{
  flushSerial(); // blocking, but very very necessary
  delay(500); // give the client half a sec to catch up
  debugPrintf("Baud change to %d.\n",baudRate);
  dequeSize=1+(baudRate/INTERNAL_FLOW_CONTROL_DIV);
  debugPrintf("Deque constant now: %d\n",dequeSize);

// TODO: make sure that this is handled cleaner. 
// There is possibly a bug in the ESP32 core that causes the second UART to go funny after
// re-initialization -- this was a patch to make it work, but I suspect that there has been some amount of fix
// applied to upstream, and Bo really should report this upstream before patching the hell
// out of the core libraries, making it hard to work with PlatformIO.

#if 0 
//#ifdef ZIMODEM_ESP32
  HWSerial.changeBaudRate(baudRate);
#else
  HWSerial.begin(baudRate, serialConfig);  //Change baud rate
#endif  
}

void changeSerialConfig(SerialConfig conf)
{
  flushSerial(); // blocking, but very very necessary
  delay(500); // give the client half a sec to catch up
  debugPrintf("Config changing %d.\n",(int)conf);
  dequeSize=1+(baudRate/INTERNAL_FLOW_CONTROL_DIV);
  debugPrintf("Deque constant now: %d\n",dequeSize);

// see changeBaudRate for why this looks weird at the moment. 
#if 0
//#ifdef ZIMODEM_ESP32
  HWSerial.changeConfig(conf);
#else
  HWSerial.begin(baudRate, conf);  //Change baud rate
#endif  
  debugPrintf("Config changed.\n");
}

int checkOpenConnections()
{
  int num=WiFiClientNode::getNumOpenWiFiConnections();
  if(num == 0)
  {
    if((dcdStatus == dcdActive)
    &&(dcdStatus != dcdInactive))
    {
      dcdStatus = dcdInactive;
      s_pinWrite(pinDCD,dcdStatus);
      if(baudState == BS_SWITCHED_TEMP)
        baudState = BS_SWITCH_NORMAL_NEXT;
      if(currMode == &commandMode)
        clearSerialOutBuffer();
    }
  }
  else
  {
    if((dcdStatus == dcdInactive)
    &&(dcdStatus != dcdActive))
    {
      dcdStatus = dcdActive;
      s_pinWrite(pinDCD,dcdStatus);
      if((tempBaud > 0) && (baudState == BS_NORMAL))
        baudState = BS_SWITCH_TEMP_NEXT;
    }
  }
  return num;
}

void setup() 
{
  for(int i=0;i<MAX_PIN_NO;i++)
    pinSupport[i]=false;
#ifdef ZIMODEM_ESP32
  Serial.begin(115200); //the debug port
  debugPrintf("Debug port open and ready.\n");
  for(int i=12;i<=23;i++)
    pinSupport[i]=true;
  for(int i=25;i<=27;i++)
    pinSupport[i]=true;
  for(int i=32;i<=33;i++)
    pinSupport[i]=true;
  pinSupport[36]=true;
  pinSupport[39]=true;
#else
  pinSupport[0]=true;
  pinSupport[2]=true;
  if((ESP.getFlashChipRealSize()/1024)>=4096) // assume this is a strykelink/esp12e
  {
    pinSupport[4]=true;
    pinSupport[5]=true;
    for(int i=9;i<=15;i++)
      pinSupport[i]=true;
    pinSupport[11]=false;
  }
#endif    
  #ifdef INCLUDE_SD_SHELL
  // set up the SD card shell for later
  // this is only set up if board.h tells us we should include the SD card
  initSDShell();
  #endif
  currMode = &commandMode;
  // If the SPIFFS fails to load, then something has happened
  if(!SPIFFS.begin())
  {
    // Format the whole goddamn thing
    SPIFFS.format();
    // try again.
    SPIFFS.begin();
    debugPrintf("SPIFFS Formatted.");
  }
  HWSerial.begin(DEFAULT_BAUD_RATE, DEFAULT_SERIAL_CONFIG);  //Start Serial
  commandMode.loadConfig();
  PhoneBookEntry::loadPhonebook();
  dcdStatus = dcdInactive;
  s_pinWrite(pinDCD,dcdStatus);
  flushSerial();
}

void loop() 
{
  if(HWSerial.available())
  {
    currMode->serialIncoming();
  }
  currMode->loop();
  zclock.tick();
}
