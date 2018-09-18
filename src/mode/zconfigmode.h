/*
   Copyright 2016-2016 Bo Zimmerman

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
#ifndef __ZCONFIGMODE_H__
#define __ZCONFIGMODE_H__

#include <Arduino.h>

#include "zmode.h"

#include "../serout.h"
#include "../wifi/wifiservernode.h"

class ZConfig : public ZMode
{
  private:
    enum ZConfigMenu
    {
      ZCFGMENU_MAIN=0,
      ZCFGMENU_NUM=1,
      ZCFGMENU_ADDRESS=2,
      ZCFGMENU_OPTIONS=3,
      ZCFGMENU_WIMENU=4,
      ZCFGMENU_WIFIPW=5,
      ZCFGMENU_WICONFIRM=6,
      ZCFGMENU_FLOW=7,
      ZCFGMENU_BBSMENU=8,
      ZCFGMENU_NEWPORT=9,
      ZCFGMENU_NEWHOST=10
    } currState;
    
    ZSerial serial; // storage for serial settings only

    void switchBackToCommandMode();
    void doModeCommand();
    bool showMenu;
    bool savedEcho;
    String EOLN;
    const char *EOLNC;
    unsigned long lastNumber;
    String lastAddress;
    String lastOptions;
    WiFiServerSpec serverSpec;
    bool newListen;
    bool settingsChanged=false;
    int lastNumNetworks=0;

  public:
    void switchTo();
    void serialIncoming();
    void loop();
};

static ZConfig configMode;

#endif