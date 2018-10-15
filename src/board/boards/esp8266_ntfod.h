#ifndef __zimodem_board_config
#define __zimodem_board_config
#ifndef ZIMODEM_ESP8266
#error "This board configuration for ESP8266."
#endif


#define NTFOD_CTL_OUT 12	// RTS/DTR 
#define NTFOD_CTL_IN  14	// CTS/DSR
#define NTFOD_DCD_RI  13	// DCD and RI share a pin. This is not optimal and should be fixed later on. 

# define DEFAULT_PIN_DSR NTFOD_CTL_OUT
# define DEFAULT_PIN_DTR NTFOD_CTL_IN
# define DEFAULT_PIN_RI  NTFOD_DCD_RI
# define DEFAULT_PIN_RTS NTFOD_CTL_OUT
# define DEFAULT_PIN_CTS NTFOD_CTL_IN
# define DEFAULT_PIN_DCD NTFOD_DCD_RI
# define DEFAULT_FCT     FCT_DISABLED
# define debugPrintf     Serial.printf

#endif