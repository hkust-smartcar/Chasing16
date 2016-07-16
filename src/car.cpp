/*
  * car.cpp
 *
 *  Created on: 2016¦~1¤ë16¤é
 *      Author: yungc
 */
#include "car.h"
#include "config.h"
#include <cstring>
#include <memory>
#include <sstream>
#include <string>

Car::Car():
	cam(GetCameraConfig()),
	Led1(GetLed1Config()),
	Led2(GetLed2Config()),
	Led3(GetLed3Config()),
	Led4(GetLed4Config()),
	encoder(GetAbEncoderConfig()),
	servo(GetServoConfig()),
	motor(GetDirmotorConfig()),
	button1(GetButton1Config()),
	button2(GetButton2Config()),
	joystick(GetJoystickConfig()),
	LCD(GetLcdConfig()),
	buzzer(GetBuzzerConfig()),
	LCDwriter(GetLcdWriterConfig(&LCD))
	{

	cam.Start();


//	LcdConsole::Config LCDCConfig;
//	LCDCConfig.lcd = LCD;
//	LCDconsole = new LcdConsole(LCDCConfig);
	// LCDconsole will bring break point for unknown reason after line:new cellbuffer

	/***********************************variable below**********************************/
	image_size = cam.GetH() * cam.GetW() / 8;
}

Car::~Car(){
	cam.Stop();
}

void Car::printvalue(int32_t value, uint16_t color){
	LCD.SetRegion(libsc::Lcd::Rect(0,0,128,40));
	sprintf(haha,"%d", value);
	LCDwriter.SetTextColor(color);
	LCDwriter.WriteString(haha);
}
//
//void Car::printvalue(int16_t x,int16_t y,int16_t w,int16_t h,int16_t value, uint16_t color){
//	LCD.SetRegion(libsc::Lcd::Rect(x,y,w,h));
//	std::string Result;
//	std::ostringstream convert;
//	convert << value;
//	Result = convert.str();
//	const char *s = Result.c_str();
//	LCDwriter.SetTextColor(color);
//	LCDwriter.WriteString(s);
//}
//
void Car::printCar(std::string Result, int8_t line){
	LCD.SetRegion(libsc::Lcd::Rect(0,line,128,40));
	const char *s = Result.c_str();
	LCDwriter.WriteString(s);
}
//
void Car::printvalue(std::string Result){
	LCD.SetRegion(libsc::Lcd::Rect(0,0,128,40));
	const char *s = Result.c_str();
	LCDwriter.WriteString(s);
}
//
//void Car::printvalue(int16_t x,int16_t y,int16_t w,int16_t h,std::string Result){
//	LCD.SetRegion(libsc::Lcd::Rect(x,y,w,h));
//	const char *s = Result.c_str();
//	LCDwriter.WriteString(s);
//}
//
//void Car::printline(int16_t value ,uint16_t color){
//	LCD.SetRegion(libsc::Lcd::Rect(0,value * 160 / 255,128,1));
//	LCD.FillColor(color);
//}

void Car::printRawCamGraph(Uint x, Uint y){//directly print Car's private 'data[600]' at(x,y)
	LCD.SetRegion(Lcd::Rect(x,y,80,60));
	LCD.FillBits(0,0xFFFF,data,80*60);
}

bool Car::updateCam(){
	if (cam.IsAvailable()){
		memcpy(data,cam.LockBuffer(),image_size);
		cam.UnlockBuffer();
		return true;
	}
	else return false;
}

Byte* Car::get_raw_image(){
	updateCam();
	return data;

}

void Car::clearLcd (uint16_t color){
	LCD.Clear(color);
}

void Car::blinkLED(int8_t id, int32_t delay_time, int32_t persist_time){
	libsc::Led* LedToBlink;
	switch(id){
	case 1:
		LedToBlink = &Led1;
		break;
	case 2:
		LedToBlink = &Led2;
		break;
	case 3:
		LedToBlink = &Led3;
		break;
	case 4:
		LedToBlink = &Led4;
		break;
	}
	LedToBlink->SetEnable(true);
	while ( (persist_time-delay_time) >= 0){
		LedToBlink->Switch();
		libsc::System::DelayMs(delay_time);
		persist_time -= delay_time;
	}
	LedToBlink->SetEnable(false);

}

void Car::beepbuzzer(bool tobeep){
	buzzer.SetBeep(tobeep);
}

void Car::switchLED(int8_t id){
	switch(id){
	case 1:
		Led1.Switch();
		break;
	case 2:
		Led2.Switch();
		break;
	case 3:
		Led3.Switch();
		break;
	case 4:
		Led4.Switch();
		break;
	}
}

bool Car::getbutton(int8_t id){
	if(id ==1) return button1.IsDown();
	else if (id == 2) return button2.IsDown();
	else return 0;
}

libsc::Joystick::State Car::getjoystick(){
	return joystick.GetState();
}


