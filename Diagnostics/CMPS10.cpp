/*
  SRF10.cpp - Library for using SRF10 Rangefinders with Arduino
  Created by Senor Neil Dhar, April 11, 2014.
*/
#include <Arduino.h>
#include <Wire.h>
#include "CMPS10.h";


CMPS10::CMPS10(TwoWire * _bus, byte Addr): i2cBus(_bus) {
  I2C_Address = Addr >> 1;
}

CMPS10::CMPS10(TwoWire * _bus, byte Addr, int _zero): i2cBus(_bus) {
  I2C_Address = Addr >> 1;
  zeroValue = _zero;
}

int CMPS10::read(){
  i2cBus->beginTransmission(I2C_Address);           //starts communication with CMPS10
  i2cBus->write(byte(0x02));                              //Sends the register we wish to start reading from
  i2cBus->endTransmission();

  i2cBus->requestFrom(I2C_Address, 4);              // Request 4 bytes from CMPS10
  highByte = i2cBus->read();           
  lowByte = i2cBus->read();
  i2cBus->read();
  i2cBus->read();

  bearing = int(int(highByte<<8)+lowByte)/10;
  bearing = bearing>=zeroValue?bearing-zeroValue:360-zeroValue+bearing;
  return bearing;
}
