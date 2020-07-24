//
// Created by wami on 2020/07/24.
//

#include "Buzzer.h"

void Buzzer::begin() {
  ledcSetup(1, 12000, 8);
  ledcAttachPin(22, 1);
}

void Buzzer::startCountUp() {
  ledcWriteTone(1,2010.6);
  delay(200);
  ledcWriteTone(1,0);
}

void Buzzer::startCountDown() {
  ledcWriteTone(1,1318.6);
  delay(200);
  ledcWriteTone(1,0);
}

void Buzzer::startCountEnd() {
  ledcWriteTone(1, 1318.510); //ミ
  delay(125);
  ledcWriteTone(1, 1567.982); //ソ
  delay(125);
  ledcWriteTone(1, 2637.020); //ミ
  delay(125);
  ledcWriteTone(1, 2093.005); //ド
  delay(125);
  ledcWriteTone(1, 2349.318); //レ
  delay(125);
  ledcWriteTone(1, 3135.963); //ソ
  delay(125);
  ledcWriteTone(1, 0);
}

