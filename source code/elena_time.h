#include <stdint.h>
#ifndef ELENA_TIME_H
#define ELENA_TIME_H

#include <virtuabotixRTC.h>

class RealTimeManager {
private:
  virtuabotixRTC* rtc;

  // Cek tahun kabisat
  bool isLeapYear(int year);
  // Maksimal hari dalam bulan
  int getMaxDay(uint8_t month, int year);

public:
  RealTimeManager(virtuabotixRTC* rtcPtr);

  // Func 1: Set time manually
  void setTimeManual(uint8_t day, uint8_t  month, int year, uint8_t  hours, uint8_t  minutes, uint8_t  seconds);
  // Func 2: Show time(HH:MM:SS)
  void showTime();
  // Func 3: Show Date (DD:MM:YYYY)
  void showDate();
};


#endif
