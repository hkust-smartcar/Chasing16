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
	m_kp =  0;
	m_ki = 0;
	m_kd = 0;
}

RunMode::RunMode(uint8_t bluetoothMode){

	maxMotorSpeed = 600;
	minMotorSpeed = 0;
	ideal_servo_degree = 0;
	ideal_motor_speed = 0;
	encoder_count = 0;

	switch (bluetoothMode)
	{
	case 0:
		break;
	case 1:
		break;
	default:
		break;
	}

}

RunMode::~RunMode(){
}


int16_t RunMode::motorPID (int16_t ideal_encoder_count){
	motorspeed_error_prev = motorspeed_error;
	motorspeed_error_sum += motorspeed_error;
	motorspeed_error = ideal_encoder_count - encoder_count;
	ideal_motor_speed = ideal_motor_speed + m_kp*motorspeed_error + m_ki*motorspeed_error_sum +  m_kd*(motorspeed_error_prev - motorspeed_error);
	//for 15 ms
	ideal_motor_speed = (ideal_motor_speed) / 15-24;
	if(encoder_count <=200 )return -1;
	return ideal_motor_speed;
	//check return, if always -1 then stop
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
	ideal_servo_degree = degree*10 + 992;
	servo->SetDegree(ideal_servo_degree);
}

void RunMode::update_encoder(){
	encoder->Update();
}

int32_t RunMode::get_encoder_count(){
	encoder_count = encoder->GetCount();
	return encoder_count;
}
