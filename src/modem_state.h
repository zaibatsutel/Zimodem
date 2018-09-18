
#ifndef __MODEM_STATE__
#define __MODEM_STATE__


#include "board/board.h"
#include "board/defaults.h"

#include "baudstate.h"

static bool wifiConnected =false;
static String wifiSSI;
static String wifiPW;
static String hostname;
static SerialConfig serialConfig = DEFAULT_SERIAL_CONFIG;
static int baudRate=DEFAULT_BAUD_RATE;
static int dequeSize=1+(DEFAULT_BAUD_RATE/INTERNAL_FLOW_CONTROL_DIV);
static BaudState baudState = BS_NORMAL; 
static int tempBaud = -1; // -1 do nothing
static int dcdStatus = LOW;
static int pinDCD = DEFAULT_PIN_DCD;
static int pinCTS = DEFAULT_PIN_CTS;
static int pinRTS = DEFAULT_PIN_RTS;
static int pinDSR = DEFAULT_PIN_DSR;
static int pinDTR = DEFAULT_PIN_DTR;
static int pinRI = DEFAULT_PIN_RI;
static int dcdActive = DEFAULT_DCD_HIGH;
static int dcdInactive = DEFAULT_DCD_LOW;
static int ctsActive = DEFAULT_CTS_HIGH;
static int ctsInactive = DEFAULT_CTS_LOW;
static int rtsActive = DEFAULT_RTS_HIGH;
static int rtsInactive = DEFAULT_RTS_LOW;
static int riActive = DEFAULT_RI_HIGH;
static int riInactive = DEFAULT_RI_LOW;
static int dtrActive = DEFAULT_DTR_HIGH;
static int dtrInactive = DEFAULT_DTR_LOW;
static int dsrActive = DEFAULT_DSR_HIGH;
static int dsrInactive = DEFAULT_DSR_LOW;


static bool pinSupport[MAX_PIN_NO];

static bool browseEnabled;


#ifdef INCLUDE_SD_SHELL
#include "mode/zbrowser.h"
#endif

#endif