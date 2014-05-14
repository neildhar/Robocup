/*
  CMPS10.h - Library for using CMPS10 Compass with Arduino
  Adapted for current purposes by Swaggy Master Sai Suman.
*/

#ifndef CMPS10_h
#define CMPS10_h

#include <Arduino.h>


  class CMPS10{
    private: 
      TwoWire * i2cBus;
      byte I2C_Address;
      unsigned int bearing;
      byte highByte, lowByte;
    public:
      CMPS10(TwoWire*, byte);
      int read();
  };

#endif
