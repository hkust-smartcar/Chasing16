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

	CamHandler();

	void covertBit();
	void camCorrection();
	void camCorrectionInit(int16_t x_size, int16_t y_size);
	void extractBase();
	void extractLeftLine(int16_t basePT);
	void extractRightLine(int16_t basePT);



	void updateRawData(Byte* rawData){RawData = rawData;}
	Byte* getRawData(){return RawData;}
	void setImageSize(int16_t imageSize){ImageSize = imageSize;}
	int16_t getImageSize(){return ImageSize;}

private:
	int16_t ImageSize = 600;
	Byte* RawData = nullptr;
	bool intermediate[4800];
	int16_t k_size = 0;
	int16_t k_rate = 0;
	int8_t x_size = 0;
	int8_t y_size = 0;

	/* extractBase */
	int16_t	leftX = -1;		//illegal -1 imply not init
	int16_t rightY = -1;	//illegal -1 imply not init
	int8_t BWB_count = -1;	//illegal -1 imply not init, its the count of routes
	int8_t WBW_count = -1;	//illegal -1 imply not init, its the count of diverse routes
	int8_t shiftPT[80] ={0};


	/* extractLine */
	int8_t leftLine[60]={0};
	int8_t rightLine[60]={0};
	enum LineType{
		Straight = 0, UType, LType
	};
	LineType lineType = Straight;
	enum Shift{
		left = 0,middle,right,ULeft,URight
	};
	Shift shift = middle;

	struct pt{
		int16_t actual_x = 65535;//65535 = not init
		int16_t actual_y = 65535;//65535 = not init
		//65535,65535 is end point
	};
	pt correctData[4800];
	int16_t processSize = 0;

};
