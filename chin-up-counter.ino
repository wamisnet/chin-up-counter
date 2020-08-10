#include "Buzzer.h"
#include "VL53L0X.h"
#include <obniz.h>

#define CHIN_UP_END 5
#define AVERAGE_DISTANCE 4
#define CHIN_UP_DISTANCE_HIGH 600
#define CHIN_UP_DISTANCE_LOW 450

VL53L0X sensor;
Buzzer buzzer;
uint8_t chin_up_finish = CHIN_UP_END;
uint8_t chin_up_count = 0;

uint32_t ave_meter = 0;
uint16_t ave_counter = 0;
uint16_t old_count = 0;

bool chin_up_flg = false;

void count_clear();
void sendCommand(uint8_t chin_up_count,bool finish_flg,uint8_t chin_up_finish,bool up_flg);

void onCommand(uint8_t* data, uint16_t length) {
  if (length > 2 && data[0] == 'm' && data[1] == 'd' && data[1] == 'c') {
    count_clear();
  } else {
    chin_up_finish = data[0];
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);

  obniz.commandReceive(onCommand);
  obniz.start();

  buzzer.begin();
  Wire.begin(27, 14);
  sensor.setTimeout(500);
  if (!sensor.init()){
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
}

void loop() {
  uint16_t meter = sensor.readRangeContinuousMillimeters();

  if (meter > 2000 ) return;

  ave_meter += meter;
  ave_counter++;
  if (ave_counter >= AVERAGE_DISTANCE) {
    uint16_t tmp_meter = ave_meter / ave_counter;
    ave_counter = 0;
    ave_meter = 0;
    if (tmp_meter > CHIN_UP_DISTANCE_HIGH && chin_up_flg) {
      Serial.println(CHIN_UP_DISTANCE_HIGH);
      old_count++;
      if (old_count < 2) return;
      chin_up_flg = false;
      old_count = 0;
      buzzer.startCountDown();
      sendCommand(chin_up_count, false,chin_up_finish, false);
    } else if (tmp_meter < CHIN_UP_DISTANCE_LOW && !chin_up_flg) {
      Serial.println(CHIN_UP_DISTANCE_LOW);
      old_count++;
      if (old_count < 2) return;
      chin_up_flg = true;
      old_count = 0;
      chin_up_count++;
      if (chin_up_count >= chin_up_finish) {
        sendCommand(chin_up_count, true,chin_up_finish, true);
        buzzer.startCountEnd();
        count_clear();
        delay(5000);
      } else {
        buzzer.startCountUp();
        sendCommand(chin_up_count, false,chin_up_finish, true);
      }
    }
  }
}

void count_clear(){
  chin_up_count = 0;
  old_count = 0;
  chin_up_flg = false;
}

void sendCommand(uint8_t chin_up_count,bool finish_flg,uint8_t chin_up_finish,bool up_flg){
  char message[100];
  sprintf(message,"{\"count\":%d,\"max_count\":%d,\"finish\":%s,\"mode\":""\"%s\"}",chin_up_count, chin_up_finish, finish_flg?"true":"false", up_flg?"up":"down");
  if (!obniz.commandSend((uint8_t *)message, strlen(message))) {
    Serial.printf("send message : %s\n", message);
  }
}