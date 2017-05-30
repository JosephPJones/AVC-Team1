#include <stdio.h>
#include "E101.h"
int main(){
	init();
	set_motor(1,60);
	set_motor(2,-60);
	for(int i = 0; i < 50; i++){
		take_picture();
		int wSum = 0;
		int rSum = 0;
		for (int col = 50; col < (320 - 50); col++){
			wSum += get_pixel(120, col, 3);
			rSum += get_pixel(120, col, 3);
		}
		if (rSum>2*wSum)return 1;
		sleep1(0,100000);
	}
	stop(0);
	return 0;
}

