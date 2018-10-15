#ifndef __zimodem_board_config
#define __zimodem_board_config
#ifndef ZIMODEM_ESP8266
#error "This board configuration for ESP8266."
#endif

# define DEFAULT_PIN_DSR 13
# define DEFAULT_PIN_DTR 12
# define DEFAULT_PIN_RI 14
# define DEFAULT_PIN_RTS 4
# define DEFAULT_PIN_CTS 5 // is 0 for ESP-01, see getDefaultCtsPin() below.
# define DEFAULT_PIN_DCD 2
# define DEFAULT_FCT FCT_RTSCTS
# define debugPrintf doNothing


#endif