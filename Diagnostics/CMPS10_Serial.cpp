/*
  SRF10.cpp - Library for using SRF10 Rangefinders with Arduino
  Created by Senor Neil Dhar, April 11, 2014.
*/
#include <Arduino.h>
#include <math.h>
#include "CMPS10_Serial.h";

CMPS10_Serial::CMPS10_Serial(Stream * _bus): serialBus(_bus) {}

CMPS10_Serial::CMPS10_Serial(Stream * _bus, int _xOffset, int _xScale, int _yOffset, int _yScale): serialBus(_bus), xOffset(_xOffset), xScale(_xScale), yOffset(_yOffset), yScale(_yScale) {}

int CMPS10_Serial::read(){
  serialBus->write(byte(0x02));

  highByte = serialBus->read();           
  lowByte = serialBus->read();
  serialBus->read();
  serialBus->read();

  bearing = int(int(highByte<<8)+lowByte)/10;
  return bearing;
}

int CMPS10_Serial::magRead(){
  if(millis()-lastMagReadTime>15){      
    serialBus->write(byte(0x21));
    while(serialBus->available()<6) delayMicroseconds(1);    

    xMagHighByte = serialBus->read();           
    xMagLowByte = serialBus->read();
    yMagHighByte = serialBus->read();           
    yMagLowByte = serialBus->read();
    serialBus->read();
    serialBus->read();
    
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

int CMPS10_Serial::readMagAxis(char axis){
  //serialBus->beginTransmission(I2C_Address);           
  serialBus->write(byte(0x21));                              
  //serialBus->endTransmission();
  
  //serialBus->requestFrom(I2C_Address, 4);
  while(serialBus->available()<6) delayMicroseconds(1);
  xMagHighByte = serialBus->read();           
  xMagLowByte = serialBus->read();
  yMagHighByte = serialBus->read();           
  yMagLowByte = serialBus->read();
  serialBus->read();
  serialBus->read();
  
  xMag = short(xMagHighByte<<8);
  xMag |= xMagLowByte;
  yMag = short(yMagHighByte<<8);
  yMag |= yMagLowByte;
  
  if(axis=='X') return xMag;
  else if(axis=='Y') return yMag;
  else return 0;
}

void CMPS10_Serial::calibrate(){
  while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("Initialized");
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0xF0));
    //serialBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("0 degrees set");
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0xF5));
    //serialBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("90 degrees set");
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0xF5));
    //serialBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("180 degrees set");
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0xF5));
    //serialBus->endTransmission();
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("270 degrees set");
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0xF5));
    //serialBus->endTransmission();
    Serial.println("Calibration Terminated"); 
}

void CMPS10_Serial::factoryReset(){
    while(!Serial.available());
    while(charRead!='a'){
      delay(1);
      byteRead=Serial.read();
      charRead = char(byteRead);
    }
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0x20));
    //serialBus->endTransmission();
    
    delay(100);
    
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0x2A));
    //serialBus->endTransmission();
    
    delay(100);
    
    //serialBus->beginTransmission(I2C_Address);
    serialBus->write(byte(22));
    serialBus->write(byte(0x60));
    //serialBus->endTransmission();
    
    Serial.println("Factory Reset Successful");
}
