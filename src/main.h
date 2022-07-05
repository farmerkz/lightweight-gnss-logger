#ifndef __MAIN_H__
#define __MAIN_H__

#include <Arduino.h>
#include "u-blox-arduino.h"
#include "SdFat.h"
#include "changebaudrate.h"
#include "disablenmea.h"
#include "enablenavpvt.h"
#include <TimeLib.h>
#include "GyverWDT.h"

#define MOSI_PIN 11                                                // SDO
#define MISO_PIN 12                                                // SDI
#define SCK_PIN 13                                                 // CLK
#define SD_CS_PIN 10                                               // CS
#define SPI_CLOCK SD_SCK_MHZ(8)                                    //
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK) //

#define PKT_NAV_LEN 100

#define VALID_DATE (1 << 0)
#define VALID_TIME (1 << 1)
#define TIME_OFFSET 6

// #define SERIAL_RX_BUFFER_SIZE 256
#endif // __MAIN_H__
