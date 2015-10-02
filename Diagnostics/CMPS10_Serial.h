/*
  CMPS10_Serial.cpp - Library for using CMPS10 compass with Arduino over Serial
  Created by Senor Neil Dhar, April 11, 2014.
*/

#ifndef CMPS10_Serial_h
#define CMPS10_Serial_h

#include <Arduino.h>

  class CMPS10_Serial{
    private: 
      HardwareSerial * serialBus;
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
      bool sentReq;
      unsigned long long lastMagReadTime;
    public:
      CMPS10_Serial(HardwareSerial*);
      CMPS10_Serial(HardwareSerial*, int, int, int, int);
      void setBaud(int);
      int read();
      int magRead();//normal read
      void magRead1();//send read req
      int magRead2();//read after req
      void calibrate();
      void factoryReset();
      int readMagAxis(char);
  };

#endif
