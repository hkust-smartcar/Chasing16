/*

 * cameraDataHandler.h
 *
 *  Created on: 2016¦~5¤ë5¤é
 *      Author: yungc
 */
#include  "libbase/misc_types.h"
#pragma once


class CamHandler{
public:

	enum Case{
			StopCar = 0,StraightRoute,SlightLeft,SlightRight,InLeftCurve,InRightCurve,
		SFront,CrossRoute,InComingLeftCurve,InComingRightCurve,
		InRightBigCurve,InLeftBigCurve,NotInit,CantRecognize
	};
	/* use method:
	 * create CamHandler object
	 * update updateRawData()
	 * obtain Case by imageProcess()
	 * depends on the case, if more information is needed, call those getXXXXX() function
	 */
	Case imageProcess();

	CamHandler();

	void convertBit();
	bool getcertainBit(int16_t);
	void camCorrection();	//not ready
	void camCorrectionInit(int16_t x_size, int16_t y_size);	//not ready
	void extractBase();
	void extractLeftLine(int16_t basePT);
	void extractRightLine(int16_t basePT);
	void lineProcess();
	int8_t checkWhite();
	bool checkLine(bool isLeft);
	void filterBase3Line();
	int8_t getError(Case turnCase);
	void updateRawData(Byte* rawData);
	Byte* getRawData(){return RawData;}
	void setImageSize(int16_t imageSize){ImageSize = imageSize;}
	int16_t getImageSize(){return ImageSize;}

private:
	int8_t checkWhiteCounter = 0;
	int16_t ImageSize = 600;
	Byte* RawData = nullptr;
	Byte* intermediateByte = nullptr;
	bool intermediate[4800];
	int16_t k_size = 0;
	int16_t k_rate = 0;
	int8_t x_size = 0;
	int8_t y_size = 0;

	/* getBit */
	int16_t target = -1;
	int16_t offset = -1;
	/* extractBase */
	int16_t	leftX = -1;		//illegal -1 imply not init
	int16_t rightY = -1;	//illegal -1 imply not init
	int8_t BWB_count = -1;	//illegal -1 imply not init, its the count of routes
	int8_t WBW_count = -1;	//illegal -1 imply not init, its the count of diverse routes
	int8_t shiftPT[80] ={0};
	enum Shift{
		left = 0,middle,right,ULeft,URight,CLeft,CRight,Stop
	};
	Shift shift = middle;

	/* extractLine */
	int8_t leftLine[57]={0};
	int8_t rightLine[57]={0};
	enum LineType{
		Straight = 0, UType, LType, SType, CType, broken, full
	};
	LineType LlineType = Straight;
	LineType RlineType = Straight;
	int16_t RbreakPT = -1;
	int16_t LbreakPT = -1;
	int8_t RangeOfSearchPT = 8;
	int16_t RBasePT = -1, LBasePT = -1;
	int16_t PrevRBasePT = -1, PrevLBasePT = -1;
	bool LWhite = false, RWhite = false;

	/* lineProcess */


	Case routeCase = NotInit;

	int16_t processSize = 0;

	/* line process*/
	int16_t leftSum = 0, rightSum = 0;
	int8_t leftZeroCount = 0, rightZeroCount = 0;
	int8_t leftContactEdge = 0, rightContactEdge = 0;
	int8_t leftVertex = 0, rightVertex = 0;
	bool leftZeroContinue = true, rightZeroContinue = true;
	bool leftDirection = true, rightDirection = true;
	bool leftDirectionInit = false, rightDirectionInit = false;
	bool leftStartFromEdge = false, rightStartFromEdge = false;
	int8_t leftLineEnd = -1, rightLineEnd = -1;
	int8_t leftRelativeSum = -1, rightRelativeSum = -1;
	int8_t leftZero = 0, rightZero = 0;
	int8_t leftVertexFirst = -1, rightVertexFirst = -1;
	/* PID error */
	int8_t midLine = 40;
	int8_t error = -1;

};
