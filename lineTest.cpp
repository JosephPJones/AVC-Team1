#include <stdio.h>
#include "E101.h"
#include <math.h>
#include <algorithm>
using namespace std;
int threshold;
const int stepSize = 4;
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

int main(){
	init();
	set_motor(1,60);
	set_motor(2,-60);
	for(int i = 0; i < 50; i++){
		int points = 0;
		take_picture();
		updateThreshold();
		for (int col = stepSize/2; col<320; col += stepSize)
			points += (get_pixel(120,col,3)>threshold)?1:0;
		if (points>300/stepSize)
			return 1;
		sleep1(0,100000);
	}
	stop(0);
	return 0;
}
