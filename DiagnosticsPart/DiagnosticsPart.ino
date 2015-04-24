#include <Wire.h>
#include "Motoren.h"
#include "JoinMaxSensors.h"
#include <math.h>
#include "SRF10.h"
#include "CMPS10_I2C.h"
#include "CMPS10_Serial.h"

#define compassYOffset 155
#define compassYScale 355
#define compassXOffset -396
#define compassXScale 375
#define ambientIR 15
#define BA_middle 50 
#define BA_back 50
#define backOffset 20
#define lAimzone 45
#define rAimzone 118
#define MPS 1

//Kc ~ 1400
//Kp ~ 0.9
//Pc ~ 0.08

#define CA_Offset 188
#define CA_Kp 1000
#define CA_Ki 0
#define CA_Kd 1700
#define CA_factor 1000

#define BA_Kp 0.71
#define kAOG 180

#define VD_maxSpeed 200

JM::compoundEye back_CE(&Wire1);
JM::compoundEye front_CE(&Wire);
SRF10 right_US(&Wire1, 0xEA);
SRF10 left_US(&Wire1, 0xEC);
SRF10 back_US(&Wire, 0xEE);

Motoren SEMotor(30,31,2);
Motoren SWMotor(32,33,3,1);
Motoren NWMotor(34,35,4,1);
Motoren NEMotor(36,37,7); //5,6 are not used as there is interference from the internal clock


//Compass Selection

//CMPS10_I2C compass(&Wire1, 0xC0, compassXOffset, compassXScale, compassYOffset, compassYScale);
CMPS10_Serial compass(&Serial1, compassXOffset, compassXScale, compassYOffset, compassYScale);
//JM::compassSensor compass(&Wire1);

//Declare Variables
byte byteRead;
char charRead;
int compassValue, NEPower, SEPower, NWPower, SWPower;
int CA_adjusted, CA_integral, CA_derivative, CA_lastRead;
int trueMagVal=189, counter, BT_ballPos, BT_ballDist, BT_dirMod, BT_clockPort, BT_anticlockPort, BT_bulb;
int xPos, yPos, BT_fIRValue, BT_bIRValue, VD_bearing, VD_compassSpeed;
const double  BT_aimKp = 90/195;//CA_Kp=double(400/180),
double VD_multiplier,VD_speed, VD_motorPowerScaler;
bool leftZone, centreZone;

int VectorDynamics[361][2]={{255,255},{255,246},{255,238},{255,230},{255,222},{255,214},{255,206},{255,199},{255,192},{255,185},{255,179},{255,172},{255,166},{255,159},{255,153},{255,147},{255,141},{255,136},{255,130},{255,124},{255,119},{255,114},{255,108},{255,103},{255,98},{255,93},{255,88},{255,83},{255,78},{255,73},{255,68},{255,64},{255,59},{255,54},{255,50},{255,45},{255,40},{255,36},{255,31},{255,27},{255,22},{255,18},{255,13},{255,9},{255,4},{255,0},{255,-4},{255,-9},{255,-13},{255,-18},{255,-22},{255,-27},{255,-31},{255,-36},{255,-40},{255,-45},{255,-50},{255,-54},{255,-59},{255,-64},{255,-68},{255,-73},{255,-78},{255,-83},{255,-88},{255,-93},{255,-98},{255,-103},{255,-108},{255,-114},{255,-119},{255,-124},{255,-130},{255,-136},{255,-141},{255,-147},{255,-153},{255,-159},{255,-166},{255,-172},{255,-179},{255,-185},{255,-192},{255,-199},{255,-206},{255,-214},{255,-222},{255,-230},{255,-238},{255,-246},{255,-255},{246,-255},{238,-255},{230,-255},{222,-255},{214,-255},{206,-255},{199,-255},{192,-255},{185,-255},{179,-255},{172,-255},{166,-255},{159,-255},{153,-255},{147,-255},{141,-255},{136,-255},{130,-255},{124,-255},{119,-255},{114,-255},{108,-255},{103,-255},{98,-255},{93,-255},{88,-255},{83,-255},{78,-255},{73,-255},{68,-255},{64,-255},{59,-255},{54,-255},{50,-255},{45,-255},{40,-255},{36,-255},{31,-255},{27,-255},{22,-255},{18,-255},{13,-255},{9,-255},{4,-255},{0,-255},{-4,-255},{-9,-255},{-13,-255},{-18,-255},{-22,-255},{-27,-255},{-31,-255},{-36,-255},{-40,-255},{-45,-255},{-50,-255},{-54,-255},{-59,-255},{-64,-255},{-68,-255},{-73,-255},{-78,-255},{-83,-255},{-88,-255},{-93,-255},{-98,-255},{-103,-255},{-108,-255},{-114,-255},{-119,-255},{-124,-255},{-130,-255},{-136,-255},{-141,-255},{-147,-255},{-153,-255},{-159,-255},{-166,-255},{-172,-255},{-179,-255},{-185,-255},{-192,-255},{-199,-255},{-206,-255},{-214,-255},{-222,-255},{-230,-255},{-238,-255},{-246,-255},{-255,-255},{-255,-246},{-255,-238},{-255,-230},{-255,-222},{-255,-214},{-255,-206},{-255,-199},{-255,-192},{-255,-185},{-255,-179},{-255,-172},{-255,-166},{-255,-159},{-255,-153},{-255,-147},{-255,-141},{-255,-136},{-255,-130},{-255,-124},{-255,-119},{-255,-114},{-255,-108},{-255,-103},{-255,-98},{-255,-93},{-255,-88},{-255,-83},{-255,-78},{-255,-73},{-255,-68},{-255,-64},{-255,-59},{-255,-54},{-255,-50},{-255,-45},{-255,-40},{-255,-36},{-255,-31},{-255,-27},{-255,-22},{-255,-18},{-255,-13},{-255,-9},{-255,-4},{-255,0},{-255,4},{-255,9},{-255,13},{-255,18},{-255,22},{-255,27},{-255,31},{-255,36},{-255,40},{-255,45},{-255,50},{-255,54},{-255,59},{-255,64},{-255,68},{-255,73},{-255,78},{-255,83},{-255,88},{-255,93},{-255,98},{-255,103},{-255,108},{-255,114},{-255,119},{-255,124},{-255,130},{-255,136},{-255,141},{-255,147},{-255,153},{-255,159},{-255,166},{-255,172},{-255,179},{-255,185},{-255,192},{-255,199},{-255,206},{-255,214},{-255,222},{-255,230},{-255,238},{-255,246},{-255,255},{-246,255},{-238,255},{-230,255},{-222,255},{-214,255},{-206,255},{-199,255},{-192,255},{-185,255},{-179,255},{-172,255},{-166,255},{-159,255},{-153,255},{-147,255},{-141,255},{-136,255},{-130,255},{-124,255},{-119,255},{-114,255},{-108,255},{-103,255},{-98,255},{-93,255},{-88,255},{-83,255},{-78,255},{-73,255},{-68,255},{-64,255},{-59,255},{-54,255},{-50,255},{-45,255},{-40,255},{-36,255},{-31,255},{-27,255},{-22,255},{-18,255},{-13,255},{-9,255},{-4,255},{0,255},{4,255},{9,255},{13,255},{18,255},{22,255},{27,255},{31,255},{36,255},{40,255},{45,255},{50,255},{54,255},{59,255},{64,255},{68,255},{73,255},{78,255},{83,255},{88,255},{93,255},{98,255},{103,255},{108,255},{114,255},{119,255},{124,255},{130,255},{136,255},{141,255},{147,255},{153,255},{159,255},{166,255},{172,255},{179,255},{185,255},{192,255},{199,255},{206,255},{214,255},{222,255},{230,255},{238,255},{246,255},{255,255}};

void setup() {
  delay(1000);
  Wire.begin();
  Wire1.begin();
  Serial.begin(9600);
  compass.init(38400); // init function begins serial as well
  //Unfortunately increasing baud doesnt seem to negate ~20us read lag
  
  left_US.setGain(0x05);
  left_US.setRange(4);
  right_US.setGain(0x05);
  right_US.setRange(4);
  back_US.setGain(0x05);
  back_US.setRange(4);
  
  
}

void loop() {
    //Start of Compass Align
    compassValue=compass.magRead();
    Serial.print("C:");
    Serial.print(compassValue);    
    compassValue=compassValue-CA_Offset;
    if(compassValue<0){
      compassValue=compassValue+360;
    }
    Serial.print("\tA:");
    Serial.print(compassValue);  
    
    /* Basic Compass
    if(compassValue<=2||compassValue>=358) NEPower=SEPower=NWPower=SWPower=0;
    else if (compassValue>180) CA_speed = -(CA_Kp)*(360-compassValue);
    else CA_speed = (CA_Kp)*compassValue;
    */
    //PID Compass
    /*
    if(compassValue<=2||compassValue>=358){
         NEPower=SEPower=NWPower=SWPower=0;
    }
    
    else{
      if(compassValue<180){//rotates anticlock //case compasslast = 30, compassvalue = 25; 
      CA_correction=(compassValue-CA_lastError)*CA_Kd+compassValue*CA_Kp;
      CA_lastError=compassValue;
      NEPower=SEPower=(CA_correction/8)+4;
      NWPower=SWPower=(CA_correction/8*(-1))-4;
      negNESE=-NEPower;
      negNWSW=-NWPower;  
      VD_compound++;    
    }
      else{
        CA_correction=((360-compassValue)-CA_lastError)*CA_Kd+(360-compassValue)*CA_Kp;
        CA_lastError=(360-compassValue);
        NEPower=SEPower=((CA_correction/8)*(-1)-4);
        NWPower=SWPower=(CA_correction/8)+4;
        negNESE=-NEPower;
        negNWSW=-NWPower;
        VD_compound++;
      }
    }
    */
    //PID compass v2
    if(compassValue<180){
    	//rotate anticlockwise
    	CA_adjusted = compassValue;
    	CA_integral += CA_adjusted; // calculate integral
    	CA_derivative = CA_adjusted - CA_lastRead;// calculate derivative
    	CA_lastRead = CA_adjusted;
    } else {
  	//rotate clockwise
  	CA_adjusted = compassValue-360;
  	CA_integral += CA_integral; // calculate integral
  	CA_derivative = CA_adjusted - CA_lastRead; // calculate derivative
  	CA_lastRead = CA_adjusted;
    }
    CA_adjusted = (CA_Kp*CA_adjusted + CA_Ki*CA_integral + CA_Kd*CA_derivative)/ CA_factor;
    if(CA_adjusted > 180){
      CA_adjusted = 180;
    } else if (CA_adjusted < -180){
      CA_adjusted = -180;
    }
    if(compassValue<=3||compassValue>=357){
      NEPower=SEPower=NWPower=SWPower=0;
      VD_compassSpeed = 0;
    } else {
      NEPower = SEPower = CA_adjusted;
      NWPower = SWPower = -CA_adjusted;
      VD_compassSpeed = CA_adjusted;
      if(VD_compassSpeed<0) VD_compassSpeed = -VD_compassSpeed;
    }

  
  //xPos = left_US.read();
  //Serial.println(back_US.read());
  //yPos = xPos<rAimzone||xPos>lAimzone?back_US.read()+20:back_US.read();
  BT_fIRValue=front_CE.highestValue(); //BT_fIRValue /=100;
  
  Serial.print("\tF:");
  Serial.print(BT_fIRValue);
  BT_bIRValue=back_CE.highestValue(); //BT_bIRValue /=100;
  
  Serial.print("\tB:");
  Serial.print(BT_bIRValue);
  if(BT_fIRValue>ambientIR||BT_bIRValue>ambientIR){ //Check if ball is on field
    //Start of Ball Tracking
    VD_speed=200;
    if(BT_fIRValue>BT_bIRValue){
      BT_bulb = front_CE.highestBulb();
      BT_ballDist = BT_fIRValue; 
      BT_ballPos = 240+(30*BT_bulb);
      BT_clockPort = BT_bulb!=7?front_CE.readBulb(BT_bulb+1):back_CE.readBulb(1);
      BT_anticlockPort = BT_bulb!=1?front_CE.readBulb(BT_bulb-1):back_CE.readBulb(7);  
    }
    else{
      BT_bulb = back_CE.highestBulb();
      BT_ballDist = BT_bIRValue;
      BT_ballPos = 60+(30*BT_bulb);
      BT_clockPort = BT_bulb!=7?back_CE.readBulb(BT_bulb+1):front_CE.readBulb(1);
      BT_anticlockPort = BT_bulb!=1?back_CE.readBulb(BT_bulb-1):front_CE.readBulb(7);  
    }
    BT_ballPos = (((BT_clockPort-BT_anticlockPort)/6)+BT_ballPos)%360; //Determine Ball Position
    //Serial.println(BT_ballPos);
    
    if ( BT_ballPos<=90 ) BT_dirMod=BT_ballPos;
    else if(BT_ballPos>=270) BT_dirMod=360-BT_ballPos;
    else BT_dirMod=80;
    
    if ( BT_ballPos<=180&&BT_ballPos>=15 ) VD_bearing=BT_ballPos+(BT_dirMod*BT_ballDist)/kAOG;
    else if ( BT_ballPos<=345&&BT_ballPos>180 ) VD_bearing=BT_ballPos-(BT_dirMod*BT_ballDist)/kAOG;
    else VD_bearing=BT_ballPos;
    VD_bearing=BT_ballPos;
    Serial.print("\tPos:");
    Serial.print(VD_bearing);
    
    if ( (BT_ballPos>345||BT_ballPos<15) && BT_ballDist==145){
      /*if (xPos<lAimzone) VD_bearing=yPos*BT_aimKp;
      else if (xPos>rAimzone) VD_bearing=360-(yPos*BT_aimKp);
      else VD_bearing=0;*/
      VD_bearing=0;
    }
  }
  
  else{
    //Base Align
    /*if(yPos>60||yPos<50){
      if(yPos>55){
        VD_bearing=180;
        VD_speed=0.4;
      }
      else{
        VD_bearing=0;
        VD_speed=0.3;
      }
    }
    else if(xPos>95||xPos<85){
      if(xPos>90){
        VD_bearing=270;
        VD_speed=0.25;
      }
      else{
        
        VD_bearing=90;
        VD_speed=0.25;
      }
    }
    else{*/
    VD_bearing=0;
    VD_speed=0.0;
    //}
    
  }
  
  if(VD_speed + VD_compassSpeed > 200){
    VD_speed -= VD_compassSpeed;
  }
  
  Serial.print("\tSpd:");
  Serial.print(VD_speed);
  
  //Compute vector
  NEPower=NEPower+(VectorDynamics[VD_bearing][1]*VD_speed)/255;
  SEPower=SEPower+(VectorDynamics[VD_bearing][0]*VD_speed)/255;
  SWPower=SWPower+(VectorDynamics[VD_bearing][1]*VD_speed)/255;
  NWPower=NWPower+(VectorDynamics[VD_bearing][0]*VD_speed)/255;
  
  Serial.print("\tNE:");
  Serial.print(NEPower);
  Serial.print("\tNW:");
  Serial.print(NWPower);
  Serial.print("\tSE:");
  Serial.print(SEPower);
  Serial.print("\tSW:");
  Serial.print(SWPower);
  
  //Set motor powers
  NEMotor.setPower(NEPower);
  SEMotor.setPower(SEPower);
  SWMotor.setPower(SWPower);
  NWMotor.setPower(NWPower);
  //output timestamp
  Serial.print("\t-\t");
  Serial.println(millis());
}



