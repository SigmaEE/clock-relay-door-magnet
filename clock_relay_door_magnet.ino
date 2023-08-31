// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

const int relay_pin = 2;
const int led_pin = 13;

struct myTimeStruct {
  int hour;
  int minute;
};

constexpr myTimeStruct start_relay_time = {7, 30};
constexpr myTimeStruct stop_relay_time = {16, 30};

#define DEBUG_ENABLED 0

void setup () {
  Serial.begin(57600);
  pinMode(relay_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //Un comment the line below in case the time is not correct
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


void loop () {
  DateTime now = rtc.now();

#ifdef DEBUG_ENABLED
  print_time(now);
#endif

  switch (now.hour()) {
    case start_relay_time.hour:
      if (now.minute() >= start_relay_time.minute) {
        enable_relay();
      } else {
        disable_relay();
      }
      break;
    case stop_relay_time.hour:
      if (now.minute() < stop_relay_time.minute) {
        enable_relay();
      } else {
        disable_relay();
      }
      break;
    default:
      if (now.hour() > start_relay_time.hour and now.hour() < stop_relay_time.hour) {
        enable_relay();
      } else {
        disable_relay();
      }
      break;
  }
}

void enable_relay() {
  // Relay is connected as active when Low
  digitalWrite(relay_pin, HIGH);
}

void disable_relay() {
  // Relay is connected as disactive when High
  digitalWrite(relay_pin, LOW);
}

void print_time(DateTime now) {
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(1000);
}
