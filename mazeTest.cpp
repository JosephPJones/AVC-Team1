#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"
using namespace std;
const int frontWallDist = 200;
const int rightWallDist = 100;
const int mazePower = 100;
void setMotors(double,int);
int main(){
	for(int i = 0; i < 100; i++){
		//int leftIR = read_analog(5);
		int frontIR = read_analog(6);
		int rightIR = read_analog(7);
		double dir = (frontIR > frontWallDist)?-1:(rightIR < rightWallDist)?1:-0.2;
		setMotors(dir, mazePower);
		sleep1(0,100000);
	}
	return 0;
}	
	void setMotors(double dir, int pow){
	dir = min(max(dir, -1.0), 1.0);	//constrain dir value to [-1,1]
	pow = min(max(pow, -254), 254); //constrain power value to [-254,254]
	double lPow = -pow * min(1+(2*dir),1.0);
	double rPow =  pow * min(1-(2*dir),1.0);
	set_motor(2, lPow);
	set_motor(1, rPow);
}
