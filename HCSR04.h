//
// Created by wami on 2020/07/24.
//

#ifndef CHIN_UP_COUNTER_HCSR04_H
#define CHIN_UP_COUNTER_HCSR04_H

#include <Arduino.h>

class HCSR04 {
public:
  HCSR04(void) {};
  virtual ~HCSR04() {};

  void begin(int Trig, int Echo);
  float distance(void);
private:
  uint8_t _trig;
  uint8_t _echo;
};

#endif // CHIN_UP_COUNTER_HCSR04_H
