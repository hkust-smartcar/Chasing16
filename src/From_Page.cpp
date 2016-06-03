/*
 * Page_BB.cpp
 *
 *  Created on: 22 Feb 2016
 *      Author: page
 */

#include "car.h"
#include "RunMode.h"



//servo
int16_t RunMode::turningPID (){
	//compare ideal mid-line and actual mid-line
	angle_error= 40-(left_edge[30]+right_edge[30])/2;
	pre_angle_error=0;
	servo_output=0;
	Kp_s=0;
	Kd_s=0;

	servo_output= Kp_s*angle_error + Kd_s*(angle_error-pre_angle_error);

	pre_angle_error= angle_error;

	return 0;

}
/*
 //a= the error of current line and ideal line
 //b, c = future prediction
 for (int j = 0; j<60; j++){
 	 mid_line[j]= (left_edge[j]+right_edge[j])/2;
 	 }

 angle_error1 = 30-mid_line[15];
 angle_error2 = mid_line[30]-mid_line[15];
 angle_error3= mid_line[50]-mid_line{15];
 angle_error_t= a_s*angle_error1 + b_s*angle_error2 + c_s*angle_error3;
//a>b>c ( reliability)



*/
//image adjustment
void RunMode::adjust_image (){

}

void RunMode::extract_line(){
	//define edge
	// z=80y +x


//find left_edge and right_edge for j=1 to 10
	for (int j=0; j<60; j++){
		if (j<=10){
			for (int i =1; i<80; i++){
				if (image[80*j+1]!= WHITE){
					if (image[80*j+i-1]==BLACK&&image[80*j+i]==WHITE){
						left_edge[j]=80j+i;
					}

				}
				if(image[80*j+79]!= WHITE){
					if (image[80*j+80-i]==BLACK&&image[80*j+79-i]==WHITE){
						right_edge[j]=80j+79-1;
					}
				}

			}

		}
		//find left_edge and right_edge for j=10 to 60
		if(j>10){
			for (int i =left_edge[j-1]-5; i<left_edge[j-1]+5 ; i++){
				if (image[80*j+i-1]==BLACK&&image[80*j+i]==WHITE){
									left_edge[j]=80j+i;
								}
			for (int i =right_edge[j-1]-5; i<right_edge[j-1]+5 ; i++){
				if (image[80*j+i]==WHITE&&image[80*j+i+1]==BLACK){
						right_edge[j]=80j+i;
								}
			}
			}
		}

	}
}
/*
 //when no edge can be found
 if(image[80j+1]=WHITE){
 call a function}
 function ( b = j;
 for (int a =j; a<60-j;a++){
 if (image[80a+1]==WHITE){
 count=count+1}
 if (image[80a+1]!=WHITE){
 f=a
 BREAK}
 mid_line[j]= (j-b)(mid_line[b]+mid_line[f])/(count+1)+mid_line[b];


 */

