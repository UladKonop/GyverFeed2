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

const int enPin = 8;
const int resPin = 9;
const int slpPin = 10;
const int stepPin = 11;
const int dirPin = 12;

#define EE_RESET 12         // любое число 0-255. Измени, чтобы сбросить настройки и обновить время
#define FEED_SPEED 1000     // задержка между шагами мотора (мкс)
#define BTN_PIN 7           // кнопка
#define STEPS_FRW 25        // шаги вперёд
#define STEPS_BKW 10        // шаги назад

// =========================================================
#include <EEPROM.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);

#include "EncButton.h"
EncButton<EB_TICK, BTN_PIN> btn;
int feedAmount = 1;

void setup() {
  
  pinMode(enPin, OUTPUT);
  pinMode(resPin, OUTPUT);
  pinMode(slpPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  digitalWrite(enPin, HIGH);
  digitalWrite(resPin, HIGH);
  digitalWrite(slpPin, HIGH);

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
    RtcDateTime now = rtc.GetDateTime();
    if (prevMin != now.Minute()) {
      prevMin = now.Minute();
      for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
       if (feedTime[i][0] == now.Hour() && feedTime[i][1] == now.Minute()) feed();
    }
  }

  btn.tick();
  if (btn.click()) feed();

  if (btn.hold()) {
    int newAmount = 0;
    while (btn.isHold()) {
      btn.tick();
      enableMotor();
      oneRev();
      newAmount++;
    }
    disableMotor();
    feedAmount = newAmount;
    EEPROM.put(1, feedAmount);
  }
}

void feed() {
  enableMotor();
  for (int i = 0; i < feedAmount; i++) oneRev();
  disableMotor();
}

void enableMotor() {
  digitalWrite(enPin, LOW);
}

void disableMotor() {
  digitalWrite(enPin, HIGH);
}

void oneRev() {
  for (int i = 0; i < STEPS_BKW; i++) runMotor(1);
  for (int i = 0; i < STEPS_FRW; i++) runMotor(0);
}

void runMotor(int8_t dir) {
  digitalWrite(dirPin, dir);
  
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(FEED_SPEED);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(FEED_SPEED);
}
