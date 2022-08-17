/*
  Скетч к проекту "Автокормушка 2"
  - Страница проекта (схемы, описания): https://alexgyver.ru/gyverfeed2/
  - Исходники на GitHub: https://github.com/AlexGyver/GyverFeed2/
  Проблемы с загрузкой? Читай гайд для новичков: https://alexgyver.ru/arduino-first/
  AlexGyver, AlexGyver Technologies, 2020
*/

// v2.1 - исправлен баг с невыключением мотора

// Клик - внеочередная кормёжка
// Удержание - задаём размер порции
const byte feedTime[][2] = {
  {7, 0},       // часы, минуты. НЕ НАЧИНАТЬ ЧИСЛО С НУЛЯ
  {11, 52},
  {11, 58},
  {12, 4},
  {12,5},
};

#define EE_RESET 12         // любое число 0-255. Измени, чтобы сбросить настройки и обновить время
#define FEED_SPEED 3000     // задержка между шагами мотора (мкс)
#define BTN_PIN 7           // кнопка
#define STEPS_FRW 19        // шаги вперёд
#define STEPS_BKW 12        // шаги назад
//const byte drvPins[] = {8, 9, 10, 11};  // драйвер (фазаА1, фазаА2, фазаВ1, фазаВ2)

// =========================================================
#include <EEPROM.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);

#include "EncButton.h"
EncButton<EB_TICK, BTN_PIN> btn;
int feedAmount = 100;

void setup() {
  Serial.begin(9600);
  rtc.Begin();
  
//  if (EEPROM.read(0) != EE_RESET) {   // первый запуск
//    EEPROM.write(0, EE_RESET);
//    EEPROM.put(1, feedAmount);
    
//    Ds1302::DateTime now;
//    rtc.getDateTime(&now);

    
//    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
//  }
//  EEPROM.get(1, feedAmount);
//  for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
}

void loop() {
  static uint32_t tmr = 0;
  if (millis() - tmr > 500) {           // два раза в секунду
    static byte prevMin = 0;
    tmr = millis();
//    DateTime now = rtc.getTime();
    RtcDateTime now = rtc.GetDateTime();
    if (prevMin != now.Minute()) {
      prevMin = now.Minute();
      for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
//       if (feedTime[i][0] == now.Hour() && feedTime[i][1] == now.Minute()) feed();
        if (feedTime[i][0] == now.Hour() && feedTime[i][1] == now.Minute()){ 
          printDateTime(now);
          Serial.println();        
        }
    }
  }

  btn.tick();
//  if (btn.click()) feed();
  if (btn.click()) {
    RtcDateTime now = rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
  }
//
//  if (btn.hold()) {
//    int newAmount = 0;
//    while (btn.isHold()) {
//      btn.tick();
//      oneRev();
//      newAmount++;
//    }
//    disableMotor();
//    feedAmount = newAmount;
//    EEPROM.put(1, feedAmount);
//  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
//void feed() {
//  for (int i = 0; i < feedAmount; i++) oneRev();
//  disableMotor();
//}

// выключаем ток на мотор
//void disableMotor() {
//  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], 0);
//}
//
//void oneRev() {
//  for (int i = 0; i < STEPS_BKW; i++) runMotor(-1);
//  for (int i = 0; i < STEPS_FRW; i++) runMotor(1);
//}
//
//const byte steps[] = {0b1010, 0b0110, 0b0101, 0b1001};
//void runMotor(int8_t dir) {
//  static byte step = 0;
//  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], bitRead(steps[step & 0b11], i));
//  delayMicroseconds(FEED_SPEED);
//  step += dir;
//}
