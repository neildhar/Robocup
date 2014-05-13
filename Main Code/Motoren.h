/*
  Motoren.h - Library for using Polulu VNH5019 motor driver with Arduino
  Created by Herr Neil Dhar, January 13, 2014.
*/

#ifndef Motoren_h
#define Motoren_h

#include <Arduino.h>

class Motoren{
  private: 
    int INAPin, INBPin, PWMPin;
    boolean flip;
  public:
    Motoren(int, int, int, boolean);
    Motoren(int, int, int);
    boolean setPower(int);
};

#endif
