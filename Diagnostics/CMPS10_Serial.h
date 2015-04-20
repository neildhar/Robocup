/*
  CMPS10_Serial.cpp - Library for using CMPS10 Compass with Arduino over Serial
*/

#ifndef CMPS10_Serial_h
#define CMPS10_Serial_h

#include <Arduino.h>

  class CMPS10_Serial{
    private: 
      Stream * serialBus;
      byte byteRead;
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
      CMPS10_Serial(Stream*);
      CMPS10_Serial(Stream*, int, int, int, int);
      int read();
      int magRead();
      void calibrate();
      void factoryReset();
      int readMagAxis(char);
  };

#endif
