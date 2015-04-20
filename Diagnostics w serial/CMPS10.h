/*
  SRF10.h - Library for using SRF10 Rangefinders with Arduino
*/

#ifndef CMPS10_h
#define CMPS10_h

#include <Arduino.h>

  class CMPS10{
    private: 
      Stream * i2cBus;
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
      CMPS10(Stream*, byte);
      CMPS10(Stream*, byte, int, int, int, int);
      int read();
      int magRead();
      void calibrate();
      void factoryReset();
      int readMagAxis(char);
  };

#endif
