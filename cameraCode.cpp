#include <stdio.h>
#include <math.h>
#include <algorithm>
#include "E101.h"
using namespace std;
int lineCenter; 									// center of the line at the time of the last camera update
int lineWidth;
float CAMERA_ANGLE = M_PI_4; 						// camera angle in radians, forward = 0, down = PI/2
float CAMERA_HEIGHT = 7; 							// camera height in cm, measured from the ground to the center of the lens
float LENS_VIEW_ANGLE = 0.3614; 					// camera viewing angle in radians, measured from the center outwards


// dist is the lateral distance in cm from the camera is going to be analysed, steps is the amount of pixels to be analyzed on the row
// this function updates the line width and center feilds
void updateCamera(float dist, int steps){
	float stepSize = 320 / steps;
	int row = (int)(240.0 - 120.0*(1+(tan(atan(dist / CAMERA_HEIGHT) - CAMERA_ANGLE)/tan(LENS_VIEW_ANGLE))));
													// I can explain this math if anyone wants to know
													// what it does, I drew a picture and everything
	row = max(min(row, 239),0);
	int maxBright = 0; 								// integer for tracking the brightest value of a pixel on the row
	int minBright = 0; 								// same thing but for dim values
	int pixel[steps]; 								// array to store brightness values of scanned pixels
	
	// scan <steps> pixels on the <row>th row, draw over them with green pixels, and find the brightest and dimmest ones
	for (int step = 0; step < steps; step++){
		int column = (step + 0.5) * stepSize;
		char p = get_pixel(row, column, 3);
		set_pixel(row, column, 255, 0, 0);
		pixel[step] = p;
		maxBright = max((int)p, maxBright);			// find brighest pixel
		minBright = min((int)p, minBright); 		// find dimmest pixel
	}
	
	int threshold = (maxBright + minBright) / 2; 	// find threshold for what brightness to count as a "white" pixel
	int totalX = 0; 								// integer for summing up white pixels' x postions on screen, so the 
	int whitePoints = 0;
	int maxPos = 0;
	int minPos = 0;
	
	// find the average x pos for all white pixels
	for (int step = 0; step < steps; step++){
		if (pixel[step] > threshold){ 				// test pixel to see if it's white
			int column = (step + 0.5) * stepSize;
			set_pixel(row, column, 0, 255, 0);
			totalX += column;
			whitePoints++;
			maxPos = max(column, maxPos); 			// find rightmost whte pixel
			minPos = min(column, minPos); 			// find leftmost whtie pixel
		}
	}
	printf("Threshold: %d\n", threshold);
	lineCenter = (maxPos + minPos) / 2;
	lineWidth = maxPos - minPos;
	
}

int main(){
	init();
	open_screen_stream();
	for(int i = 0; i < 30; i++){
		take_picture();
		for(float ii = 3.3; ii < 15.5; ii+=0.1){
			updateCamera(ii,320);
		}
		update_screen();	
		sleep1(0,50000);
	}
	return 0;
}
