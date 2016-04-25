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
	RunMode Run;

//code for ploting graph for a equation of y = mx +c, where y and x are encoder counting or motor PWM
//uncomment for usage

	//tune encoder here
	//to uncomment this code, comment all pVarManager object
	JyMcuBt106::Config config;
	config.id = 0;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	config.rx_irq_threshold = 2;
	JyMcuBt106 fuck(config);
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



/*
	 to use pVarManager, you need to use Chrome to download the app by peter
	 link:
	 https://chrome.google.com/webstore/search/pgrapher?utm_source=chrome-ntp-icon
*/

	//-------------------------------------your code below----------------------------------------//

	System::Init();
	//must init for using LCD and anything that contain function inside "System"
	//use tick
	//...

	while(1){
	Run.servo_control(0);
	System::DelayMs(500);
	Run.servo_control(30);
	System::DelayMs(500);
	Run.servo_control(0);
	System::DelayMs(500);
	Run.servo_control(-30);
	System::DelayMs(500);
	Run.servo_control(-30);
	}






	return 0;
}
