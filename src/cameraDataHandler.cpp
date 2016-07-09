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
				extractLeftLine(i+1);
				leftLocated = true;
				break;
			}
		}
		for(int i = 4600; i < 4640; i++){
			if(intermediate[i]){
				extractRightLine(i-1);
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
				if(LshiftPT == -1) LshiftPT = 4599-i;
			}

			if(!intermediate[4600+i]){
				RwhiteCount++;
				if(RshiftPT == -1) RshiftPT = 4600+i;
			}
		}

		// then identify line type and call extract line to have deep identification
		if(RshiftPT != -1 && LshiftPT == -1){	//left side no white point, then the line in the right is the left line
			shift = right;
			extractLeftLine(RshiftPT);
			extractRightLine(4639);
			return;
		}
		else if(RshiftPT == -1 && LshiftPT != -1){	//right side no white point
			shift = left;
			extractRightLine(LshiftPT);
			extractLeftLine(4560);
			return;

		}
		else if(RshiftPT != -1 && LshiftPT != -1){	//both side have white point
			if(LwhiteCount > RwhiteCount){	// more white in the left,take the left route as legal route
				shift = CRight;
				extractRightLine(LshiftPT);
				extractLeftLine(4639);
				return;
			}
			else if(LwhiteCount < RwhiteCount){	//more white in the right,take the right route as legal route
				shift = CLeft;
				extractLeftLine(RshiftPT);
				extractRightLine(4560);
				return;
			}
			else{	// same white count
				// use the previous data or further process the image
				shift = Unrecongize;
			}
		}
		else{	//both side no white, all black
				// use the previous data or further process the image
		}
		shift = Unrecongize;
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
	LbreakPT = -1;
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
	for(int i = 1; i<57;i++){
		reachBound = false;
		leftLine[i] = leftLine[i-1];
		relativePT = relativePT -80;
		if(!intermediate[relativePT]){	//must be white for the right part, since there are suppose to exist a right edge
			/* outer loop->loop for every white pixel
			 * inner loop->loop for scanning connected white pixel */
			//current is white
			for(int j = 0; j < RangeOfSearchPT; j++){
				if(relativePT%80 == 0){//current at the bound of the image
					//white above white
//					leftLine[i] = 0;
					break;	// done for this row, go to the above row
				}
				else{
					if((relativePT-j)%80 == 0){	// (relativePT -1) is another row
						reachBound = true;
					}
					if(intermediate[relativePT-j] && !reachBound){
						relativePT -= j;
						leftLine[i] -= j;
						break;
					}
					else if(intermediate[relativePT+j]){
						relativePT += j;
						leftLine[i] += j;
						break;
					}else if(reachBound){
						relativePT -= j;
						leftLine[i] -= j;
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
	RbreakPT = -1;
	RlineType = Straight;
	//basePT is the point which is White and going to turn Black
	int16_t relativePT = basePT;	// the rightLine[i] will store distance from base point
	bool reachBound;
	rightLine[0] = 0;
	for(int i = 1; i<57;i++){
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
//					rightLine[i] = 0;
					break;	// done for this row, go to the above row
				}
				else{
					if(((relativePT+j)%80) == 79){	// (relativePT +1) is another row
						reachBound = true;
					}
					if(intermediate[relativePT+j] && !reachBound){
						relativePT += j;
						rightLine[i] += j;
						break;
					}
					else if(intermediate[relativePT-j]){
						relativePT -= j;
						rightLine[i] -= j;
						break;
					}else if(reachBound){
						relativePT += j;
						rightLine[i] += j;
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
	if(shift == Unrecongize) {
		routeCase = CantRecognize;
		return;
	}
	//version 2, do all process then separate case
	//init all variable
	/* process left line first */
	leftZeroContinue = true;
	leftDirectionInit = false;
	leftSum = 0;
	leftZeroCount = 1;
	leftContactEdge = 0;
	leftVertex = 0;
	leftVertexFirst = -1;
	leftSumUntilVertex = 0;
	leftSumUVright = 0;
	if(LlineType == broken)	leftLineEnd = LbreakPT+1;
	else leftLineEnd = 58;

	if(!(LBasePT %80))	leftStartFromEdge = true;
	else leftStartFromEdge = false;

	for(int i = 1; i < leftLineEnd; i++){
		if(!((LBasePT-80*i+leftLine[i]) %80)){	// lies on the left edge
			leftContactEdge++;
		}
		// filter those zero
		if(!leftLine[i] && leftZeroContinue){
			leftZeroCount++;
			continue;
		}
		else{
			leftZeroContinue = false;
		}
		//count the sum of the relative shift and find out the vertex, for vertex, it should turns more than 2 unit
		leftSum += leftLine[i]; // add all shift
		/* only once below */
		if(!leftDirectionInit){	// init direction of turing
			if(leftLine[i] > 0) leftDirection = true;
			else leftDirection = false;
			leftDirectionInit = true;
			continue;
		}
		/* only once above */
		if(leftLine[i] > 0 && leftDirection){	// positive shift toward right
			if((leftLine[i] - leftLine[i-1]) < -1 || (leftLine[i] - leftLine[i-2]) < -1){	//curr - prev, if +ve, the shift continue, else vertex appear
				leftDirection = false;
				leftVertex ++;
				leftVertexArr[leftVertex-1] = i;
			}
		}
		else if(leftLine[i] > 0 && !leftDirection){	//positive shift toward left
			if((leftLine[i] - leftLine[i-1]) > 1 || (leftLine[i] - leftLine[i-2]) > 1 ){
				leftDirection = true;
				leftVertex ++;
				leftVertexArr[leftVertex-1] = i;
			}
		}
		else if(leftLine[i] < 0 && !leftDirection){	//negative shift toward left
			if((leftLine[i-1] - leftLine[i]) < -1 || (leftLine[i-2] - leftLine[i]) < -1){
				leftDirection = true;
				leftVertex ++;
				leftVertexArr[leftVertex-1] = i;
			}
		}
		else if(leftLine[i] < 0 && leftDirection){	//negative shift toward right
			if((leftLine[i-1] - leftLine[i]) > 1 || (leftLine[i-2] - leftLine[i]) > 1){
				leftDirection = false;
				leftVertex ++;
				leftVertexArr[leftVertex-1] = i;
			}
		}
		if(leftVertex == 1 && leftVertexFirst == -1) {
			leftVertexFirst = i;
		}
	}
	leftRelativeSum = leftSum / (leftLineEnd-leftZeroCount);
	leftZero = leftLineEnd - leftZeroCount;
	if(leftVertexFirst != -1){
		for(int8_t i = 0; i< leftVertexFirst ; i++){
			leftSumUntilVertex += leftLine[i];
			leftSumUVright += rightLine[i];
		}
	}

	/* process right line then */
	rightZeroContinue = true;
	rightDirectionInit = false;
	rightSum = 0;
	rightZeroCount = 1;
	rightContactEdge = 0;
	rightVertex = 0;
	rightVertexFirst = -1;
	rightSumUntilVertex = 0;
	rightSumUVleft = 0;
	if(RlineType == broken)	rightLineEnd = RbreakPT+1;
	else rightLineEnd = 58;

	if((RBasePT %80) == 79)	rightStartFromEdge = true;
	else rightStartFromEdge = false;

	for(int i = 1; i < rightLineEnd; i++){
		if(!((RBasePT-80*i+rightLine[i]) %79)){	// lies on the left edge
			rightContactEdge++;
		}
		// filter those zero
		if(!rightLine[i] && rightZeroContinue){
			rightZeroCount++;
			continue;
		}
		else{
			rightZeroContinue = false;
		}
		//count the sum of the relative shift and find out the vertex, for vertex, it should turns more than 2 unit
		rightSum += rightLine[i]; // add all shift
		/* only once below */
		if(!rightDirectionInit){	// init direction of turing
			if(rightLine[i] > 0) rightDirection = true;
			else rightDirection = false;
			rightDirectionInit = true;
			continue;
		}
		/* only once above */
		if(rightLine[i] > 0 && rightDirection){	// positive shift toward right
			if((rightLine[i] - rightLine[i-1]) < -3 ||  (rightLine[i] - rightLine[i-2]) < -3){	//curr - prev, if +ve, the shift continue, else vertex appear
				rightDirection = false;
				rightVertex ++;
				rightVertexArr[rightVertex-1] = i;
			}
		}
		else if(rightLine[i] > 0 && !rightDirection){	//positive shift toward left
			if((rightLine[i] - rightLine[i-1]) > 1 || (rightLine[i] - rightLine[i-2]) > 1){
				rightDirection = true;
				rightVertex ++;
				rightVertexArr[rightVertex-1] = i;
			}
		}
		else if(rightLine[i] < 0 && !rightDirection){	//negative shift toward left
			if((rightLine[i-1] - rightLine[i]) < -1 || (rightLine[i-2] - rightLine[i]) < -1){
				rightDirection = true;
				rightVertex ++;
				rightVertexArr[rightVertex-1] = i;
			}
		}
		else if(rightLine[i] < 0 && rightDirection){	//negative shift toward right
			if((rightLine[i-1] - rightLine[i]) > 1 || (rightLine[i-2] - rightLine[i]) > 1){
				rightDirection = false;
				rightVertex ++;
				rightVertexArr[rightVertex-1] = i;
			}
		}
		if(rightVertex == 1 && rightVertexFirst == -1) {
			rightVertexFirst = i;
		}
	}
	rightRelativeSum = rightSum / (rightLineEnd-rightZeroCount);
	rightZero = rightLineEnd - rightZeroCount;
	if(rightVertexFirst != -1){
		for(int8_t i = 0; i< rightVertexFirst ; i++){
			rightSumUntilVertex += rightLine[i];
			rightSumUVleft += leftLine[i];
		}
	}

	/* now return case */
	if(shift == middle){

		if((leftVertex > 2 && rightVertex > 0 ) || (leftVertex > 0 && rightVertex > 2 ) ||
				(leftVertex >2 && (rightZero < 5 && rightZero > -1))
				|| (rightVertex > 2 && leftZero < 5 && leftZero > -1)){
			routeCase = SFront;
			return;
		}
		//new add to fix some bug, than the sight of the camera is too far, it should be more localize
		if(leftVertex > 0 ||rightVertex >0){
			if((RbreakPT >= LbreakPT  || RbreakPT == -1)&& (leftSumUVright -leftSumUntilVertex) < -400  && leftSumUVright >-50){	// vertax for left edge
				routeCase = InRightCurve;
				return;
			}
			if((RbreakPT <= LbreakPT || LbreakPT == -1) && (rightSumUntilVertex + rightSumUVleft) < -400 && rightSumUVleft <50){	// vertax for left edge
				routeCase = InLeftCurve;
				return;
			}
			if(rightVertexFirst > leftVertexFirst  && leftVertexFirst > 20&& ((leftSumUntilVertex + leftSumUVright) < 5  || (leftSumUntilVertex + leftSumUVright) > -5 )){
				routeCase = StraightRoute;
				return;
			}
			if(rightVertexFirst < leftVertexFirst  && rightVertexFirst > 20&& ((rightSumUntilVertex + rightSumUVleft) < 5  || (rightSumUntilVertex + rightSumUVleft) > -5 )){
				routeCase = StraightRoute;
				return;
			}
		}


		//special process for cross road
		if((checkWhite() > 5 && (leftSum > rightSum ) && (rightStartFromEdge || leftStartFromEdge)) ||
				(rightStartFromEdge && leftStartFromEdge && checkLine(1) && checkLine(0) && (checkWhite() >4))){
			if((leftVertex <2 && rightVertex <1) ||(leftVertex <1 && rightVertex <2)){
				if(leftSum > -20 && leftSum <20 && rightSum < -550) {
					routeCase = InLeftBigCurve;
					return;
				}
				if(rightSum > -20 && rightSum <20 && leftSum > 550) {
					routeCase = InRightBigCurve;
					return;
				}
			}

			routeCase = CrossRoute;
			return;

		}

		if((leftSum > rightSum && rightSum <= 0 && (leftSum < 0 || (leftZero < 5 && leftZero > -1)) )
				|| (rightSum <= 0 && leftSum >0 && leftVertex > 0 && rightVertex < 1)){
			routeCase = InLeftCurve;
			if(leftZero < 5 && leftZero > -1)routeCase = InLeftBigCurve;
			return;
		}
		if((leftSum > rightSum && leftSum > 0 && (rightSum > 0 || (rightZero < 5 && rightZero > -1)) )
				||(rightSum <= 0 && leftSum > 0 && rightVertex > 0 && leftVertex < 1)){
			routeCase = InRightCurve;
			if(rightZero < 5 && rightZero > -1)routeCase = InRightBigCurve;
			return;
		}
		if(((rightRelativeSum + leftRelativeSum) < 5 ||(rightRelativeSum + leftRelativeSum) > -5)){
			// future improvement to assure its a straight line
			routeCase = StraightRoute;
			return;
		}
	}
	else if(shift == left || shift == CLeft){
		routeCase = InLeftBigCurve;
		return;
	}
	else if(shift == right || shift == CRight){
		routeCase = InRightBigCurve;
		return;
	}
	routeCase = CantRecognize;

}

int8_t CamHandler::checkWhite(){
	checkWhiteCounter = 0;
	int8_t tempCounter = 0;
	int8_t temp = 0;
	for(int8_t j=0; j<60;j++){
		for(int16_t i = 0;i<10 ;i++){
			if(RawData[i+10*j] == 0){
				tempCounter++;
				if(tempCounter == 10) {
					checkWhiteCounter++;
					tempCounter = 0;
					break;
				}
			}
			else {
				tempCounter = 0;
				break;
			}
		}
	}
	return checkWhiteCounter;
}

bool CamHandler::checkLine(bool isLeft){
	int16_t relativePTL = 4560;
	int16_t relativePTR = 4639;
	int8_t lineCounterL =0;
	int8_t lineCounterR =0;

	bool pointLocated = false;
	bool lineExist = false;
	if(isLeft){
		for(int8_t i=0; i< 58 ;i++){
			//4560 to 4639
			relativePTL = 4560-80*i;
			if(!intermediate[relativePTL]){
				continue;
			}
			if(!pointLocated){
				for(int8_t j = 0; j<40;j++){
					if(!intermediate[relativePTL+j]){
						relativePTL = relativePTL+j;
						pointLocated =true;
						break;
					}
				}
				if(!pointLocated)return false; // no point exist
				continue;
			}
			if(!intermediate[relativePTL]){
				for(int8_t k = 1; k<=3;k++){
					if(intermediate[relativePTL-k]){
						relativePTL -= k+1;
						lineCounterL++;
						break;
					}
					if(intermediate[relativePTL+k]){
						relativePTL += k-1;
						lineCounterL++;
						break;
					}
					if(k == 3 ){
						if(lineCounterL >4) return true;
						else return false;
					}
				}
			}
			else{
				for(int8_t k = 1; k<=3;k++){
					if(intermediate[relativePTL+k]){
						relativePTL += k-1;
						lineCounterL++;
						break;
					}
					if(k == 3 ){
						if(lineCounterL >4) return true;
						else return false;
					}
				}
			}
		}
		if(lineCounterL >4) return true;
		else return false;
	}
	else if(!isLeft){
		for(int8_t i=0; i< 58;i++){
			//4560 to 4639
			relativePTR = 4639-80*i;
			if(!intermediate[relativePTR]){
				continue;
			}
			if(!pointLocated){
				for(int8_t j = 0; j<40;j++){
					if(!intermediate[relativePTR+j]){
						relativePTR = relativePTR+j;
						pointLocated =true;
						break;
					}
				}
				if(!pointLocated)return false; // no point exist
				continue;
			}
			if(!intermediate[relativePTR]){
				for(int8_t k = 1; k<=3;k++){
					if(intermediate[relativePTR-k]){
						relativePTR -= k+1;
						lineCounterR++;
						break;
					}
					if(intermediate[relativePTR+k]){
						relativePTR += k-1;
						lineCounterR++;
						break;
					}
					if(k == 3 ){
						if(lineCounterR >4) return true;
						else return false;
					}
				}
			}
			else{
				for(int8_t k = 1; k<=3;k++){
					if(intermediate[relativePTR+k]){
						relativePTR += k-1;
						lineCounterR++;
						break;
					}
					if(k == 3 ){
						if(lineCounterR >4) return true;
						else return false;
					}
				}
			}
		}
		if(lineCounterR >4) return true;
		else return false;
	}

}

void CamHandler::updateRawData(Byte* rawData){
	RawData = rawData;
}


CamHandler::Case CamHandler::imageProcess(){
	extractBase();
	lineProcess();
	return routeCase;
}

int8_t CamHandler::getMidPT(){
	int16_t temp = -20, counter = 0, temp1 = 20;
	prevMidPT = midPT;
		if(routeCase == StraightRoute){
			leftRelativePT = LBasePT%80;
			rightRelativePT = RBasePT%80;
			midPT = (leftRelativePT + rightRelativePT)/2;
		}else if(routeCase == InLeftCurve || routeCase == InRightCurve){
			leftRelativePT = LBasePT%80 + leftLine[10];
			rightRelativePT = RBasePT%80 + rightLine[10];
			midPT = (leftRelativePT + rightRelativePT)/2;
		}else if(routeCase == InLeftBigCurve || routeCase == InRightBigCurve){
			leftRelativePT = LBasePT%80;
			rightRelativePT = RBasePT%80;
			midPT = (leftRelativePT + rightRelativePT)/2;
		}else if(routeCase == SFront){
			if(leftVertexFirst != -1 && rightVertexFirst != -1){
				if(leftVertex == 1)leftRelativePT = LBasePT%80 + leftLine[leftVertexFirst];
				else{
					for(int8_t i = leftVertex; i >0; i--){
						if (leftLine[leftVertexArr[i-1]] > temp) temp = leftLine[leftVertexArr[i-1]];
					}
					leftRelativePT = LBasePT%80 + temp;
				}

				if(rightVertex == 1) rightRelativePT = RBasePT%80 + rightLine[rightVertexFirst];
				else{
					for(int8_t i = rightVertex; i>0; i--){
						if (rightLine[rightVertexArr[i-1]] < temp) temp = leftLine[leftVertexArr[i-1]];
					}
					leftRelativePT = LBasePT%80 + temp;
				}

				if((rightRelativePT - leftRelativePT) > 20){
					midPT = (leftRelativePT + rightRelativePT)/2;
				}
				else{
					leftRelativePT = LBasePT%80 + leftLine[5];
					rightRelativePT = RBasePT%80 + rightLine[5];
					midPT = (leftRelativePT + rightRelativePT)/2;
				}
			}

		}
		else if(routeCase == CrossRoute ){
			leftRelativePT = LBasePT%80;
			rightRelativePT = RBasePT%80;
			midPT = (leftRelativePT + rightRelativePT)/2;
		}
		else{
		midPT = prevMidPT;
		}

	return midPT;
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

/*//version 1 decide case, have unknown bug to fix
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
/* above not ready for use */

