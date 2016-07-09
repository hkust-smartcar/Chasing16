/*
 * chase_method.cpp
 *
 *  Created on: 2016¦~6¤ë3¤é
 *      Author: yungc
 */
#include "chase_method.h"


ChaseMethod::ChaseMethod(){
	role = follower;
}

ChaseMethod::ChaseMethod(Role carRole,uint8_t id){
	role = carRole;

	libsc::k60::JyMcuBt106::Config config;
	config.id = id;
	config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	HC12 = new libsc::k60::JyMcuBt106 (config);
}

ChaseMethod::~ChaseMethod(){

}

bool ChaseMethod::init(libsc::Timer::TimerInt period){
	if(role == leader){

	}
	else if (role == follower){

	}else{
		while(1);//loop right here if it can't recognize its role
	}

	return false;
}
