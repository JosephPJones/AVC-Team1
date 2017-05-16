#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"
using namespace std;
int whitePoints;
int lineCenter;
float kp;
float kd;
int quadrant = 1;

float updateCamera(int row){
	int maxBright = 0;
	int minBright = 0;
	int pixel[320];
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
	int lineCenter = 0;
	if (whitePoints > 0){lineCenter = ((sum/whitePoints) - 160.0)/160.0;}
}

void setMotors(float direction, int power){
	direction = min(max(direction, -1), 1);
	set_motor(1, power * min(1 + (2 * direction), (float)1));
	set_motor(2, power * min(1 - (2 * direction), (float)1));
	printf("kd: %f lc: %f rc: %f\n", kp,  min(1 + (2 * direction), (float)1), min(1 - (2 * direction), (float)1));
}

void doQuadrantOne(){
	
}

void doQuadrantTwo(){
	take_picture();
		updateCamera(120);
		while (whitePoints < 10){
			updateCamera();		
			setMotors(0, -128);
		}
		update_screen();
		kp = (lineCenter-160)/160;
		int firstPoint = lineCenter;
		updateCamera(60);
		kd = (lineCenter - firstPoint - 160)/160;
		setMotors(1.0*kp - 1.0*kd, 128);
		sleep1(0,100000);
}

void doQuadrantThree(){
	take_picture();
	updateCamera(120);
	if (whitePoints > 100){	//check if line is wide (possibly an intersection.)
		if (){
			for(int i = 1; i < 30){ doQuadrantOne();}
		}
	}
}

void doQuadrantFour(){
		
}

int main(){	
	open_screen_stream();
	//while (quadrant <= 4){
	for(int i = 0; i < 100; i++){//temp
		switch(quadrant){
			case 1:{doQuadrantOne(); break;}
			case 2:{doQuadrantTwo(); break;}
			case 3:{doQuadrantThree(); break;}
			case 4:{doQuadrantFour(); break;}
		}
	}	
	stop(0);
	return 0;
}
