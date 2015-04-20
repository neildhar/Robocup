/*

Code for arduino micro to read cmps10 and send analog signal to MEGA2560
Chang Si Yuan 18/4/15

Running on Mega32u4, rated at 1khz

CMPS10 onto micro's i2c bus
TX/RX onto Mega RX1/TX1
Vin/5V+ and GND into Mega Vin and GND

*/

#include <math.h> //for atan2
#include <Wire.h> //for i2c with cmps10

//Declare Pins
#define intPin 4 //interrupt pin, int.4 -> D7 pin
#define ledPin 13 //debug LED pin, D13

//Declare baud
#define SerialBaud 115200 //can change to other values

//Compass Calibrations
#define xOffset 327
#define yOffset -119
#define xScale 39
#define yScale 42

//Declare Variables
byte hB,lB; //highByte and lowByte storage values
int xRead,yRead; //raw magnometer data
double xMag, yMag; //compensated magnetometer data
double compassRead; //compass bearing reading
int compassReadInt; //bearing data that is sent thru serial
volatile bool toOutput; //trigger for serial output

void setup(){
  //Initiate i2c and Serial
  Wire.begin();
  Serial1.begin(SerialBaud);
  
  //Initiate pins
  pinMode(intPin,INPUT_PULLUP); // int.4 input mode
  attachInterrupt(intPin,outputSerial,FALLING); //attach int.4 
  pinMode(ledPin,OUTPUT); //debug led
  digitalWrite(ledPin,LOW);
  
  //disable serial output first loop
  toOutput = false;
  
  //Wait for serial
  while(!Serial1);
  digitalWrite(ledPin,HIGH); //turn on led when loop starts
}

void loop(){
  
  //Request i2c data
  Wire.beginTransmission(byte(96));
  Wire.write(byte(0x0a));
  Wire.endTransmission();
  Wire.requestFrom(96, 4);
  
  //read x and y data
  hB = Wire.read();           
  lB = Wire.read();
  xRead = int(int(hB<<8)+lB);
  hB = Wire.read();           
  lB = Wire.read();
  yRead = int(int(hB<<8)+lB);
  
  //offset and scale
  xMag = (xRead-xOffset)*xScale;
  yMag = (yRead-yOffset)*yScale;
  
  //calcuate bearing
  compassRead = 180-atan2(xMag,yMag)*180/PI;
  
  if(toOutput){
    //Begin serial output
    //Calculate data to output, 5sf
    compassReadInt = compassRead*100;
    Serial1.write("d"); // output begin marker
    
    //output highbyte
    hB = compassReadInt/256; 
    Serial1.write(hB);
    
    //output lowbyte
    lB = compassReadInt%256;
    Serial1.write(lB);
    
    //end serial data marker
    Serial1.write("\n");
    
    //disable output next cycle
    toOutput = false;
  }
}

void outputSerial(){
  //enable output when interrupt is triggered
  toOutput = true;
}
