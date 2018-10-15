#ifndef __ZIMODEM_BOARD_CONFIG__
#define __ZIMODEM_BOARD_CONFIG__

#include "defaults.h"
#define MAX_PIN_NO 50

/*  This will in the future handle loading board-specific details
 * for now, we are simply defining some of the more bosic board-specific things
 * specifically:
 * 
 *  - hardware serial
 *  - default pins for flow control and transmit/recieve
 *  - etc, etc. 
*/


#ifndef ZIMODEM_TARGET
#error "No board target specified. Please specify one with -DZIMODEM_TARGET="
#else
// This is a hack:
// The C preprocessor allows us to expand macros only in #defines, but allows us to
// use a single #define in an include path
// To the poor sod who has to figure this out:
// https://stackoverflow.com/a/32077478 I stole it from here

// This creates the identity of a thing, making it a token
#define IDENT(x) x
// this creates the stringified version of a token
#define XSTR(x) #x
// this turns the stringified thing into a token
#define STR(x) XSTR(x)
// this smashes several tokens together
#define PATH(x,y,z) STR(IDENT(x)IDENT(y)IDENT(z))
// this defines the two sides of our token-string-contraption
#define BOARD_DIR boards/
#define BOARD_EXT .h
// and this crams it into the place we expect it. 
#include PATH(BOARD_DIR,ZIMODEM_TARGET, BOARD_EXT)



#endif

#if ZIMODEM_ESP32
# include <WiFi.h>
# define ENC_TYPE_NONE WIFI_AUTH_OPEN
# include <HardwareSerial.h>
# include <SPIFFS.h>
# include <Update.h>
# include "SD.h"
# include "SPI.h"
# include "driver/uart.h"
//HardwareSerial HWSerial(UART_NUM_2);
// This is defined in HardwareSerial.h -- we can use the global serial helpers. 
#define HWSerial Serial2
#elif ZIMODEM_ESP8266
# include "ESP8266WiFi.h"
#include <FS.h>
# define HWSerial Serial
#endif

inline void doNothing(const char* format, ...) {
	/* does nothing */
}


#if 0
#ifdef ZIMODEM_ESP32
// ESP32 specific things. 
# define DEFAULT_PIN_DCD GPIO_NUM_14
# define DEFAULT_PIN_CTS GPIO_NUM_13
# define DEFAULT_PIN_RTS GPIO_NUM_15 // unused
# define DEFAULT_PIN_RI GPIO_NUM_32
# define DEFAULT_PIN_DSR GPIO_NUM_12
# define DEFAULT_PIN_DTR GPIO_NUM_27
# define debugPrintf Serial.printf
//# define INCLUDE_SD_SHELL true
# define DEFAULT_FCT FCT_DISABLED
# define SerialConfig uint32_t
# define UART_CONFIG_MASK 0x8000000
# define UART_NB_BIT_MASK      0B00001100 | UART_CONFIG_MASK
# define UART_NB_BIT_5         0B00000000 | UART_CONFIG_MASK
# define UART_NB_BIT_6         0B00000100 | UART_CONFIG_MASK
# define UART_NB_BIT_7         0B00001000 | UART_CONFIG_MASK
# define UART_NB_BIT_8         0B00001100 | UART_CONFIG_MASK
# define UART_PARITY_MASK      0B00000011
# define UART_PARITY_NONE      0B00000000
# define UART_NB_STOP_BIT_MASK 0B00110000
# define UART_NB_STOP_BIT_0    0B00000000
# define UART_NB_STOP_BIT_1    0B00010000
# define UART_NB_STOP_BIT_15   0B00100000
# define UART_NB_STOP_BIT_2    0B00110000

#elif ZIMODEM_ESP8266  // ESP-8266, e.g. ESP-01, ESP-12E, inverted for C64Net WiFi Modem
# define DEFAULT_PIN_DSR 13
# define DEFAULT_PIN_DTR 12
# define DEFAULT_PIN_RI 14
# define DEFAULT_PIN_RTS 4
# define DEFAULT_PIN_CTS 5 // is 0 for ESP-01, see getDefaultCtsPin() below.
# define DEFAULT_PIN_DCD 2
# define DEFAULT_FCT FCT_RTSCTS

//# define RS232_INVERTED 1
#ifndef ZIMODEM_SLIME
# define debugPrintf doNothing
inline void doNothing(const char* format, ...) {
	/* does nothing */
}
#else
#define debugPrintf Serial.printf
#endif


#else
#error "FAILURE: Unknown build target????"

#endif

#endif // 0


#endif
