#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"
using namespace std;
int whitePoints;
float lineCenter;
float kp;
float kd;
float do_network;
int quadrant = 2;

void updateCamera(int row){
	int maxBright = 0;
	int minBright = 0;
	int pixel[320];
	lineCenter = whitePoints = 0;
	for(int step = 0; step < 320; step++){
		char p = get_pixel(row, step,3);
		maxBright = max(maxBright, (int)p);
		minBright = min(minBright, (int)p);
		pixel[step] = p;
	}
	int threshold = (maxBright - minBright) / 2;	
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
	}
	if (lineCenter>0){
		set_pixel(2,2,255,0,0);
		set_pixel(2,3,255,0,0);
		set_pixel(2,4,255,0,0);
		set_pixel(3,2,255,0,0);
		set_pixel(3,3,255,0,0);
		set_pixel(3,4,255,0,0);
		set_pixel(4,2,255,0,0);
		set_pixel(4,3,255,0,0);
		set_pixel(4,4,255,0,0);
	}
	
}

void setMotors(float direction, int power){//direction
	direction = min(max(direction, - (float)1), (float)1);
	set_motor(1, power * min(1 + (2 * direction), (float)1));
	set_motor(2, power * min(1 - (2 * direction), (float)1));
	//printf("kd: %f lc: %f rc: %f\n", kp,  min(1 + (2 * direction), (float)1), min(1 - (2 * direction), (float)1));
}

void doQuadrantOne(){
	
}

void doQuadrantTwo(int dftRow){
	take_picture();
		updateCamera(dftRow);
		//while (whitePoints < 10){
		//	updateCamera(120);		
		//	setMotors(0, -128);
		//}
		kp = (lineCenter-160)/160;
		int firstPoint = lineCenter;
		updateCamera(60);
		kd = (lineCenter - firstPoint - 160)/160;
		//setMotors(1.0*kp - 1.0*kd, 64);
		update_screen();
		sleep1(0,100000);
}

void doQuadrantThree(){
	take_picture();
	updateCamera(120);
	if (whitePoints > 100){	//check if line is wide (possibly an intersection.)
		if (lineCenter > 0){for(int i = 1; i < 30; i++){ doQuadrantTwo(120);}} //take the turn if intersection has right turn
		else{for(int i = 1; i < 30; i++){ doQuadrantTwo(120);}} // keep going if intersection has leftTurn
	}
}

void doQuadrantFour(){
		
}
 
int main(){	
	init();
	open_screen_stream();
	//while (quadrant <= 4){
	for(int i = 0; i < 100; i++){//temp
		switch(quadrant){
			case 1:{doQuadrantOne(); break;}
			case 2:{doQuadrantTwo(120); break;}
			case 3:{doQuadrantThree(); break;}
			case 4:{doQuadrantFour(); break;}
		}
	}	
	stop(0);
	return 0;
}
