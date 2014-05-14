/*
  CMPS10.cpp - Library for using CMPS10 Compass with Arduino
  Created by Senor Neil Dhar, April 11, 2014.
*/
#include <Arduino.h>
#include <Wire.h>
#include "CMPS10.h";


CMPS10::CMPS10(TwoWire * _bus, byte Addr): i2cBus(_bus) {
  I2C_Address = Addr >> 1;
}

int CMPS10::read(){
  i2cbus->beginTransmission(I2C_ADDRESS);           //starts communication with CMPS10
  i2cbus->write(byte(0x02));                              //Sends the register we wish to start reading from
  i2cbus->endTransmission();

  i2cbus->requestFrom(I2C_ADDRESS, 4);              // Request 4 bytes from CMPS10
  highByte = i2cbus->read();           
  lowByte = i2cbus->read();
  i2cbus->read();
  i2cbus->read();

  bearing = int(int(highByte<<8)+lowByte)/10;
  return bearing;
}