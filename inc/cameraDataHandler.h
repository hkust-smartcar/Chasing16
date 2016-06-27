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
		TurnLeftWithoutProcess = 0,TurnRightWithoutProcess,StopCar,StraightRoute,LeftURoute,RightURoute,SFront,CrossRoute,NotInit
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
	void camCorrection();	//not ready
	void camCorrectionInit(int16_t x_size, int16_t y_size);	//not ready
	void extractBase();
	void extractLeftLine(int16_t basePT);
	void extractRightLine(int16_t basePT);
	void lineProcess();
	bool getBit(int16_t);

	void updateRawData(Byte* rawData){RawData = rawData;}
	Byte* getRawData(){return RawData;}
	void setImageSize(int16_t imageSize){ImageSize = imageSize;}
	int16_t getImageSize(){return ImageSize;}

private:

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
		left = 0,middle,right,ULeft,URight,Stop
	};
	Shift shift = middle;

	/* extractLine */
	int8_t leftLine[60]={0};
	int8_t rightLine[60]={0};
	enum LineType{
		Straight = 0, UType, LType, SType, CType, unknownType, full
	};
	LineType LlineType = Straight;
	LineType RlineType = Straight;
	int16_t RbreakPT = -1;
	int16_t LbreakPT = -1;
	int8_t RangeOfSearchPT = 3;

	/* lineProcess */


	Case routeCase = NotInit;

	int16_t processSize = 0;

};
