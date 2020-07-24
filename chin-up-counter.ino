#include "Buzzer.h"
#include <Wire.h>
#include "VL53L0X.h"
#include <obniz.h>

#define CHIN_UP_END 5

VL53L0X sensor;
Buzzer buzzer;
uint8_t chin_up_finish = CHIN_UP_END;
uint8_t chin_up_count = 0;

uint16_t free_counter = 0;
uint16_t meter = 0;
uint32_t ave_meter = 0;
uint16_t ave_count = 0;
uint16_t old_count = 0;
uint32_t time_msec = 0;

bool chin_up_flg = false;
char message[100];

void onCommand(uint8_t* data, uint16_t length){
  Serial.println("\nonCommand");
  Serial.write(data,length);
  Serial.println("\nEnd onCommand");
  if(length > 2 && data[0] == 'm'&& data[1] == 'd'){
    switch (data[2] ){
      case 'c':
        chin_up_count = 0;
        old_count = 0;
        chin_up_flg = false;
        break;
    }
  }else{
    chin_up_finish = data[0];
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  obniz.commandReceive(onCommand);
  obniz.start();

  while(!obniz.getId());
  Serial.printf("obnizID : %s\n",obniz.getId());

  buzzer.begin();
  Wire.begin(27,14);

  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
}


void loop() {
  meter = sensor.readRangeContinuousMillimeters();
//  Serial.print("row : ");
//  Serial.print(meter);
//  Serial.println(" mm");

  if(meter > 2000 ){
    return;
  }

  ave_meter += meter;
  ave_count++;

  if(ave_count >= 8){
    uint16_t tmp_meter = ave_meter/ave_count;

    ave_count = 0;
    ave_meter = 0;
//    Serial.print("ave : ");
//    Serial.print(tmp_meter);
//    Serial.println(" mm");
//    Serial.print("time : ");
//    Serial.print((micros()-time_msec)/1000);
//    Serial.println(" msec");
    time_msec = micros();
    if(tmp_meter > 700 && chin_up_flg){
      Serial.println(" 700");
      old_count++;
      if(old_count >= 2){
        chin_up_flg = false;
        old_count = 0;
        buzzer.startCountDown();
        sprintf(message, "{\"count\":%d,\"max_count\":%d,\"finish\":false,\"mode\":\"down\"}", chin_up_count, chin_up_finish);
        if (!obniz.commandSend((uint8_t *)message, strlen(message))) {
          Serial.printf("send message : %s\n", message);
        }
      }
    }else if(tmp_meter < 500 && !chin_up_flg){
      Serial.println(" 500");
      old_count++;
      if(old_count >= 2){
        chin_up_flg = true;
        old_count = 0;
        chin_up_count++;
        if(chin_up_count >= chin_up_finish){
          sprintf(message, "{\"count\":%d,\"max_count\":%d,\"finish\":true,\"mode\":\"up\"}", chin_up_count, chin_up_finish);
          if (!obniz.commandSend((uint8_t *)message, strlen(message))) {
            Serial.printf("send message : %s\n", message);
          }
          buzzer.startCountEnd();
          chin_up_count = 0;
          chin_up_flg = false;
          delay(5000);
        }else {
          buzzer.startCountUp();
          sprintf(message, "{\"count\":%d,\"max_count\":%d,\"finish\":false,\"mode\":\"up\"}", chin_up_count, chin_up_finish);
          if (!obniz.commandSend((uint8_t *)message, strlen(message))) {
            Serial.printf("send message : %s\n", message);
          }
        }
      }
    }
  }
}
