//
// Created by wami on 2020/07/24.
//

#ifndef CHIN_UP_COUNTER_SOUND_H
#define CHIN_UP_COUNTER_SOUND_H

#include <Arduino.h>

class Buzzer {
public:
  void begin();

  void startCountUp();
  void startCountDown();
  void startCountEnd();

};

#endif // CHIN_UP_COUNTER_SOUND_H
