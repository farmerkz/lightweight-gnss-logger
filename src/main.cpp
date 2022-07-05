#include "main.h"

/**
 * Hardware Connections:
 * Connect the U-Blox serial TX pin to Uno pin 8
 * Connect the U-Blox serial RX pin to Uno pin 9
 *
 *   SD card attached to SPI bus as follows:
 ** SDO - pin 11 MOSI
 ** SDI - pin 12 MISO
 ** CLK - pin 13 SCK
 ** CS  - pin 4/10
 **/

#ifdef CLASSIC_FILE_NAMES
#undef USE_LONG_FILE_NAMES
#endif

ublox gps;        // this is initializing the parser. The buffer is in this object
navpvt8 nav(gps); // this defines the UBX-NAV-PVT message

bool isDateValid = false;

SdFat32 sd;
File32 ubxFile;
uint8_t navBuf[PKT_NAV_LEN];              // Буфер для сырых данных
uint8_t *pointerBuf = (uint8_t *)&navBuf; //
uint8_t countPointer = 0;

#ifdef USE_LONG_FILE_NAMES
char filename[] = "YYYY-MM-DD_hh:mm.ubx";
#else
char filename[] = "MMDDhhmm.ubx";
#endif

/**
 * @brief Отправка байта на GNSS
 *
 * @param b байт для отправки
 */
void sendByte(byte b)
{
  Serial.write(b);
}

/**
 * @brief Отправка пакета на GNSS
 *
 * @param packet пакет для отправки
 * @param len длина пакета
 */
void sendPacket(byte *packet, byte len)
{
  for (byte i = 0; i < len; i++)
  {
    Serial.write(packet[i]);
  }
  Serial.flush();
  delay(20);
}

/**
 * @brief Callback для установки timestamp файла. Вызывается только при создании
 * файла и sync()
 *
 * @param date дата
 * @param time время
 * @param ms10 миллисекунды
 */
void dateTime(uint16_t *date, uint16_t *time, uint8_t *ms10)
{

  // Return date using FS_DATE macro to format fields.
  *date = FS_DATE(year(), month(), day());

  // Return time using FS_TIME macro to format fields.
  *time = FS_TIME(hour(), minute(), second());

  // Return low time bits in units of 10 ms.
  *ms10 = second() & 1 ? 100 : 0;
}

/**
 * @brief Оановная программа
 *
 */
void setup()
{
  delay(500);

  if (!sd.begin(SD_CONFIG))
  {
    while (1)
      ;
  }

  Serial.begin(9600);

  changeBaudrate(GPS_SPEED);

  delay(100);
  Serial.begin(GPS_SPEED);

  delay(100);
  disableNmea();

  delay(100);
  enableNavPvt();
  Serial.flush();

  delay(100);

  FsDateTime::setCallback(dateTime);
  Watchdog.enable(RST_MODE, WDT_TIMEOUT_4S); // Режим сторжевого сброса , таймаут ~ 4 секунды
}

/**
 * @brief Основной цикл
 *
 */
void loop()
{

  while (Serial.available())
  {
    uint8_t c = Serial.read();

    *(pointerBuf + countPointer++) = c;
    if (countPointer >= PKT_NAV_LEN)
    {
      countPointer = 0;
    }

    char *r = (char *)gps.parse(c);
    // myState = gps.state;

    if (gps.state == State::sync1)
    {
      countPointer = 0;
    }

    if (strlen(r) > 0)
    {
      if (strcmp(r, "navpvt8") == 0)
      {
        if ((nav.getvalid() & VALID_DATE) &&
            (nav.getvalid() & VALID_TIME) &&
            !isDateValid)
        {
          sprintf(filename,
                  "%02d%02d%02d%02d.ubx",
                  nav.getmonth(),
                  nav.getday(),
                  nav.gethour(),
                  nav.getminute());

          setTime(nav.gethour(),
                  nav.getminute(),
                  nav.getsecond(),
                  nav.getday(),
                  nav.getmonth(),
                  nav.getyear());
          adjustTime(TIME_OFFSET * SECS_PER_HOUR);
          isDateValid = true;
          ubxFile = sd.open(filename, FILE_WRITE | O_APPEND);
        }
        if (isDateValid)
        {
          ubxFile.write(navBuf, sizeof(navBuf));
          ubxFile.flush();
        }
        countPointer = 0;
      }
    }
    Watchdog.reset();
  }
}