/*

 * cameraDataHandler.cpp
 *
 *  Created on: 2016¦~6¤ë3¤é
 *      Author: yungc
 */

#include "cameraDataHandler.h"

CamHandler::CamHandler(){

}

void CamHandler::extractBase(){
	covertBit();
	bool middleState = intermediate[4599];

	if(middleState){	// currently   XXXXXXXXBBBWWWWWWWWWWWBBBXXXXXXX
		shift = middle;	//first identify straight
		for(int i = 4598; i>4559; i--){
			if(!intermediate[i]){
				extractLeftLine(i);
			}
		}
		for(int i = 4600; i<4639; i++){
			if(!intermediate[i]){
				extractRightLine(i);
			}
		}

	}
	else{	// currently at black
			// some case may lead to this situation:
			// 1) a block is watched
			// 2) + route
			// 3) route out of bound
		int8_t 	LwhiteCount = 0,
				RwhiteCount = 0,
				LshiftPT = -1,//check -1 for not init
				RshiftPT = -1;//check -1 for not init

		for(int i=1; i<40;i++){
			if(intermediate[4599-i]){
				LwhiteCount++;
				if(LshiftPT == -1) LshiftPT = 4599-i;
			}

			if(intermediate[4598+i]){
				RwhiteCount++;
				if(RshiftPT == -1) RshiftPT = 4599-i;
			}
		}

		// then identify line type and call extract line to have deep identification
		if(RshiftPT != -1 || LshiftPT == -1){	//left side no white point
			extractLeftLine(LshiftPT);
			shift = left;
		}
		else if(RshiftPT == -1 || LshiftPT != -1){

		}

	}


}

void CamHandler::covertBit(){
	int curByte = 0;
	int temp = 0;
	for(int i = 0; i< ImageSize;i++){
		curByte = RawData[i];
		for(int j = 7; j >= 0; j--){
			if( j == 7) temp = (curByte >> 7);
			else temp = (curByte >> j) - (temp << 1);
		intermediate[8*i+(7-j)] = temp;
		}
	}
	/* what is in intermediate:
	 * intermediate[9] = {1,2,3,4,5,6,7,8,9}
	 * then the image is
	 * 	1	2	3
	 * 	4	5	6
	 * 	7	8	9
	 *
	 * 	then filter the bottom third line.
	*/

	// the following only apply to the bottom third row
	bool temp0 =intermediate[4559];
	bool temp1 =intermediate[4560];
	bool temp2;
	int8_t blackCount =0;
	for(int i =4561; i<=4639;i++){
		switch (i%3){
		case 0:
			temp1 = intermediate[i];
			break;
		case 1:
			temp2 = intermediate[i];
			break;
		case 2:
			temp0 = intermediate[i];
			break;
		default:
			break;
		}
		if(temp0 != temp1 && temp1 != temp2){ // BWB or WBW for 3 pixel then make it BBB or WWW
			intermediate[i-1] = intermediate[i];
		}
	}
}

void CamHandler::camCorrection(){

}

void CamHandler::camCorrectionInit(int16_t x_size,int16_t y_size){
	for(int i = x_size/-2; i < x_size/2; i++){
		for(int j = y_size/2; j < y_size/-2; j--){

		}

	}
}

