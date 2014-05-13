#include <Wire.h>
#include "Motoren.h"
#include "JoinMaxSensors.h"
#include <math.h>
#include "SRF10.h"
JM::compoundEye front_CE(&Wire);
JM::compoundEye back_CE(&Wire1);
SRF10 right_US(&Wire,0xEA);
SRF10 left_US(&Wire, 0xEC);
SRF10 back_US(&Wire1, 0xEE);
JM::compassSensor compass(&Wire1);
Motoren SEMotor(30,31,2);
Motoren SWMotor(32,33,3,1);
Motoren NEMotor(34,35,4);
Motoren NWMotor(36,37,5,1);

void setup() {
  Wire.begin();
  Wire1.begin();
  Serial.begin(9600);
  left_US.setGain(0x05);
  left_US.setRange(4);
  right_US.setGain(0x05);
  right_US.setRange(4);
  back_US.setGain(0x05);
  back_US.setRange(4);
  
}
int reading;
void loop() {
    Serial.println(left_US.read());
}
