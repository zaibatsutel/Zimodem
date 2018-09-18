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
# define RS232_INVERTED 1
# define debugPrintf doNothing
inline void doNothing(const char* format, ...) { /* it does nothing! */ }

#else
#error "FAILURE: Unknown build target????"

#endif

#endif