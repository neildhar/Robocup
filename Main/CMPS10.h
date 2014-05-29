/*
  SRF10.h - Library for using SRF10 Rangefinders with Arduino
  Adapted for current purposes by Swaggy Master Sai Suman.
*/

#ifndef CMPS10_h
#define CMPS10_h

#include <Arduino.h>

  class CMPS10{
    private: 
      TwoWire * i2cBus;
      byte I2C_Address, byteRead;
      char charRead;
      unsigned int bearing;
      signed short xMag, yMag;
      double realXMag, realYMag;
      double magBearing;
      byte highByte, lowByte;
      byte xMagHighByte, xMagLowByte, yMagHighByte, yMagLowByte;
      int xOffset, xScale;
      int yOffset, yScale;
    public:
      CMPS10(TwoWire*, byte);
      CMPS10(TwoWire*, byte, int, int, int, int);
      int read();
      int magRead();
      void calibrate();
      void factoryReset();
      int readMagAxis(char);
  };

#endif
