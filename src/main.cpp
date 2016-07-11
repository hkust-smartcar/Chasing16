/*
 * main.cpp
 *
 * Author: Peter
 * Copyright (c) 2014-2015 HKUST SmartCar Team
 * Refer to LICENSE for details
 */

#include <cassert>
#include <cstring>
#include <libsc/system.h>
#include <stdint.h>
#include "car.h"
#include "RunMode.h"
#include "cameraDataHandler.h"
#include <libutil/pGrapher.h>
using namespace libsc;
using namespace libutil;
using namespace libbase::k60;


int main(void)
{
	System::Init();
	RunMode Run;
	CamHandler Image;
	pGrapher Grapher;
	Timer::TimerInt init_time = 0;
	Timer::TimerInt persist_time = 0;
	Timer::TimerInt current_time = 0;
	Timer::TimerInt past_time = 0, past_time2 = 0, past_time3 = 0, past_time4 = 0;
	bool UsensorIsReady = false;
	Run.servo_control(0);
	uint16_t distance = 0;
//	char haha[30];
//	while(1);
//code for ploting graph for a equation of y = mx +c, where y and x are encoder counting or motor PWM
//uncomment for usage

	/*//tune encoder here
	//to uncomment this code, comment all pVarManager object

	k60::JyMcuBt106::Config config;
	config.id = 0;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	k60::JyMcuBt106 fuck(config);
	char *PWM_buffer = new char[120]{0};
	float encoder_counting = 0;
	int motor_speed =0;
 while(1){
	 motor_speed += 1;
	 Run.motor_control(motor_speed,true);
	 Run.update_encoder();
	 System::DelayMs(30);
	 Run.update_encoder();

	 encoder_counting = Run.get_encoder_count();
	 int n = sprintf(PWM_buffer,"%d %d \n",(int)motor_speed,(int)encoder_counting);
	 fuck.SendBuffer((Byte*)PWM_buffer,n);
	 memset(PWM_buffer,0,n);
	 if (motor_speed > 500) {	 Run.motor_control(0,true);while(1);}
	 System::DelayMs(20);
 }


	//10-7-2016
	//y=32.664x-837.77,  x->PWM, y->encoder count *(-1)
	// thus , x = (y+837.77)/32.664
	// this is for 30ms*/



/*
	 to use pVarManager, you need to use Chrome to download the app by peter
	 link:
	 https://chrome.google.com/webstore/search/pgrapher?utm_source=chrome-ntp-icon
*/

	//-------------------------------------your code below----------------------------------------//

//	k60::JyMcuBt106::Config config;
//	config.id = 1;
//	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
//	k60::JyMcuBt106 fuck(config);
//	char* buffer = new char[100]{0};
//	int n;
//	Byte* RawData = nullptr;
	//must init for using LCD and anything that contain function inside "System"
	//use tick
	//...
	float speed = 0.0f, midPT = 0.0f;
	float angle = 0.0f;
	Grapher.addSharedVar(&Run.s_kpBCurve,"s_kpBCurve");
	Grapher.addSharedVar(&Run.s_kdBCurve,"s_kdBCurve");
	Grapher.addSharedVar(&Run.s_kpBCurveR,"s_kpBCurveR");
	Grapher.addSharedVar(&Run.s_kdBCurveR,"s_kdBCurveR");
	Grapher.addSharedVar(&speed,"speed");
//	Grapher.addSharedVar(&angle,"angle");
//	Grapher.addSharedVar(&Run.s_kpBCurve,"s_kpBCurve");
//	Grapher.addSharedVar(&Run.s_kdBCurve,"s_kpBCurve");
//	Grapher.addSharedVar(&Run.s_kpBCurveR,"s_kpBCurveR");
//	Grapher.addSharedVar(&Run.s_kdBCurveR,"s_kpBCurveR");
//	Grapher.addSharedVar(&Run.m_kp,"m_kp");
	Grapher.addWatchedVar(&angle,"angle");
	Grapher.addWatchedVar(&midPT,"midPT");
//	Grapher.addWatchedVar(&Run.encoder_count,"encoder_count");
	Grapher.addWatchedVar(&distance,"distance");


	Run.clearLcd(0);
	while(1){
		if(current_time !=System::Time()){
			current_time = System::Time();

			if((int32_t)(current_time - past_time) >= 12){
				past_time = current_time;
//				Run.printRawCamGraph(0,40);
				Run.updateCam();
				Image.updateRawData(Run.get_raw_image());
//				Run.print_case(Image.imageProcess());
				midPT = Image.getMidPT();
				angle = Run.turningPID(Image.imageProcess(), midPT);
				Run.servo_control(angle);
				}
			}

		if((int32_t)(current_time - past_time2) >= 30){
			past_time2 = current_time;
			Run.update_encoder();
			Run.get_encoder_count();
			Grapher.sendWatchData();
			Run.motor_control(Image.getPreviousCase(), speed, distance);
//			Run.motor_control(Run.motorPID(speed),1);
		}

		if(!UsensorIsReady){
			past_time3 = current_time;
			Run.update_Usensor();
			UsensorIsReady = true;
		}
//
		if((int32_t)(current_time - past_time4) >= 50 && UsensorIsReady){
			past_time4 = current_time;
			if(Run.checkUSensor()) UsensorIsReady = false;
			distance = Run.getFrontObjDistance();
		}



	}


	return 0;
}
