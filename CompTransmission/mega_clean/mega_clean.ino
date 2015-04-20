/*

Code for arduino MEGA2560 to read serial signal from micro w/ compass sensor attached
Chang Si Yuan 18/4/15

CMPS10 onto micro's i2c bus
TX/RX onto Mega RX1/TX1
Vin/5V+ and GND into Mega Vin and GND

*/

//Declare pins
#define callPin 2 //pin connected to arduino micro's int4/D7 pin

//Declare baud
#define SerialBaud 9600 //baud to computer terminal
#define Serial1Baud 115200 //baud to micro

//declare variables
byte hB, lB; //highbyte and lowbyte
double compassRead; //compass value

void setup(){
  Serial.begin(SerialBaud);
  Serial1.begin(Serial1Baud);
  pinMode(callPin,OUTPUT);
}

void loop(){
  
  //trigger interrupt
  delayMicroseconds(9000);
  digitalWrite(callPin,HIGH);
  delayMicroseconds(1000);
  digitalWrite(callPin,LOW);
  
  //wait for serial data to input
  while(Serial1.available()!=4||Serial1.peek()!='d'){
    //ensure that buffer has string of 4 bytes, starts with 'd' header
    if(Serial1.peek()!='d') Serial1.read(); //clear front if not 'd'
    while(Serial1.available()>4 ) Serial1.read(); //clear if longer than 4;
    //loop if less than 4
    delayMicroseconds(1);
  }
  
  //read serial data
  Serial1.read();
  hB = Serial1.read();
  lB = Serial1.read();
  Serial1.read();
  
  //parse bytes to bearing
  compassRead = (int(int(hB<<8)+lB));
  compassRead = compassRead/100;
  
  //Output to Serial monitor
  Serial.println(compassRead);
}
