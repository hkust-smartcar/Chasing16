/*
 * cameraCalibration.h
 *
 *  Created on: 2016¦~5¤ë5¤é
 *      Author: yungc
 */

#pragma once


class CamHandler{
public:
	CamHandler();
	CamHandler(int16_t imageSize);




	void updateRawData(int16_t rawData){RawData = rawData;}
	int16_t getRawData(){return RawData;}
	void setImageSize(int16_t imageSize){ImageSize = imageSize;}
	int16_t getImageSize(){return ImageSize;}

private:
	int16_t ImageSize;
	Byte* RawData = new Byte[ImageSize];
	Byte* Intermediate = new Byte[ImageSize];
	Byte* TreatedData = new Byte[ImageSize];

};
