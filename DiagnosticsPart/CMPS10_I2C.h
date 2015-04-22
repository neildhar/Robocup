/*
  CMPS10_I2C.h - Library for using CMPS10 Compass with Arduino over I2C
*/

#ifndef CMPS10_I2C_h
#define CMPS10_I2C_h

#include <Arduino.h>

  class CMPS10_I2C{
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
      unsigned long long lastMagReadTime;
    public:
      CMPS10_I2C(TwoWire*, byte);
      CMPS10_I2C(TwoWire*, byte, int, int, int, int);
      int read();
      int magRead();
      void calibrate();
      void factoryReset();
      int readMagAxis(char);
  };

#endif
