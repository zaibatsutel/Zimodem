
#ifndef __MODEM_STATE__
#define __MODEM_STATE__


#include "board/board.h"
#include "board/defaults.h"

#include "baudstate.h"

extern bool wifiConnected;
extern String wifiSSI;
extern String wifiPW;
extern String hostname;
extern SerialConfig serialConfig;
extern int baudRate;
extern int dequeSize;
extern BaudState baudState; 
extern int tempBaud;
extern int dcdStatus;
extern int pinDCD;
extern int pinCTS;
extern int pinRTS;
extern int pinDSR;
extern int pinDTR;
extern int pinRI;
extern int dcdActive;
extern int dcdInactive;
extern int ctsActive;
extern int ctsInactive;
extern int rtsActive;
extern int rtsInactive;
extern int riActive;
extern int riInactive;
extern int dtrActive;
extern int dtrInactive;
extern int dsrActive;
extern int dsrInactive;
extern bool pinSupport[];
extern bool browseEnabled;


// This should not be here. 
#ifdef INCLUDE_SD_SHELL
#include "mode/zbrowser.h"
#endif

#endif
