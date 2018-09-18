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

#include "connSettings.h"

ConnSettings::ConnSettings(int flagBitmap)
{
#ifdef INCLUDE_PETSCII
  petscii = (flagBitmap & FLAG_PETSCII) > 0;
#endif
  telnet = (flagBitmap & FLAG_TELNET) > 0;
  echo = (flagBitmap & FLAG_ECHO) > 0;
  xonxoff = (flagBitmap & FLAG_XONXOFF) > 0;
  rtscts = (flagBitmap & FLAG_RTSCTS) > 0;
  secure = (flagBitmap & FLAG_SECURE) > 0;
}

ConnSettings::ConnSettings(const char *dmodifiers)
{
#ifdef INCLUDE_PETSCII
  petscii=((strchr(dmodifiers,'p')!=NULL) || (strchr(dmodifiers,'P')!=NULL));
#endif
  telnet=((strchr(dmodifiers,'t')!=NULL) || (strchr(dmodifiers,'T')!=NULL));
  echo=((strchr(dmodifiers,'e')!=NULL) || (strchr(dmodifiers,'E')!=NULL));
  xonxoff=((strchr(dmodifiers,'x')!=NULL) || (strchr(dmodifiers,'X')!=NULL));
  rtscts=((strchr(dmodifiers,'r')!=NULL) || (strchr(dmodifiers,'R')!=NULL));
  secure=((strchr(dmodifiers,'s')!=NULL) || (strchr(dmodifiers,'S')!=NULL));
}

ConnSettings::ConnSettings(String modifiers) : ConnSettings(modifiers.c_str())
{
}

int ConnSettings::getBitmap()
{
  int flagsBitmap = 0;
#ifdef INCLUDE_PETSCII
  if(petscii)
    flagsBitmap = flagsBitmap | FLAG_PETSCII;
#endif
  if(telnet)
    flagsBitmap = flagsBitmap | FLAG_TELNET;
  if(echo)
    flagsBitmap = flagsBitmap | FLAG_ECHO;
  if(xonxoff)
    flagsBitmap = flagsBitmap | FLAG_XONXOFF;
  if(rtscts)
    flagsBitmap = flagsBitmap | FLAG_RTSCTS;
  if(secure)
    flagsBitmap = flagsBitmap | FLAG_SECURE;
  return flagsBitmap;
}

int ConnSettings::getBitmap(FlowControlType forceCheck)
{
  int flagsBitmap = getBitmap();
  if(((flagsBitmap & (FLAG_XONXOFF | FLAG_RTSCTS))==0)
  &&(forceCheck==FCT_RTSCTS))
    flagsBitmap |= FLAG_RTSCTS;
  return flagsBitmap;
}

String ConnSettings::getFlagString()
{
  String lastOptions = "";
#ifdef INCLUDE_PETSCII
  lastOptions += (petscii?"p":"");
#endif
  lastOptions += (telnet?"t":"");
  lastOptions += (echo?"e":"");
  lastOptions += (xonxoff?"x":"");
  lastOptions += (rtscts?"r":"");
  lastOptions += (secure?"s":"");
  return lastOptions;
}
