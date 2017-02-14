#include <avr/power.h>
#include <Wire.h>
#include <EEPROM.h>
#include "RTClib.h"

#define VALVE_PIN 1

RTC_PCF8523 rtc;

TimeSpan ft, fd;

/*************************************************************************************************************
  setup
*************************************************************************************************************/
void setup() {
  // set speed to 16 mHz
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);

  // rtc setup
  if(!rtc.begin()) {
    while(1);
  }

  // rtc init (only runs the first time)
  if(!rtc.initialized()) {
    // set date (don't modify this line)
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // set flush time (0, hours, minutes, seconds) after midnight
    ft = TimeSpan(0, 6, 0, 0);
  
    // set flush duration (seconds)
    fd = TimeSpan(300);
  
    // write to eeprom
    int ee_addr = 0;
    EEPROM.put(ee_addr, ft);
    ee_addr += sizeof(TimeSpan);
    EEPROM.put(ee_addr, fd);
  }
  // otherwise just read flush settings
  else {
    // read from eeprom
    int ee_addr = 0;
    EEPROM.get(ee_addr, ft);
    ee_addr += sizeof(TimeSpan);
    EEPROM.get(ee_addr, fd);
  }

  // valve setup
  pinMode(VALVE_PIN, OUTPUT);
  digitalWrite(VALVE_PIN, LOW);
}

/*************************************************************************************************************
  loop
*************************************************************************************************************/
void loop() {
  // current time
  DateTime dt = rtc.now();

  // today @ midnight
  DateTime td(dt.year(), dt.month(), dt.day());

  // flush start
  TimeSpan t1 = dt - (td + ft);

  // flush stop
  TimeSpan t2 = dt - (td + ft + fd);

  // check for flush
  if((t1.totalseconds() >= 0) && (t2.totalseconds() < 0))
    digitalWrite(VALVE_PIN, HIGH);
  else
    digitalWrite(VALVE_PIN, LOW);

  // delay before next loop
  delay(1000);
}

