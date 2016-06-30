/*

 * cameraDataHandler.cpp
 *
 *  Created on: 2016¦~6¤ë3¤é
 *      Author: yungc
 */

#include "cameraDataHandler.h"
#include <cstring>

CamHandler::CamHandler(){

}

void CamHandler::extractBase(){
	convertBit();
	bool middleState = intermediate[4599];
	bool leftLocated = false, rightLocated = false;


	if(!middleState){	// currently   XXXXXXXXBBBWWWWWWWWWWWBBBXXXXXXX
		shift = middle;	//first identify straight
		for(int i = 4598; i>= 4560; i--){
			if(intermediate[i]){
				extractLeftLine(i);
				leftLocated = true;
				break;
			}
		}
		for(int i = 4600; i < 4640; i++){
			if(intermediate[i]){
				extractRightLine(i);
				rightLocated = true;
				break;
			}
		}
		if(!leftLocated && !rightLocated){
			extractLeftLine(4560);
			extractRightLine(4639);
		}else if(!leftLocated && rightLocated){
			extractLeftLine(4560);
		}else if(leftLocated && !rightLocated){
			extractRightLine(4639);
		}
	}
	else{	// currently at black
			// some case may lead to this situation:
			// 1) a block is watched
			// 2) + route
			// 3) route out of bound
		int8_t 	LwhiteCount = 0,
				RwhiteCount = 0;
		int16_t	LshiftPT = -1,//check -1 for not init
				RshiftPT = -1;//check -1 for not init

		for(int i=0; i<40;i++){
			if(!intermediate[4599-i]){
				LwhiteCount++;
				if(LshiftPT == -1) LshiftPT = 4599-i+1;
			}

			if(!intermediate[4600+i]){
				RwhiteCount++;
				if(RshiftPT == -1) RshiftPT = 4600+i-1;
			}
		}

		// then identify line type and call extract line to have deep identification
		if(RshiftPT != -1 || LshiftPT == -1){	//left side no white point, then the line in the right is the left line
			shift = left;
			extractLeftLine(RshiftPT);
			extractRightLine(4639);
		}
		else if(RshiftPT == -1 || LshiftPT != -1){	//right side no white point
			shift = right;
			extractRightLine(LshiftPT);
			extractLeftLine(4560);

		}
		else if(RshiftPT != -1 || LshiftPT != -1){	//both side have white point
			if(LwhiteCount > RwhiteCount){	// more white in the left,take the left route as legal route
				shift = CRight;
				extractRightLine(LshiftPT);
			}
			else if(LwhiteCount < RwhiteCount){	//more white in the right,take the right route as legal route
				shift = CLeft;
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

bool CamHandler::getcertainBit(int16_t temp){
	target = temp/8;
	offset = 7 - temp%8;
	return (RawData[target]>>offset) %2;
	return 0;
}

void filterBase3Line(){

}

void CamHandler::convertBit(){
	int curByte = 0;
	int counter = 0;
	for(int i = 0; i< ImageSize;i++){
		curByte = RawData[i];

		if(curByte == 0){
			counter = 8;
			while(counter){
				counter--;
			intermediate[8*i+counter] = 0;
			}
		}
		else if(curByte == 255){
			counter = 8;
			while(counter){
				counter--;
			intermediate[8*i+counter] = 1;
			}
		}
		else{
			for(int j = 7; j >= 0; j--){
			intermediate[8*i+(7-j)] = (curByte >> j) % 2;
		}
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
	PrevLBasePT = LBasePT;
	LBasePT = basePT;
	LlineType = Straight;
	int16_t relativePT = basePT;	// the leftLine[i] will store distance from base point
	bool reachBound;
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
	leftLine[0] = 0;
	for(int i = 1; i<58;i++){
		reachBound = false;
		leftLine[i] = leftLine[i-1];
		relativePT = relativePT -80;
		if(!intermediate[relativePT]){	//must be white for the right part, since there are suppose to exist a right edge
			/* outer loop->loop for every white pixel
			 * inner loop->loop for scanning connected white pixel */
			//current is white
			for(int j = 0; j < RangeOfSearchPT; j++){
				if(!relativePT%80){//current at the bound of the image
					//white above white
					leftLine[i] = 0;
					break;	// done for this row, go to the above row
				}
				else{
					if(intermediate[relativePT-j] && !reachBound){
						if(!(relativePT-j)%80){	// (relativePT -1) is another row
							reachBound = true;
						}
						relativePT -= j;
						leftLine[i] -= j;
						break;
					}
					else if(intermediate[relativePT+j]){
						relativePT += j;
						leftLine[i] += j;
						break;
					}
				}
				if(j == (RangeOfSearchPT-1)){	// broken line
					LlineType = broken;
					LbreakPT = i;
					return;
				}
			}
//			/* wont go here, just to ensure */
//			LlineType = unknownType;
//			LbreakPT = i;
//			return;
			}
		else{	// if the upper point is black, the left side is not possible to be white.
				// Thus only need to scan for the left part, just to search for white in the right
				for(int j = 1; j < RangeOfSearchPT; j++){
				if(!intermediate[relativePT+j] ){
					relativePT += j;
					leftLine[i] += j;
					break;
				}
//				else if(!intermediate[relativePT-j] && !(relativePT-j)%80){
//					relativePT -= j;
//					leftLine[i] -= j;
//					break;
//				}
					if(j == (RangeOfSearchPT-1)){	// broken line
					LlineType = broken;
					LbreakPT = i;
					return;
				}
			}
		}
	}
	LlineType = full;

	}

void CamHandler::extractRightLine(int16_t basePT){
	PrevRBasePT = RBasePT;
	RBasePT = basePT;
	RlineType = Straight;
	//basePT is the point which is White and going to turn Black
	int16_t relativePT = basePT;	// the rightLine[i] will store distance from base point
	bool reachBound;
	rightLine[0] =0;
	for(int i = 1; i<58;i++){
		reachBound = false;
		rightLine[i] = rightLine[i-1];
		relativePT = relativePT -80;
		if(!intermediate[relativePT]){	//above is white
			/* outer loop->loop for every white pixel
			 * inner loop->loop for scanning connected white pixel */
			for(int j = 0; j < RangeOfSearchPT; j++){
				if(((relativePT)%80) == 79){
					//current at the bound of the image
					//white above white
					rightLine[i] = 0;
					break;	// done for this row, go to the above row
				}
				else{
					if(intermediate[relativePT+j] && !reachBound){
						if(((relativePT+j)%80) == 79){	// (relativePT +1) is another row
							reachBound = true;
						}
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
				if(j == (RangeOfSearchPT-1)){	// broken line
					RlineType = broken;
					RbreakPT = i;
					return;
				}
			}
		}
		else{
			for(int j = 1; j < RangeOfSearchPT; j++){
				if(!intermediate[relativePT-j]){
					relativePT -= j;
					rightLine[i] -= j;
					break;
				}
//				else if(!intermediate[relativePT-j]){
//					relativePT -= j;
//					rightLine[i] -= j;
//					return;
//				}
				if(j == (RangeOfSearchPT-1)){	// broken line
					RlineType = broken;
					RbreakPT = i;
					return;
				}
			}
		}
	}
	RlineType =	full;
}

void CamHandler::lineProcess(){
	//init all variable
	leftZeroContinue = true;
	leftDirection = true;
	leftSum = 0;
	leftZeroCount = 0;
	leftContactEdge = 0;
	leftVertex = 0;
	// process left line first
	for(int i = 1; i < 58; i++){
		// filter those zero
		if(!leftLine[i] && leftZeroContinue){
			leftZeroCount++;
			continue;
		}
		else{
			leftZeroContinue = false;
		}
		//count the sum of the relative shift and find out the vertex, for vertex, it should turns more than 2 unit


	}

	/*//version 1, have unknown bug to fix
	 * int16_t leftSum = 0, rightSum = 0;
	int8_t lcounter = 0, rcounter = 0;
	int8_t leftEdge = 0, rightEdge = 0;
	if(shift == middle){	// both side is expected to have lines
		if(RlineType == broken && LlineType == broken && RbreakPT < 50 && LbreakPT < 50){	//cant extract full line for both side, may due to + route
			//or U or C route.
			//to identify these route
			for(int i = 0;i<=57;i++){
				leftSum += leftLine[i];
				if(!leftLine[i])lcounter++;
				rightSum += rightLine[i];
				if(!rightLine[i])rcounter++;
			}
			if(leftSum <= 0 && rightSum > 10){
				if(rcounter < 2 && lcounter > 0){
					routeCase = InLeftBigCurve;
					return;
				}else{
					routeCase =  InComingLeftCurve;
					return;
				}
			}
			else if(leftSum > 10 && rightSum <= 0){
				if(rcounter > 0 && lcounter < 2){
					routeCase = InRightBigCurve;
					return;
				}else{
					routeCase =  InComingRightCurve;
					return;
				}
			}else if(rcounter>3 && lcounter >3){
				for(int i = 0; i < (58-LbreakPT);i++){
					if(intermediate[80*i]) leftEdge = i;
				}
				for(int i = 0; i < (58-RbreakPT);i++){
					if(intermediate[80*i]) rightEdge = i;
				}
				if(leftEdge < rightEdge){ // for a cross, the vertex in the left is higher than right
					routeCase = CrossRoutetoLeft;
					return;
				}
				else{
					routeCase = CrossRoutetoRight;
					return;
				}
			}
		}else if(RlineType == broken && LlineType != broken){
			routeCase = InLeftCurve;
			return;
		}else if(RlineType != broken && LlineType == broken){
			routeCase = InRightCurve;
			return;
		}else if(RlineType != broken && LlineType != broken) {
			// it could be S route or straight route
			int8_t lVertexCounter = 0, RVertexCounter = 0;
			bool lWasZero = true, RWasZero = true;
			int8_t lZeroCounter = 0, RZeroCounter = 0;
			for(int i = 1; i < 58; i++){
				if(leftLine[i] == 0){	// count the continuous zero
					if(lWasZero)
					lZeroCounter++;
					continue;
				}else if((leftLine[i-1]/leftLine[i]) < 0){	//if +ve turn -ve  or -ve turn +ve, +/- <0, i.e. vertex
					lVertexCounter++;
					continue;
				}
				if(leftLine[i] != 0) lWasZero = false;	// if zero not continue, false this flag
			}
			for(int i = 1; i < 58; i++){
				if(rightLine[i] == 0){	// count the continuous zero
					if(RWasZero)
					RZeroCounter++;
					continue;
				}else if((rightLine[i-1]/rightLine[i]) < 0){	//if +ve turn -ve  or -ve turn +ve, +/- <0, i.e. vertex
					RVertexCounter++;
					continue;
				}
				if(rightLine[i] != 0) RWasZero = false;	// if zero not continue, false this flag
			}
			if(RVertexCounter > 1 || lVertexCounter > 1 ){	//any edge have more than 3 vertex
				routeCase = SFront;
				return;
			}
			else{
				if(RZeroCounter > lZeroCounter){
					routeCase = SlightRight;
					return;
				}
				else if(RZeroCounter < lZeroCounter){
					routeCase = SlightLeft;
					return;
				}
				else{
					routeCase = StraightRoute;
					return;
				}

			}

		}
	}*/
}

void CamHandler::updateRawData(Byte* rawData){
	RawData = rawData;
}


CamHandler::Case CamHandler::imageProcess(){
	extractBase();
	lineProcess();
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
