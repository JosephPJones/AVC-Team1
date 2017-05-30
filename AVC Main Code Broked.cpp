#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"

using namespace std;
const int recordSize = 12;
int whitePoints;
double lineCenter;
double lcRecord[recordSize]; //a record of the recent line center values
double kp;
double kd;
int powerBoost = 0; //Boosts the power to the motors depending on how much the line ceneter is changing
const double turnScale = 0.6; //How hard our tiny rick turns
const double bendScale = 0.6; //for slowing down aproaching corners
const int    	 power = 50; //The standard power to the motors before modifiers are applied
const int    turnBoost = 0; //for AMPING UP the motors on corners, so it doesn't stall
const double boostScale = 0.15; //Multiplier for powerBoost
const int    stdRow = 95; //fuck this, ignore the shit out of it
int quadrant = 1; //yeah, this too 
int threshold;
bool pastGate = false;

void updateThreshold(int row){
	int maxBright = 0;
	int minBright = 0;
	row -= 80;
	for(int i = 0; i < 5; i++){
		for(int step = 0; step < 320; step++){
			char p = get_pixel(row, step,3);
			maxBright = max(maxBright, (int)p);
			minBright = min(minBright, (int)p);
		}
	row += 40;
	}
	threshold = (maxBright - minBright) / 2;
	printf("threshold = %d\n", threshold);
}

void updateCamera(int row){
	int pixel[320];
	lineCenter = whitePoints = 0;
	for(int step = 0; step < 320; step++){
		char p = get_pixel(row, step,3);
		pixel[step] = p;
	}
	int whitePoints = 0;
	int sum = 0;
	for(int step = 0; step < 320; step++){
		if (pixel[step] > threshold){
			set_pixel(row, step, 0, 128, 128);
			whitePoints++;
			sum += step;
		}
	}
	if (whitePoints > 0){
		lineCenter = ((sum/whitePoints) - 160.0)/160.0;
		set_pixel(row, (int)(sum/whitePoints), 255, 0, 0);
		set_pixel(row - 1, (int)(sum/whitePoints), 0, 255, 0);		
		set_pixel(row + 1, (int)(sum/whitePoints), 0, 0, 255);
		printf("center:%f points:%d\n",lineCenter, whitePoints);
	}	
}

void setMotors(double dir, int pow){//direction
	pow+=(int)((fabs(dir))*turnBoost);
	pow = min(max(pow, -254), 254);
	double turning;
	turning = min(max(dir, -(double)1), (double)1);
	turning = (fabs(dir)>(turnScale/10)? turning : 0);
	double p1 =  pow * min(1 + (2 * turning), (double)1);
	double p2 =  -pow * min(1 - (2 * turning), (double)1);
	set_motor(1, p1);
	set_motor(2, p2);
}

void followLine(int dftRow){
	take_picture();
	updateThreshold(dftRow);
	updateCamera(dftRow);
	kp = -lineCenter;
	double lastCenter = lineCenter;
	updateCamera(dftRow / 2);
	double bend = lineCenter - lastCenter;
	for(int i = 0; i < recordSize-1; i++){
		lcRecord[i] = lcRecord[i+1];
	}
	lcRecord[recordSize-1] = lineCenter;
	double mx = lineCenter;
	double mn = lineCenter;
	for(int i = 0; i < recordSize; i++){// loops through array finding its range
		mx = max(mx, lcRecord[i]);
		mn = min(mn, lcRecord[i]);
	}
	powerBoost = max(50 - (int)(mx - mn), 0);
	setMotors(kp*turnScale, power - fabs(bend)*127*bendScale + powerBoost*boostScale);
	sleep1(0,100000);
}

void doQuadrantOne(){
	char server_addr[15] = "130.195.6.196";
	connect_to_server(server_addr, 1024);
	char message[24] = "Please";
	send_to_server(message);
	receive_from_server(message);
	printf("%s\n", message);
	send_to_server(message);
	quadrant++;
}

void doQuadrantTwo(int dftRow){
	//updateCamera(60);
	//kd = (lineCenter - firstPoint - 160)/160;
	take_picture();
	updateThreshold(stdRow);
	updateCamera(stdRow);
	if (whitePoints < 10){
		setMotors(0, -75);
		sleep1(0, 500000);
	}else{
		followLine(dftRow);
		sleep1(0,100000);
		if (whitePoints>280){
		quadrant++;
		}
	}
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

void doQuadrantThree(){
	take_picture();
	updateThreshold(stdRow);
	updateCamera(stdRow);
	if (whitePoints > 200){	//check if line is wide (possibly an intersection.)
		if (lineCenter < -0.1){
			set_motor(1,0);
			set_motor(2,0);
			sleep1(0,150000);
			set_motor(1, 80);
			set_motor(2, -15);
			sleep1(0,800000);
		} //take the turn if intersection has left turn
		else for(int i = 1; i < 10; i++){
			followLine(stdRow/2);
			sleep1(0,100000);
			} // keep going if intersection doesnt
	}
	else if (whitePoints < 40){
		updateCamera (240-(stdRow/2));
		if (whitePoints > 60){//turn around
			set_motor(1, 80);
			set_motor(2, 80);
			sleep1(0,800000);
			set_motor(1,0);
			set_motor(2,0);
			sleep1(0,150000);
			set_motor(1,-60);
			set_motor(2,60);
			sleep1(0,500000);
		}
	}
	else followLine(stdRow);
	if (checkRed(stdRow)) quadrant++;
}

void doQuadrantFour(){
	int sl = read_analog(5);
	int sc = read_analog(6);
	int sr = read_analog(7);
	if (checkRed(stdRow) && ! pastGate){
		while (sc >200)sleep1(0,100000);
		pastGate = true;
	}			
	int diff = sl - sr;
	int pow = power;
	if (sc > 300){}
	setMotors(turnScale*(diff)/10, pow);
} 
 
int main(){	
	init();
	for (int i = 0; i < recordSize; i++){
		lcRecord[i] = 0;
	}
	open_screen_stream();
	for(int i =0; i<60; i++){
		update_screen();
		switch(quadrant){
			case 1:{doQuadrantOne(); break;}
			case 2:{doQuadrantTwo(stdRow); break;}
			case 3:{doQuadrantThree(); break;}
			case 4:{doQuadrantFour(); break;}
		}
	}	
	stop(0);
	return 0;
}
