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
#include "pVarManager.h"
#include "car.h"
#include "RunMode.h"


using namespace libsc;

using namespace libbase::k60;


int main(void)
{
	System::Init();
	RunMode Run;
	Timer::TimerInt init_time;


/*

//code for ploting graph for a equation of y = mx +c, where y and x are encoder counting or motor PWM
//uncomment for usage

	//tune encoder here
	//to uncomment this code, comment all pVarManager object
	k60::JyMcuBt106::Config config;
	config.id = 0;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	config.tx_dma_channel = -1;
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

*/


/*
	 to use pVarManager, you need to use Chrome to download the app by peter
	 link:
	 https://chrome.google.com/webstore/search/pgrapher?utm_source=chrome-ntp-icon
*/

	//-------------------------------------your code below----------------------------------------//


	//must init for using LCD and anything that contain function inside "System"
	//use tick
	//...
	Run.servo_control(0);
	Timer::TimerInt current_time = 0;
	Timer::TimerInt past_time = 0, past_time2 = 0;
	Run.clearLcd(0);
	
	while(1){
		if(current_time !=System::Time()){
			current_time = System::Time();
			if((int32_t)(current_time - past_time) >= 20){
				past_time = current_time;
				Run.printRawCamGraph(0,0);
				Run.updateCam();
			}

			if((int32_t)(current_time - past_time2) >= 20){
				past_time2 = current_time;
			}
		}
	}


	return 0;
}
