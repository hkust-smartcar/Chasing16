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
	convertBit();
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
				if(LshiftPT == -1) LshiftPT = 4599-i+1;
			}

			if(intermediate[4598+i]){
				RwhiteCount++;
				if(RshiftPT == -1) RshiftPT = 4599+i-1;
			}
		}

		// then identify line type and call extract line to have deep identification
		if(RshiftPT != -1 || LshiftPT == -1){	//left side no white point, then the line in the right is the left line
			shift = left;
			extractLeftLine(RshiftPT);
		}
		else if(RshiftPT == -1 || LshiftPT != -1){	//right side no white point
			shift = right;
			extractRightLine(LshiftPT);
		}
		else if(RshiftPT != -1 || LshiftPT != -1){	//both side have white point
			if(LwhiteCount > RwhiteCount){	// more white in the left,take the left route as legal route
				shift = right;
				extractRightLine(LshiftPT);
			}
			else if(LwhiteCount < RwhiteCount){	//more white in the right,take the right route as legal route
				shift = left;
				extractLeftLine(RshiftPT);
			}
			else{	// same white count
				// use the previous data or further process the image
			}
		}
		else{	//both side no white, all black
				// use the previous data or further process the image
		}

	}


}

bool CamHandler::getBit(int16_t temp){
	target = temp/8;
	offset = temp%8;
//	return (intermediateByte>>offset) %2;
	return 0;

}

void CamHandler::convertBit(){
	int curByte = 0;
	for(int i = 0; i< ImageSize;i++){
		curByte = RawData[i];
		for(int j = 7; j >= 0; j--){
			intermediate[8*i+(7-j)] = (curByte >> j) % 2;
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



void CamHandler::extractLeftLine(int16_t basePT){
	//basePT is the point which is White and going to turn Black
	LlineType = Straight;
	int16_t relativePT = basePT;	// the leftLine[i] will store distance from base point
/*	if(shift == middle){	//that means must be some white in the right
		leftLine[0] = 0;
		for(int i = 1; i<60;i++){
			leftLine[i] = leftLine[i-1];
			relativePT = relativePT -80;
			 outer loop->loop for every white pixel
			 * inner loop->loop for scanning connected white pixel
			if(intermediate[relativePT]){	//must be white for the right part, since there are suppose to exist a right edge
				for(int j = 0; j < RangeOfSearchPT; j++){
					if(!intermediate[relativePT-j-1]){
						relativePT -= j;
						leftLine[i] -= j;
						break;
					}
				}
				 if go here,then no point is located
				// if no point if located:
				LlineType = unknownType;

			}
			else{	// if the upper point is black, the left side is not possible to be white.
					// Thus only need to scan for the left part, just to search for white in the right
				for(int j = 1; j <= RangeOfSearchPT; j++){
					if(intermediate[relativePT+j]){
						relativePT += j;
						leftLine[i] += j;
						break;
					}
				}
				 if go here,then no point is located
				// if no point if located:
				RlineType = unknownType;
			}
		}
	}
	else {	// currently left side is black, that means the car has/nearly out of the route may still extract line for future use
		*/

	//the above is commented because it contain wrong expectation,

			for(int i = 1; i<60;i++){
			leftLine[i] = leftLine[i-1];
			relativePT = relativePT -80;
			if(intermediate[relativePT]){	//must be white for the right part, since there are suppose to exist a right edge
				/* outer loop->loop for every white pixel
				 * inner loop->loop for scanning connected white pixel */
				for(int j = 0; j < RangeOfSearchPT; j++){
					if(!intermediate[relativePT-j-1]){
						relativePT -= j;
						leftLine[i] -= j;
						break;
					}
					else if(!intermediate[relativePT+j+1]){
						relativePT += j;
						leftLine[i] += j;
						break;
					}
				}
				/* if go here,then no point is located */
				// if no point if located:
				LlineType = unknownType;
				LbreakPT = i;
				return;

			}
			else{	// if the upper point is black, the left side is not possible to be white.
					// Thus only need to scan for the left part, just to search for white in the right
				for(int j = 1; j <= RangeOfSearchPT; j++){
					if(intermediate[relativePT+j]){
						relativePT += j;
						leftLine[i] += j;
						break;
					}
					else if(intermediate[relativePT-j]){
						relativePT -= j;
						leftLine[i] -= j;
						break;
					}
				}
				/* if go here,then no point is located */
				// if no point if located:
				LlineType = unknownType;
				LbreakPT = i;
				return;
			}
		}
			LlineType = full;

	}

void CamHandler::extractRightLine(int16_t basePT){
	RlineType = Straight;
	//basePT is the point which is White and going to turn Black
	int16_t relativePT = basePT;	// the rightLine[i] will store distance from base point
	for(int i = 1; i<60;i++){
		rightLine[i] = rightLine[i-1];
		relativePT = relativePT -80;
		if(intermediate[relativePT]){
			/* outer loop->loop for every white pixel
			 * inner loop->loop for scanning connected white pixel */
			for(int j = 0; j < RangeOfSearchPT; j++){
				if(!intermediate[relativePT-j-1]){
					relativePT -= j;
					rightLine[i] -= j;
					break;
				}
				else if(!intermediate[relativePT+j+1]){
					relativePT += j;
					rightLine[i] += j;
					break;
				}
			}
			/* if go here,then no point is located */
			// if no point if located:
			RlineType = unknownType;
			RbreakPT = i;
			return;
		}
		else{
			for(int j = 1; j <= RangeOfSearchPT; j++){
				if(intermediate[relativePT+j]){
					relativePT += j;
					rightLine[i] += j;
					break;
				}
				else if(intermediate[relativePT-j]){
					relativePT -= j;
					rightLine[i] -= j;
					break;
				}
			}
			/* if go here,then no point is located */
			// if no point if located:
			RlineType = unknownType;
			RbreakPT = i;
			return;
		}
	}
	RlineType =	full;
}

void CamHandler::lineProcess(){
	if(shift == middle){	// both side is expected to have lines
		if(RlineType == unknownType && RlineType != unknownType){

		}
	}
}

CamHandler::Case CamHandler::imageProcess(){
	extractBase();

	return routeCase;
}



/* given up for this: */
void CamHandler::camCorrection(){

}


void CamHandler::camCorrectionInit(int16_t x_size,int16_t y_size){
	for(int i = x_size/-2; i < x_size/2; i++){
		for(int j = y_size/2; j < y_size/-2; j--){

		}

	}
}
/* above not ready for use */
