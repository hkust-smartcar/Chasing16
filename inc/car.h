/*
 * car.h
 *
 *  Created on: 2016¦~1¤ë16¤é
 *      Author: yungc
 */

#pragma once


#include <libsc/led.h>
#include <libbase/k60/mcg.h>
#include <libsc/ab_encoder.h>
#include <libsc/futaba_s3010.h>
#include <libsc/k60/ov7725.h>
#include <libsc/button.h>
#include <libsc/joystick.h>
#include <libsc/st7735r.h>
#include <libsc/simple_buzzer.h>
#include <string>
#include <libsc/dir_motor.h>
#include <libsc/lcd_console.h>
#include <libsc/lcd_typewriter.h>
#include <array>
#include <cstring>
#include <sstream>
#include <functional>



//

class Car{
public:
	Car();
	~Car();

	/*--------------------------------printing below------------------------------------*/
/*	What color can be use:
 * 	static constexpr uint16_t kBlack = 0x0000;
	static constexpr uint16_t kGray = 0x7BEF;
	static constexpr uint16_t kWhite = 0xFFFF;
	static constexpr uint16_t kRed = 0xF800;
	static constexpr uint16_t kYellow = 0xFFE0;
	static constexpr uint16_t kGreen = 0x07E0;
	static constexpr uint16_t kCyan = 0x07FF;
	static constexpr uint16_t kBlue = 0x001F;
	static constexpr uint16_t kPurple = 0xF81F;
	*/
	void printvalue(int32_t value, uint16_t color);

//	void printvalue(int16_t x,int16_t y,int16_t w,int16_t h,int16_t value, uint16_t color);
//
	void printvalue(std::string);
//
//	void printvalue(int16_t x,int16_t y,int16_t w,int16_t h,std::string Result);
//
//	void printline(int16_t value,uint16_t color);
//
	void printCar(std::string, int8_t);

	void printRawCamGraph(Uint x, Uint y);

	bool updateCam();

	void clearLcd (uint16_t);

	void blinkLED(int8_t id, int32_t delay_time, int32_t persist_time);

	void beepbuzzer(bool tobeep);

	void switchLED(int8_t id);

	bool getbutton(int8_t id);

	libsc::Joystick::State getjoystick();

//	int16_t otsu_threshold();
	Byte* get_raw_image();


	virtual int16_t turningPID (int16_t){return 0; };

	virtual int16_t motorPID (int16_t){return 0; };

	virtual void motor_control(uint16_t power, bool is_clockwise_rotating){ };

	virtual void servo_control(int16_t degree){ };

	virtual int32_t get_encoder_count(){return 0; };




private:
	Byte data[600];
	int16_t image_size;
	char haha[30];
protected:


	libsc::Led Led1;
	libsc::Led Led2;
	libsc::Led Led3;
	libsc::Led Led4;
	libsc::AbEncoder encoder;
	libsc::FutabaS3010 servo;
	libsc::DirMotor motor;
	libsc::Button button1;
	libsc::Button button2;
	libsc::Joystick joystick;
	libsc::St7735r LCD;
	libsc::LcdTypewriter LCDwriter;
	libsc::SimpleBuzzer buzzer;
	libsc::k60::Ov7725 cam;

};
