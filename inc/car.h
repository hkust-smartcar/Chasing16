/*
 * car.h
 *
 *  Created on: 2016年1月16日
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
public: // public means anyone can access it
	//there are even 公廁, that means really anyone can access it, even when the class object does not exist :O
	// if you wanna do that, just search " class friend c++"

	Car();
	// default constructor, it will be automatically called once the class object is created
	// example : Car D7689;
	// once the compiler run this code, Car() will be immediately called.
	//same name as the class name
	~Car();
	//default destructor, it will be automatically called when the class object is gonna out the scope
	// example : int main(){  while(1){Car D7689;}   }
	// in the example, D7689 is inside the scope of the while loop, that means after ' ; 'destructor will be called
	// in other word you will D 7 689 again and again
	// if u still dont understand, google "class c++" :)
	//same name as the class name with a '~'


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
	void printvalue(int16_t value, uint16_t color);
	//print number to up-right corner in LCD

	void printvalue(int16_t x,int16_t y,int16_t w,int16_t h,int16_t value, int16_t color);
	//print number to specific location, (x,y,w,h,value-to-print) respectively

	void printvalue(std::string);
	//print string to up-right corner in LCD

	void printvalue(int16_t x,int16_t y,int16_t w,int16_t h,std::string Result);
	//print string to to specific location, (x,y,w,h,value-to-print) respectively

	void printRawCamGraph(Uint x, Uint y);

	bool updateCam();

	void clearLcd (uint16_t);
	//clear the lcd the the given color
	// its very slow

	void printline(int16_t value,uint16_t color);
	// print a horizontal line, could be for threshold

	/*--------------------------------printing above------------------------------------*/





	/*--------------------------------signal component below------------------------------------*/

	void blinkLED(int8_t id, int32_t delay_time, int32_t persist_time);
	//blink LED
	//time in ms
	//Internal delay inside

	void beepbuzzer(bool tobeep);
	//beep buzzer
	//its so fuxking annoying

	void switchLED(int8_t id);
	//use inside ticks
	/*--------------------------------signal component above------------------------------------*/





	/*--------------------------------switch below------------------------------------*/

	bool getbutton(int8_t id);
	//return true when the specific button has been pressed

	libsc::Joystick::State getjoystick();
	//return state, use with ticks

	/*--------------------------------switch above------------------------------------*/





	/*--------------------------------get data from component below------------------------------------*/

	int16_t otsu_threshold();
	void get_raw_image(void);



	/*--------------------------------get data from component above------------------------------------*/





	/*--------------------------------implement in inherited class------------------------------------*/
	//virtual simply means you can have different implementation of that function in the inherited class
	// dont understand? NVM, just skip this part


	virtual int16_t turningPID (int16_t){return 0; };
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error
	// one of the method to turn your car

	virtual int16_t motorPID (int16_t){return 0; };
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error

	virtual void motor_control(uint16_t power, bool is_clockwise_rotating){ };	//0~1000
	//apply motor PID to improve the acceleration

	virtual void servo_control(int16_t degree){ }; //-90 to 90
	//apply turning PID to improve the turning delay & to have a smooth turning

	virtual int32_t get_encoder_count(){return 0; };
	//not sure how to use encoder



private: // on9 sin set private :)
	//private means only member of it's only class can access it.
	Byte* data = new Byte[600];
	int16_t image_size;
protected: //protected seems professional
	//protected means either member of it's only class or class inherited


	libsc::Led* Led1=nullptr;
	libsc::Led* Led2=nullptr;
	libsc::Led* Led3=nullptr;
	libsc::Led* Led4=nullptr;
	libsc::AbEncoder* encoder=nullptr;
	libsc::FutabaS3010* servo=nullptr;
	libsc::DirMotor* motor=nullptr;
	libsc::Button* button1=nullptr;
	libsc::Button* button2=nullptr;
	libsc::Joystick* joystick=nullptr;
	libsc::St7735r* LCD=nullptr;
	libsc::LcdConsole* LCDconsole=nullptr;
	libsc::LcdTypewriter* LCDwriter=nullptr;
	libsc::SimpleBuzzer* buzzer=nullptr;
	libsc::k60::Ov7725* cam=nullptr;



	//the above * means its a pointer.
	// either  int* a;
	// or 	   int *a;
	// are correct, but int* a is recommended. The reason is....i forgot, just better to do this


};
