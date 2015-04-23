/*
  SRF10.cpp - Library for using SRF10 Rangefinders with Arduino
  Created by Senor Neil Dhar, April 11, 2014.
*/
#include <Arduino.h>
#include <math.h>
#include "CMPS10_Serial.h";

CMPS10_Serial::CMPS10_Serial(HardwareSerial * _bus): serialBus(_bus) {}

CMPS10_Serial::CMPS10_Serial(HardwareSerial * _bus, int _xOffset, int _xScale, int _yOffset, int _yScale): serialBus(_bus), xOffset(_xOffset), xScale(_xScale), yOffset(_yOffset), yScale(_yScale) {}

//Initialize serial, CMPS10 over Serial and increase baudrate
//though it doesnt actuallly help with the latency between requests
void CMPS10_Serial::init(int baudRate){
  serialBus->begin(9600);
  delayMicroseconds(1000);
  serialBus->write(0xA1);
  delayMicroseconds(1000);
  serialBus->end();
  serialBus->begin(baudRate);
  delayMicroseconds(1000);
  serialBus->read();
}

//Basic Tilt-compensated 2 byte read
int CMPS10_Serial::read(){
  serialBus->write(byte(0x13));
  
  //Wait for Serial Buffer
  while(serialBus->available()<2) delayMicroseconds(1); 

  highByte = serialBus->read();           
  lowByte = serialBus->read();

  bearing = int(int(highByte<<8)+lowByte)/10;
  return bearing;
}

//Raw Read to Bearing
int CMPS10_Serial::magRead(){
  if(millis()-lastMagReadTime>15){      
    serialBus->write(byte(0x21));
    
    //Wait for Serial Buffer
    while(serialBus->available()<6){
      delayMicroseconds(1);    
      //Serial.println("Waiting");
    }

    xMagHighByte = serialBus->read();//high x           
    xMagLowByte = serialBus->read();//low x
    yMagHighByte = serialBus->read();//high y           
    yMagLowByte = serialBus->read();//low y
    serialBus->read();//high z
    serialBus->read();//low z
    
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

void CMPS10_Serial::magRead1(){
  if(millis()-lastMagReadTime>15){      
    serialBus->write(byte(0x21));
    sentReq = true;
  }
  return;
}

int CMPS10_Serial::magRead2(){
  if(sentReq){
    //Wait for Serial Buffer
    while(serialBus->available()<6){
      delayMicroseconds(1);    
      //Serial.println("Waiting");
    }

    xMagHighByte = serialBus->read();//high x           
    xMagLowByte = serialBus->read();//low x
    yMagHighByte = serialBus->read();//high y           
    yMagLowByte = serialBus->read();//low y
    serialBus->read();//high z
    serialBus->read();//low z
    
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

//
int CMPS10_Serial::readMagAxis(char axis){
  
  serialBus->write(byte(0x21));
  
  //Wait for buffer to fill
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

//Calibration function
void CMPS10_Serial::calibrate(){
  while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("Initializing");
    
    //init caibration mode 0x31,0x45,0x5A,
    delay(150);
    serialBus->write(byte(0x31));
    delay(150);
    serialBus->write(byte(0x45));
    delay(150);
    serialBus->write(byte(0x5A));
    delay(150);
    
    Serial.println("Initialized");
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("0 degrees set");
    serialBus->write(byte(0x5E));
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("90 degrees set");
    serialBus->write(byte(0x5E));
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("180 degrees set");
    serialBus->write(byte(0x5E));
    
    while(!Serial.available());
    byteRead=Serial.read();
    charRead = char(byteRead);
    Serial.println("Awaiting input");
    while(charRead!='a');
    Serial.println("270 degrees set");
    serialBus->write(byte(0x5E));
    Serial.println("Calibration Terminated"); 
}

void CMPS10_Serial::factoryReset(){
    while(!Serial.available());
    while(charRead!='a'){
      delay(1);
      byteRead=Serial.read();
      charRead = char(byteRead);
    }
    
    //send 0x6A,0x7C,0x81.
    delay(150);
    serialBus->write(byte(0x6A));
    delay(150);
    serialBus->write(byte(0x7C));
    delay(150);
    serialBus->write(byte(0x81));
    delay(150);
    
    Serial.println("Factory Reset Successful");
}
