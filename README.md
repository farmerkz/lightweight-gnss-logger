# Простой GNSS логгер

Используется GNSS модуль u-blox M8. Конфигурация модуля должна быть конфигурацией по умолчанию. На модуле отключается протокол NMEA и включается протокол UBX, message UBX-NAV-PVT, скорость переключается на GPS_SPEED. Запись пакетов UBX-NAV-PVT в файл начинается после получения валидных даты и времени, имя файла формируется как MMDDHHMM.ubx