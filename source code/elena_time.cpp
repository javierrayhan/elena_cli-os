#include "elena_time.h"

#include <stdint.h>
#include <virtuabotixRTC.h>

RealTimeManager::RealTimeManager(virtuabotixRTC* rtcPtr) {
  rtc = rtcPtr;
}

  // Cek tahun kabisat
  bool RealTimeManager::isLeapYear(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
  }

  // Maksimal hari dalam bulan
  int RealTimeManager::getMaxDay(uint8_t month, int year) {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) {
      return 29;
    }
    return daysInMonth[month - 1];
  }

  // Fungsi 1: Atur waktu manual
  void RealTimeManager::setTimeManual(uint8_t day, uint8_t  month, int year, uint8_t  hours, uint8_t  minutes, uint8_t  seconds) {
    if (month < 1 || month > 12) {
      Serial.println(F("[!] Invalid Month."));
      return;
    }
    int maxDay = getMaxDay(month, year);
    if (day < 1 || day > maxDay) {
      Serial.print(F("[!] Day not valid, Max: "));
      Serial.println(maxDay);
      return;
    }
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
      Serial.println(F("[!] Time invalid."));
      return;
    }

    rtc->setDS1302Time(seconds, minutes, hours, 1, day, month, year);
    Serial.println(F("[S] Time set!."));
  }

  // Fungsi 2: Tampilkan waktu (HH:MM:SS)
  void RealTimeManager::showTime() {
    rtc->updateTime();
    if (rtc->hours < 10) Serial.print('0');
    Serial.print(rtc->hours); Serial.print(':');
    if (rtc->minutes < 10) Serial.print('0');
    Serial.print(rtc->minutes); Serial.print(':');
    if (rtc->seconds < 10) Serial.print('0');
    Serial.println(rtc->seconds);
  }

  // Fungsi 3: Tampilkan tanggal (DD:MM:YYYY)
  void RealTimeManager::showDate() {
    rtc->updateTime();
    if (rtc->dayofmonth < 10) Serial.print('0');
    Serial.print(rtc->dayofmonth); Serial.print('/');
    if (rtc->month < 10) Serial.print('0');
    Serial.print(rtc->month); Serial.print('/');
    Serial.println(rtc->year);
  }



