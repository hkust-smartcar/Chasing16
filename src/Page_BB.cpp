/*
 * Page_BB.cpp
 *
 *  Created on: 22 Feb 2016
 *      Author: page
 */

#include "car.h"
#include "RunMode.h"

	bool left_edge[60] = {false};
	bool right_edge[60]= {false};


int16_t RunMode::turningPID (int16_t mid_line){
	int angle_error= 40-(left_edge[30]+right_edge[30])/2;
	int pre_angle_error=0;
	int servo_output=0;
	int Kp_s=0;

	int Kd_s=0;
	int Kd;


	servo_output= Kp_s*angle_error + Kd_s*(angle_error-pre_angle_error);

	pre_angle_error= angle_error;

	return 0;

}
//image adjustment
void RunMode::adjust_image (){

}

void RunMode::extract_line(Byte image[600]){
	//define edge
	// z=80y +x


/*
	for (int j=0; j<60; j++){
		if (j<=10){
			for (int i =1; i<80; i++){
				if (image[80j+1]!= WHITE){
					if (image[80j+i-1]==BLACK&&image[80j+i]==WHITE){
						left_edge[j]=80j+i;
					}

				}
				if(image[80j+79]!= WHITE){
					if (image[80j+80-i]==BLACK&&image[80j+79-i]==WHITE){
						right_edge[j]=80j+79-1;
					}
				}

			}

		}
		if(j>10){
			for (int i =left_edge[j-1]-5; i<left_edge[j-1]+5 ; i++){
				if (image[80j+i-1]==BLACK&&image[80j+i]==WHITE){
									left_edge[j]=80j+i;
								}
			for (int i =right_edge[j-1]-5; i<right_edge[j-1]+5 ; i++){
				if (image[80j+i]==WHITE&&image[80j+i+1]==BLACK){
						right_edge[j]=80j+i;
								}
			}
			}
		}

	}*/
}




//servo
