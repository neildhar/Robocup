#include<iostream>
#include<cmath>
#define PI 3.14159265
using namespace std;
double NWSESpeed,SWNESpeed,angle;
int main(){
    cout<<"char VectorDynamics[361][2]={";
    for(double i=0;i<361;i++){
        if(i>=0&&i<=90){
            angle=-i+45;
            NWSESpeed=100;
            SWNESpeed=100*tan(angle*PI/180);
        }
        else if(i>=90&&i<=180){
            angle=135-i;
            SWNESpeed=-100;
            NWSESpeed=100*tan(angle*PI/180);
        }
        else if(i>=180&&i<=270){
            angle=225-i;
            NWSESpeed=-100;
            SWNESpeed=-100*tan(angle*PI/180);
        }
        else if(i>=270&&i<=360){
            angle=315-i;
            SWNESpeed=100;
            NWSESpeed=-100*tan(angle*PI/180);
        }
        cout<<(i?",{":"{")<<floor(NWSESpeed+0.5)<<","<<floor(SWNESpeed+0.5)<<"}";
    }
    cout<<"};";
    return 0;

}
