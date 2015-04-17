#include<iostream>
using namespace std;
int motorValueProc(int value, bool resultType){
    if(resultType==0){
        return value>=0?value:value*-1;
    }
    else if(resultType ==1){
        return value<0?2:(value==0?1:0);
    }
}
int main(){
    int v,r;
    cin>>v>>r;
    cout<<motorValueProc(v,r);
    return 0;
}
