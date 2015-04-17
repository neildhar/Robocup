/*
  Motoren.cpp - Library for using Polulu VNH5019 motor driver with Arduino
  Created by Herr Neil Dhar, January 13, 2014.
*/

#include <Arduino.h>
#include "Motoren.h";




Motoren::Motoren(int _INA, int _INB, int _PWM, boolean _flip) : INAPin(_INA), INBPin(_INB), PWMPin(_PWM), flip(_flip) {
  pinMode(INAPin, OUTPUT);
  pinMode(INBPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}
Motoren::Motoren(int _INA, int _INB, int _PWM) : INAPin(_INA), INBPin(_INB), PWMPin(_PWM), flip(0) {
  pinMode(INAPin, OUTPUT);
  pinMode(INBPin, OUTPUT);
  pinMode(PWMPin, OUTPUT);
}
boolean Motoren::setPower(int Power){
  if(Power>=0){
  	analogWrite(PWMPin, Power);
  	digitalWrite(INAPin, 1-flip);
  	digitalWrite(INBPin, flip);
  	return 1;
  }
  else{
    analogWrite(PWMPin, -Power);
    digitalWrite(INAPin, flip);
    digitalWrite(INBPin, 1-flip);
    return 1;
  }
}
