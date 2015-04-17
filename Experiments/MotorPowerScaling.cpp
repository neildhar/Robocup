#include<iostream>;
using namespace std;
int maxi(int a, int b){
    return a>b?a:b;
}
double MPS(int Ma,int Mb, int Mc, int Md){
    double highestPow=maxi(maxi(Ma,Mb),maxi(Mc,Md));
    if(highestPow>0) return (double)(100/highestPow);
    else return 0;
}
int main(){
    cout<<MPS(40,30,30,40)<<endl;
    return 0;
}
