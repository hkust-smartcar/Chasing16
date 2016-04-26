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

Car::Car(){
	Led1 = new Led(GetLed1Config());
	Led2 = new Led(GetLed2Config());
	Led3 = new Led(GetLed3Config());
	Led4 = new Led(GetLed4Config());
	cam = new k60::Ov7725(GetCameraConfig());
	cam->Start();
	encoder = new AbEncoder(GetAbEncoderConfig());
	servo = new TrsD05(GetServoConfig());
//	motor = new AlternateMotor(GetAltmotorConfig());
	motor = new DirMotor(GetDirmotorConfig());
	button1 = new Button(GetButton1Config());
	button2 = new Button(GetButton2Config());
	joystick = new Joystick(GetJoystickConfig());
	LCD = new St7735r(GetLcdConfig());
	buzzer = new SimpleBuzzer(GetBuzzerConfig());

	LcdTypewriter::Config LcdWconfig;
	LcdWconfig.lcd = LCD;
	LCDwriter = new LcdTypewriter(LcdWconfig);

	LcdConsole::Config LCDCConfig;
	LCDCConfig.lcd = LCD;
	LCDconsole = new LcdConsole(LCDCConfig);

	/***********************************variable below**********************************/
	image_size = cam->GetH() * cam->GetW() / 8;
}
	// for constructor, you can temporarily understand it as :
	// initialize all variable ( & pointer)
	// for the word "new", it is to create dynamic object.
	// dynamic object means, it will always exist once you create it.
	// opposite of dynamic is static.
	//  int a; <----its static, meamory space of a will be release out the scope
	//  int* a = new int; <----its dynamic, meamory space of datatype int will be preserved, and "new int" will
	// return the address of that meamory space. Thats why we use a pointer to store the address.

Car::~Car(){

	delete Led1 ;
	delete Led2 ;
	delete Led3 ;
	delete Led4 ;
	delete encoder ;
	delete servo ;
	delete motor ;
	delete button1 ;
	delete button2 ;
	delete joystick ;
	delete LCD;
	delete buzzer ;
	delete LCDwriter;
	delete LCDconsole;
	cam->Stop();
	delete cam;

}
	// for destructor, you can understand as following
	// "how many times you "new" a object in constructor, same amount of "delete" you need to put in destructor"
	// if you "new" a object but dont "delete" it, the object will forever exist.( for more search "memory leak" )

void Car::printvalue(int16_t value, int16_t color){
	LCD->SetRegion(libsc::Lcd::Rect(0,0,128,40));
	std::string Result;
	std::ostringstream convert;
	convert << value;
	Result = convert.str();
	const char *s = Result.c_str();
	LCDwriter->SetTextColor(color);
	LCDwriter->WriteString(s);
}

void Car::printvalue(int x,int y,int w,int h,int16_t value, int16_t color){
	LCD->SetRegion(libsc::Lcd::Rect(x,y,w,h));
	std::string Result;
	std::ostringstream convert;
	convert << value;
	Result = convert.str();
	const char *s = Result.c_str();
	LCDwriter->SetTextColor(color);
	LCDwriter->WriteString(s);
}

void Car::printvalue(std::string Result){
	LCD->SetRegion(libsc::Lcd::Rect(0,0,128,40));
	const char *s = Result.c_str();
	LCDwriter->WriteString(s);
}

void Car::printvalue(int x,int y,int w,int h,std::string Result){
	LCD->SetRegion(libsc::Lcd::Rect(x,y,w,h));
	const char *s = Result.c_str();
	LCDwriter->WriteString(s);
}

void Car::printRawCamGraph(Uint x, Uint y){//directly print Car's private 'data[600]' at(x,y)
	LCD->SetRegion(Lcd::Rect(x,y,80,60));
	LCD->FillBits(0,0xFFFF,data,80*60);
}

bool Car::updateCam(){
	if (cam->IsAvailable()){
		memcpy(data,cam->LockBuffer(),image_size);
		cam->UnlockBuffer();
		return true;
	}
	else return false;
}

void Car::printline(int16_t value ,uint16_t color){
	LCD->SetRegion(libsc::Lcd::Rect(0,value * 160 / 255,128,1));
	LCD->FillColor(color);
}

void Car::clearLcd (uint16_t color){
	LCD->Clear(color);
}

void Car::blinkLED(int8_t id, int delay_time, int persist_time){
	libsc::Led* LedToBlink;
	switch(id){
	case 1:
		LedToBlink = Led1;
		break;
	case 2:
		LedToBlink = Led2;
		break;
	case 3:
		LedToBlink = Led3;
		break;
	case 4:
		LedToBlink = Led4;
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
	buzzer->SetBeep(tobeep);
}

void Car::switchLED(int8_t id){
	switch(id){
	case 1:
		Led1->Switch();
		break;
	case 2:
		Led2->Switch();
		break;
	case 3:
		Led3->Switch();
		break;
	case 4:
		Led4->Switch();
		break;
	}
}

bool Car::getbutton(int8_t id){
	if(id ==1) return button1->IsDown();
	else if (id == 2) return button2->IsDown();
	else return 0;
}

libsc::Joystick::State Car::getjoystick(){
	return joystick->GetState();
}

int16_t Car::otsu_threshold(){
	//wait for implementation
	//wait for kitty's improved(that mean fast) otsu alogrithm
	return 0;
}

