/*
 * RunMode.cpp
 *
 *  Created on: 2016¦~2¤ë20¤é
 *      Author: yungc
 */
#include "car.h"
#include "RunMode.h"

RunMode::RunMode(){
	//can initialize the variable here,
	maxMotorSpeed = 600;
	minMotorSpeed = 0;
	ideal_servo_degree = 0;
	ideal_motor_speed = 0;
	encoder_count = 0;


}

RunMode::~RunMode(){

}


int16_t RunMode::motorPID (int16_t ideal_encoder_count){

	return 0;//your implementation
	// tips, remember to add something to protect your motor, for example:
	// e.g. 1) add a simple if-statement, if encoder count is near zero for 1~2s, stop the motor
	// e.g. 2) if the car are sure its crazy, stop motor
}

void RunMode::motor_control(uint16_t power, bool is_clockwise_rotating){
	if(power > maxMotorSpeed) power = maxMotorSpeed;
	if(power < minMotorSpeed) power = minMotorSpeed;
	motor->SetClockwise(is_clockwise_rotating);
	ideal_motor_speed = power;
	motor->SetPower(ideal_motor_speed);
}
//not a good method

void RunMode::servo_control(int16_t degree){
	if(degree > maxServoAngle ) degree = maxServoAngle;
	if(degree < minServoAngle ) degree = minServoAngle;
	ideal_servo_degree = degree*10 + 900;
	servo->SetDegree(ideal_servo_degree);
}

void RunMode::update_encoder(){
	encoder->Update();
}

int32_t RunMode::get_encoder_count(){
	encoder_count = -1 * encoder->GetCount();
	return encoder_count;
}
