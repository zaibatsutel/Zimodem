
#include "baudstate.h"
#include "modem_state.h"
#include "board/board.h"
#include "board/defaults.h"

bool wifiConnected =false;
String wifiSSI;
String wifiPW;
String hostname;
SerialConfig serialConfig = DEFAULT_SERIAL_CONFIG;
int baudRate=DEFAULT_BAUD_RATE;
int dequeSize=1+(DEFAULT_BAUD_RATE/INTERNAL_FLOW_CONTROL_DIV);
BaudState baudState = BS_NORMAL; 
int tempBaud = -1; // -1 do nothing
int dcdStatus = LOW;
int pinDCD = DEFAULT_PIN_DCD;
int pinCTS = DEFAULT_PIN_CTS;
int pinRTS = DEFAULT_PIN_RTS;
int pinDSR = DEFAULT_PIN_DSR;
int pinDTR = DEFAULT_PIN_DTR;
int pinRI = DEFAULT_PIN_RI;
int dcdActive = DEFAULT_DCD_HIGH;
int dcdInactive = DEFAULT_DCD_LOW;
int ctsActive = DEFAULT_CTS_HIGH;
int ctsInactive = DEFAULT_CTS_LOW;
int rtsActive = DEFAULT_RTS_HIGH;
int rtsInactive = DEFAULT_RTS_LOW;
int riActive = DEFAULT_RI_HIGH;
int riInactive = DEFAULT_RI_LOW;
int dtrActive = DEFAULT_DTR_HIGH;
int dtrInactive = DEFAULT_DTR_LOW;
int dsrActive = DEFAULT_DSR_HIGH;
int dsrInactive = DEFAULT_DSR_LOW;
bool pinSupport[MAX_PIN_NO];
bool browseEnabled = false;
