/*

Code for arduino micro to read cmps10 and send analog signal to MEGA2560
Chang Si Yuan 18/4/15

Running on Mega32u4, rated at 1khz

CMPS10 onto micro's i2c bus
TX/RX onto Mega RX1/TX1
Vin/5V+ and GND into Mega Vin and GND

*/

#include <math.h>
#include <Wire.h>

double cmpRead; //actual bearing
long int i_cmpRead; //int storage
int loopno=0;
int maxRead=300, minRead=300; // used in calbiration, change to find max and minimum read values
long int xRead=0, yRead=0; // stores raw read data
double xMag, yMag; // stores scaled read data

int pwmPin = 7; //pin for reading pwm output
int intPin = 4; //int pin for reading interrupt data (int.4 -> pin 7)
int ledPin = 13; //debug ledpin for cycle rating
volatile bool toOutput = false; //interrupt var
byte hB, lB; // highbyte and lowbyte for i2c read()
int Serial1Debug = 3; // 0 for disabled, 1 for read cmpRead, 2 for debug calculation values, 3 for int
int readMode = 3; // 0=PWM, 1=I2C main, 2=I2C XY Calbirate 3=I2C XY Read

/* Calibrate Values
X max       582     min     71       range 511
  offset    327     scale   39      
Y max       114     min     -352     range 466
  offset    -119    scale   42      
T max       +-1000
*/

//offsets are subtracted from read, scales are multiplied from read
const int xOffset = 327, yOffset = -119, xScale = 39, yScale = 42;

void setup(){
  delay(3000);
  //pwm or wire
  if(readMode==0) pinMode(pwmPin,INPUT);
  else Wire.begin();
  //setup interrupt
  pinMode(intPin,INPUT_PULLUP);
  attachInterrupt(intPin,outputSerial1,FALLING);
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
  //setup Serial1
  if(Serial1Debug==1||Serial1Debug==2||Serial1Debug==3) Serial1.begin(115200); //hardware serial, not usb serial (whyy micro whyyy), usb serial uses Serial.
  while(!Serial1);
  
}

void loop(){
  loopno++;
  if(loopno==10000) digitalWrite(ledPin,HIGH); // turn on led at 1000th cycle
  if(loopno==20000){
    digitalWrite(ledPin,LOW);
    loopno=0;
  } //turn off at 2000th cycle, repeat
  if(readMode==0){
    cmpRead = pulseIn(pwmPin, HIGH); // obtain pwm pulse valuel
    cmpRead -= 1000; // compensate for 1ms delay
    cmpRead /= 100;  // microseconds to degrees
  } else if (readMode==1){
    //default read
    Wire.beginTransmission(byte(96));
    Wire.write(byte(0x02));
    Wire.endTransmission();
    Wire.requestFrom(96, 2);
    hB = Wire.read();           
    lB = Wire.read();
    cmpRead = int(int(hB<<8)+lB)/10;
  } else if (readMode==2){
    //read one dimension x or y
    Wire.beginTransmission(byte(96));
    Wire.write(byte(0x0a)); //0x0a for x, 0x0c for y
    Wire.endTransmission();
    Wire.requestFrom(96, 2);
    hB = Wire.read();           
    lB = Wire.read();
    cmpRead = int(int(hB<<8)+lB);
    if(maxRead<cmpRead) maxRead = cmpRead;
    if(minRead>cmpRead) minRead = cmpRead;
    //display max and minimum
    if(Serial1Debug==2){
      Serial1.print(maxRead);
      Serial1.print("\t");
      Serial1.print(minRead);
      Serial1.print("\t");
    }
  } else if (readMode==3){
    Wire.beginTransmission(byte(96));
    Wire.write(byte(0x0a));
    Wire.endTransmission();
    Wire.requestFrom(96, 4);
    //read x and y
    hB = Wire.read();           
    lB = Wire.read();
    xRead = int(int(hB<<8)+lB);
    hB = Wire.read();           
    lB = Wire.read();
    yRead = int(int(hB<<8)+lB);
    //offset and scale
    xMag = (xRead-xOffset)*xScale;
    yMag = (yRead-yOffset)*yScale;
    if(Serial1Debug==2){
      Serial1.print(xMag/100);
      Serial1.print("\t");
      Serial1.print(yMag/100);
      Serial1.print("\t");
    }
    cmpRead = atan2(xMag,yMag)*180/PI+180;
  }
  if(Serial1Debug==1||Serial1Debug==2){
    Serial1.print(cmpRead);
  }
  //Serial1.println(toOutput);
  if(toOutput){
    //Serial1.println(cmpRead);
    i_cmpRead = cmpRead*100;
    Serial1.write("d");
    hB = i_cmpRead/256;
    Serial1.write(hB);
    lB = i_cmpRead%256;
    Serial1.write(lB);
    Serial1.write("\n");
    toOutput = false;
  }
  //Serial1.println(digitalRead(7));
}

void outputSerial1(){
  toOutput = true;
}
