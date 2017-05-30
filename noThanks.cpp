#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"
using namespace std;

//Convenience constants
const int stepSize = 4;
const int rMotor = 1;
const int lMotor = 2;
const int startQuadrant = 1; //quadrant to start on
const int endQuadrant = 4; //quadrant to end upon beginning(should be 5 for robot assessment)
//Debugging Constants
const int basePower = 80;
const int mazePower =  100; 
const double turnPower = 0.5;
const int defaultRow = 120;
const int rightWallDist = 100;
const int frontWallDist = 100;
const int lowerLineThreshold = 20/stepSize;

//Declare feilds
int quadrant = startQuadrant;
int lineCenter;
int whitePoints;
int threshold;
int power = basePower;
bool pastGate = false;

//Declare functions
void doQuadrant(int);
void scanLine(int);
void updateThreshold();
void setMotors(double, int);
void sleepMs(int);
void followLine(int);
bool checkRed(int);

//The actual code stuff
int main(){
	while(true){
		take_picture();
		updateThreshold();
		doQuadrant(quadrant);
		if (quadrant == endQuadrant){
			stop(0);
			return 0;
		}
	}
}

void doQuadrant(int quad){
	switch (quad){	
			
	//////QUADRANT 1//////	
case 1:{
	char IP[15] = "130.195.6.196";
	connect_to_server(IP, 1024);
	char message[24] = "Please";
	send_to_server(message);
	receive_from_server(message);
	send_to_server(message);
	printf("Quadrant 1 completed, beginning quadrant 2\n");
	quadrant++;
	
break;}
	//////QUADRANT 2//////
case 2:{
	followLine(defaultRow);
	if (whitePoints > 300){
		quadrant++;
		for (int i = 0; i < 5; i++){
			followLine(defaultRow/2);
			sleepMs(100);
		}
	}
break;}
	//////QUADRANT 3//////
case 3:{
	if (whitePoints > 150/stepSize){			//check for intersection
		if (lineCenter <150/stepSize){			//if intersection turns left
			setMotors(0,0);	sleepMs(200);		//take the turn
			setMotors(-0.6, 80); sleepMs(800);
		}
		else 									//otherwise
			for (int i = 0; i < 5; i++){		//continue forwards
				followLine(defaultRow/2);
				sleepMs(100);
			}
	}
	else followLine(defaultRow);
	if (checkRed(defaultRow)){
		quadrant++;
	}
break;}
	//////QUADRANT 4//////
case 4:{
	//int leftIR = read_analog(5);
	int frontIR = read_analog(6);
	int rightIR = read_analog(7);
	double dir = (frontIR > frontWallDist)?-1:(rightIR < rightWallDist)?1:-0.2;
	setMotors(dir, mazePower);
	if (checkRed(defaultRow) && !pastGate){
		setMotors(0,0);
		while (frontIR<300)//wait for gate to close
			sleepMs(100);
		while (frontIR>300)//wait for gate to open
			sleepMs(100);
		pastGate = true;
	}
break;}
	}
}

void updateThreshold(){
	int maxVal = 0;
	int minVal = 255;
	for(int row = stepSize/2; row < 240; row += stepSize){
		for(int col = stepSize/2; col < 320; col += stepSize){
			int pixelVal = get_pixel(row, col, 3);
			maxVal = max(maxVal, pixelVal);
			minVal = min(minVal, pixelVal);
		}
	threshold = (maxVal+minVal)/2;
	}
}

void scanLine(int row){
	int sum = 0;
	whitePoints = 0;
	for (int col = stepSize/2; col < 320; col += stepSize){
		sum += (get_pixel(row, col, 3)>threshold)? col : 0;
		whitePoints += get_pixel(row, col, 3)>threshold? 1 : 0;
	}
	if (whitePoints > 0) lineCenter = sum/whitePoints;
	else lineCenter = -1;
}

void followLine(int row){
	double turn = (lineCenter-160)/160.0;
	scanLine(row);
	if (whitePoints < lowerLineThreshold){ 		// if camera sees black
		scanLine(240-(240-defaultRow)/2); 		//scan closer to robot
		if (whitePoints < lowerLineThreshold 	//check to see if there is a line closer to the robot
		&& fabs(turn) < 0.1){ 					//and if closer line is centered
			setMotors(0,0); sleepMs(200);		//turn around and reverse a little
			setMotors(-1,80); sleepMs(800);
			setMotors(0,0); sleepMs(200);
			setMotors(0, -80); sleepMs(300);
		}
		else{									//otherwise just reverse
		setMotors(0, -80); sleepMs(300);}
	}
	else setMotors(turn*turnPower, power); 		//and if there IS a line, just follow it like a normal person
}

void setMotors(double dir, int pow){
	dir = min(max(dir, -1.0), 1.0);	//constrain dir value to [-1,1]
	pow = min(max(pow, -254), 254); //constrain power value to [-254,254]
	double lPow = -pow * min(1+(2*dir),1.0);
	double rPow =  pow * min(1-(2*dir),1.0);
	set_motor(lMotor, lPow);
	set_motor(rMotor, rPow);
}

void sleepMs(int ms){
	sleep1((int)(ms/1000.0),(ms%1000)*1000);
}

bool checkRed(int row){		
	int wSum = 0;
	int rSum = 0;
	for (int col = 50; col < (320 - 50); col++){
		wSum += get_pixel(row, col, 3);
		rSum += get_pixel(row, col, 3);
	}
	return (rSum/wSum>2);
}
