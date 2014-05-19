#include <Wire.h>
#include "Motoren.h"
#include "JoinMaxSensors.h"
#include <math.h>
#include "SRF10.h";
#include "CMPS10.h"

//Declare constants
#define ambientIR 15
#define BA_middle 50 
#define BA_back 50
#define backOffset 20
#define lAimzone 45
#define rAimzone 118
#define MPS 1
#define CA_Kd 5
//#define CA_Kd 0
#define BA_Kp 0.71

//Declare Sensors
JM::compoundEye front_CE(&Wire);
JM::compoundEye back_CE(&Wire1);
SRF10 right_US(&Wire,0xEA);
SRF10 left_US(&Wire, 0xEC);
SRF10 back_US(&Wire1, 0xEE);
CMPS10 compass(&Wire1, 0xC0);

//Declare Motors
Motoren SEMotor(30,31,2);
Motoren SWMotor(32,33,3,1);
Motoren NEMotor(34,35,4);
Motoren NWMotor(36,37,5,1);

//Declare Vectors
int VectorDynamics[361][2]={{255,255},{255,246},{255,238},{255,230},{255,222},{255,214},{255,206},{255,199},{255,192},{255,185},{255,179},{255,172},{255,166},{255,159},{255,153},{255,147},{255,141},{255,136},{255,130},{255,124},{255,119},{255,114},{255,108},{255,103},{255,98},{255,93},{255,88},{255,83},{255,78},{255,73},{255,68},{255,64},{255,59},{255,54},{255,50},{255,45},{255,40},{255,36},{255,31},{255,27},{255,22},{255,18},{255,13},{255,9},{255,4},{255,0},{255,-4},{255,-9},{255,-13},{255,-18},{255,-22},{255,-27},{255,-31},{255,-36},{255,-40},{255,-45},{255,-50},{255,-54},{255,-59},{255,-64},{255,-68},{255,-73},{255,-78},{255,-83},{255,-88},{255,-93},{255,-98},{255,-103},{255,-108},{255,-114},{255,-119},{255,-124},{255,-130},{255,-136},{255,-141},{255,-147},{255,-153},{255,-159},{255,-166},{255,-172},{255,-179},{255,-185},{255,-192},{255,-199},{255,-206},{255,-214},{255,-222},{255,-230},{255,-238},{255,-246},{255,-255},{246,-255},{238,-255},{230,-255},{222,-255},{214,-255},{206,-255},{199,-255},{192,-255},{185,-255},{179,-255},{172,-255},{166,-255},{159,-255},{153,-255},{147,-255},{141,-255},{136,-255},{130,-255},{124,-255},{119,-255},{114,-255},{108,-255},{103,-255},{98,-255},{93,-255},{88,-255},{83,-255},{78,-255},{73,-255},{68,-255},{64,-255},{59,-255},{54,-255},{50,-255},{45,-255},{40,-255},{36,-255},{31,-255},{27,-255},{22,-255},{18,-255},{13,-255},{9,-255},{4,-255},{0,-255},{-4,-255},{-9,-255},{-13,-255},{-18,-255},{-22,-255},{-27,-255},{-31,-255},{-36,-255},{-40,-255},{-45,-255},{-50,-255},{-54,-255},{-59,-255},{-64,-255},{-68,-255},{-73,-255},{-78,-255},{-83,-255},{-88,-255},{-93,-255},{-98,-255},{-103,-255},{-108,-255},{-114,-255},{-119,-255},{-124,-255},{-130,-255},{-136,-255},{-141,-255},{-147,-255},{-153,-255},{-159,-255},{-166,-255},{-172,-255},{-179,-255},{-185,-255},{-192,-255},{-199,-255},{-206,-255},{-214,-255},{-222,-255},{-230,-255},{-238,-255},{-246,-255},{-255,-255},{-255,-246},{-255,-238},{-255,-230},{-255,-222},{-255,-214},{-255,-206},{-255,-199},{-255,-192},{-255,-185},{-255,-179},{-255,-172},{-255,-166},{-255,-159},{-255,-153},{-255,-147},{-255,-141},{-255,-136},{-255,-130},{-255,-124},{-255,-119},{-255,-114},{-255,-108},{-255,-103},{-255,-98},{-255,-93},{-255,-88},{-255,-83},{-255,-78},{-255,-73},{-255,-68},{-255,-64},{-255,-59},{-255,-54},{-255,-50},{-255,-45},{-255,-40},{-255,-36},{-255,-31},{-255,-27},{-255,-22},{-255,-18},{-255,-13},{-255,-9},{-255,-4},{-255,0},{-255,4},{-255,9},{-255,13},{-255,18},{-255,22},{-255,27},{-255,31},{-255,36},{-255,40},{-255,45},{-255,50},{-255,54},{-255,59},{-255,64},{-255,68},{-255,73},{-255,78},{-255,83},{-255,88},{-255,93},{-255,98},{-255,103},{-255,108},{-255,114},{-255,119},{-255,124},{-255,130},{-255,136},{-255,141},{-255,147},{-255,153},{-255,159},{-255,166},{-255,172},{-255,179},{-255,185},{-255,192},{-255,199},{-255,206},{-255,214},{-255,222},{-255,230},{-255,238},{-255,246},{-255,255},{-246,255},{-238,255},{-230,255},{-222,255},{-214,255},{-206,255},{-199,255},{-192,255},{-185,255},{-179,255},{-172,255},{-166,255},{-159,255},{-153,255},{-147,255},{-141,255},{-136,255},{-130,255},{-124,255},{-119,255},{-114,255},{-108,255},{-103,255},{-98,255},{-93,255},{-88,255},{-83,255},{-78,255},{-73,255},{-68,255},{-64,255},{-59,255},{-54,255},{-50,255},{-45,255},{-40,255},{-36,255},{-31,255},{-27,255},{-22,255},{-18,255},{-13,255},{-9,255},{-4,255},{0,255},{4,255},{9,255},{13,255},{18,255},{22,255},{27,255},{31,255},{36,255},{40,255},{45,255},{50,255},{54,255},{59,255},{64,255},{68,255},{73,255},{78,255},{83,255},{88,255},{93,255},{98,255},{103,255},{108,255},{114,255},{119,255},{124,255},{130,255},{136,255},{141,255},{147,255},{153,255},{159,255},{166,255},{172,255},{179,255},{185,255},{192,255},{199,255},{206,255},{214,255},{222,255},{230,255},{238,255},{246,255},{255,255}};

//Declare Misc. Variables
int compassValue, CA_correction, CA_lastError,
VD_compound, NEPower, SEPower, NWPower, SWPower,
fIRValue, bIRValue, BT_ballPos, BT_ballDist, BT_dirMod, BT_clockPort, BT_anticlockPort, BT_bulb,
BA_xDisp, BA_yDisp,
VD_bearing, VD_highestPower,
xPos, yPos, rUSValue;
const double CA_Kp=1.42, BT_aimKp = 90/195;
double VD_motorPowerScaler, VD_speed;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();
}

void loop() {
  Wire.begin();
  Wire1.begin();
  //Compass Align
  compassValue = compass.read(); 
  CA_correction = compassValue!=0?(compassValue-CA_lastError)*CA_Kd+compassValue*CA_Kp:0;
  CA_lastError=compassValue;
  NEPower=SEPower=CA_correction;
  NWPower=SWPower=-CA_correction;
  ++VD_compound;

  xPos = left_US.read();
  yPos = xPos<rAimzone||xPos>lAimzone?back_US.read()+20:back_US.read();
  fIRValue=front_CE.highestValue();
  bIRValue=back_CE.highestValue();
  if(fIRValue>ambientIR||bIRValue>ambientIR){
    VD_speed=0.8;
    //Ball Tracking
    if(fIRValue>bIRValue){
      BT_bulb = front_CE.highestBulb();
      BT_ballDist = fIRValue;
      BT_ballPos = 240+(30*BT_bulb);
      BT_clockPort = BT_bulb!=7?front_CE.readBulb(BT_bulb+1):back_CE.readBulb(1);
      BT_anticlockPort = BT_bulb!=1?front_CE.readBulb(BT_bulb-1):back_CE.readBulb(7);  
    }
    else{
      BT_bulb = back_CE.highestBulb();
      BT_ballDist = bIRValue;
      BT_ballPos = 60+(30*BT_bulb);
      BT_clockPort = BT_bulb!=7?back_CE.readBulb(BT_bulb+1):front_CE.readBulb(1);
      BT_anticlockPort = BT_bulb!=1?back_CE.readBulb(BT_bulb-1):front_CE.readBulb(7);  
    }
    Serial.print(BT_ballPos = (((BT_clockPort-BT_anticlockPort)/6)+BT_ballPos)%360); Serial.print(" "); Serial.println(BT_ballDist); //Determine Ball Position
    if ( BT_ballPos<=90 ) BT_dirMod=BT_ballPos;
    else BT_dirMod=BT_ballPos>=270?360-BT_ballPos:80;
    if ( BT_ballPos<=180&&BT_ballPos>=15 ) VD_bearing=BT_ballPos+(BT_dirMod*BT_ballDist)/145;
    else if ( BT_ballPos<=345&&BT_ballPos>180 ) VD_bearing=BT_ballPos-(BT_dirMod*BT_ballDist)/145;
    else VD_bearing=BT_ballPos;
    if ( (BT_ballPos>345||BT_ballPos<15) && BT_ballDist==145){
      if (xPos<lAimzone) VD_bearing=yPos*BT_aimKp;
      else if (xPos>rAimzone) VD_bearing=360-(yPos*BT_aimKp);
      else VD_bearing=0;
    }
  }
  
  else{
    //Base Align
    VD_bearing=0;
    VD_speed=0;
    /*BA_xDisp = BA_middle-xPos;
    BA_yDisp = BA_back-yPos;
    if(BA_yDisp<0) VD_bearing = 180+atan(BA_xDisp/BA_yDisp)*57.2957795;
    else if(BA_yDisp>0&&BA_xDisp>0) VD_bearing = atan(BA_xDisp/BA_yDisp)*57.2957795;
    else if(BA_yDisp>0&&BA_xDisp<0) VD_bearing = 360+atan(BA_xDisp/BA_yDisp)*57.2957795;
    else if(BA_yDisp>=0&&BA_xDisp==0) VD_bearing = 0;
    else if(BA_yDisp==0&&BA_xDisp>0) VD_bearing = 90;
    else if(BA_yDisp<0&&BA_xDisp==0) VD_bearing = 270;
    if(abs(BA_yDisp)<10&&abs(BA_xDisp)<10){VD_speed=0; VD_compound--;}
    else VD_speed=sqrt(sq(BA_xDisp)+sq(BA_yDisp))*BA_Kp;*/
  }
  //Serial.println(VD_bearing);
  //Compute vector
  VD_compound++;
  NEPower=(NEPower+(VectorDynamics[VD_bearing][1]*VD_speed));
  SEPower=(SEPower+(VectorDynamics[VD_bearing][0]*VD_speed));
  SWPower=(SWPower+(VectorDynamics[VD_bearing][1]*VD_speed));
  NWPower=(NWPower+(VectorDynamics[VD_bearing][0]*VD_speed));
  //Compute and set motor powers
  VD_motorPowerScaler=(double)MPS/VD_compound;
  NEMotor.setPower(NEPower*=VD_motorPowerScaler);
  SEMotor.setPower(SEPower*=VD_motorPowerScaler);
  SWMotor.setPower(SWPower*=VD_motorPowerScaler);
  NWMotor.setPower(NWPower*=VD_motorPowerScaler);
  Serial.print(NEPower); Serial.print(" ");Serial.print(NWPower); Serial.print(" ");
  Serial.print(SEPower); Serial.print(" ");Serial.print(SWPower); Serial.print(" "); Serial.println(compassValue);
  VD_compound=0;
  
}                         
