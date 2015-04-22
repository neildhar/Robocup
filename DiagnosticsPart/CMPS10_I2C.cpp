/*
  SRF10.cpp - Library for using SRF10 Rangefinders with Arduino
  Created by Senor Neil Dhar, April 11, 2014.
*/
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "CMPS10_I2C.h";


CMPS10_I2C::CMPS10_I2C(TwoWire * _bus, byte Addr): i2cBus(_bus) {
  I2C_Address = Addr >> 1;
}

CMPS10_I2C::CMPS10_I2C(TwoWire * _bus, byte Addr, int _xOffset, int _xScale, int _yOffset, int _yScale): i2cBus(_bus), xOffset(_xOffset), xScale(_xScale), yOffset(_yOffset), yScale(_yScale) {
  I2C_Address = Addr >> 1;
}

int CMPS10_I2C::read(){
  i2cBus->beginTransmission(I2C_Address);           //starts communication with CMPS10
  i2cBus->write(byte(0x02));                              //Sends the register we wish to start reading from
  i2cBus->endTransmission();

  i2cBus->requestFrom(I2C_Address, 4);              // Request 4 bytes from CMPS10
  highByte = i2cBus->read();           
  lowByte = i2cBus->read();
  i2cBus->read();
  i2cBus->read();

  bearing = int(int(highByte<<8)+lowByte)/10;
  return bearing;
}

int CMPS10_I2C::magRead(){
  if(millis()-lastMagReadTime>15){
    i2cBus->beginTransmission(I2C_Address);           
    i2cBus->write(byte(10));                              
    i2cBus->endTransmission();
    
    i2cBus->requestFrom(I2C_Address, 4);
    xMagHighByte = i2cBus->read();           
    xMagLowByte = i2cBus->read();
    yMagHighByte = i2cBus->read();           
    yMagLowByte = i2cBus->read();
    
    xMag = short(xMagHighByte<<8);
    xMag |= xMagLowByte;
    yMag = short(yMagHighByte<<8);
    yMag |= yMagLowByte;
    
    realXMag = double(double(xMag+xOffset)/xScale);
    realYMag = double(double(yMag+yOffset)/yScale);
    
    if(realYMag!=0){
      magBearing = (atan2(realXMag,realYMag)*180.0)/3.141592654;
    }
    else if(realXMag>0) magBearing=0.0;
    else if(realYMag<0) magBearing=180.0;
    else magBearing = 1000;
    if(magBearing < 0) magBearing+=360;
    magBearing=360-magBearing;
    lastMagReadTime=millis();
  }
  return magBearing;
}

int CMPS10_I2C::readMagAxis(char axis){
  i2cBus->beginTransmission(I2C_Address);           
  i2cBus->write(byte(10));                              
  i2cBus->endTransmission();
  
  i2cBus->requestFrom(I2C_Address, 4);
  xMagHighByte = i2cBus->read();           
  xMagLowByte = i2cBus->read();
  yMagHighByte = i2cBus->read();           
  yMagLowByte = i2cBus->read();
  
  xMag = short(xMagHighByte<<8);
  xMag |= xMagLowByte;
  yMag = short(yMagHighByte<<8);
  yMag |= yMagLowByte;
  
  if(axis=='X') return xMag;
  else if(axis=='Y') return yMag;
  else return 0;
}

void CMPS10_I2C::calibrate(){
  while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("Initialized");
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0xF0));
    i2cBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("0 degrees set");
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0xF5));
    i2cBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("90 degrees set");
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0xF5));
    i2cBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("180 degrees set");
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0xF5));
    i2cBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("270 degrees set");
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0xF5));
    i2cBus->endTransmission();
    Serial.println("Calibration Terminated"); 
}

void CMPS10_I2C::factoryReset(){
    while(!Serial.available());
    while(charRead!='a'){
      delay(1);
      byteRead=Serial.read();
      charRead = char(byteRead);
    }
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0x20));
    i2cBus->endTransmission();
    
    delay(100);
    
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0x2A));
    i2cBus->endTransmission();
    
    delay(100);
    
    i2cBus->beginTransmission(I2C_Address);
    i2cBus->write(byte(22));
    i2cBus->write(byte(0x60));
    i2cBus->endTransmission();
    
    Serial.println("Factory Reset Successful");
}
