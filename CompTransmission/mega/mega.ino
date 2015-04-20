/*

Code for arduino MEGA2560 to read serial signal from micro w/ compass sensor attached
Chang Si Yuan 18/4/15

CMPS10 onto micro's i2c bus
TX/RX onto Mega RX1/TX1
Vin/5V+ and GND into Mega Vin and GND

*/

byte hB, lB; // highbyte and lowbyte for serial read()
double cmpRead; //cmp value

void setup() {
  delay(1000);
  Serial.begin(9600);// debug serial
  Serial1.begin(115200); // read from comp serial
  pinMode(2,OUTPUT);
}

void loop() {
  delay(9);
  digitalWrite(2,HIGH);
  delay(1);
  digitalWrite(2,LOW);
  //while(!Serial1.available());
  //Serial.write(Serial1.read());
  
  while(Serial1.available()!=4||Serial1.peek()!='d'){
    //account for serial memory leaks
    if(Serial1.peek()!='d') Serial1.read();
    if(Serial1.available()>4){
       while(Serial1.available()!=4 ) Serial1.read();
    }
  };//wait for serial data to input
  Serial1.read();
  hB = Serial1.read();
  lB = Serial1.read();
  Serial1.read();
  cmpRead = (int(int(hB<<8)+lB)); // parse hb and lb into reading
  cmpRead = cmpRead/100;
  Serial.println(cmpRead); // print reading
}
