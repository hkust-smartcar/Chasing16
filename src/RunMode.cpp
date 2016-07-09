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
	// motor
	maxMotorSpeed = 600;
	minMotorSpeed = 0;
	ideal_motor_speed = 0;
	encoder_count = 0;
	m_kp =  1;
	m_ki = 0;
	m_kd = 0;

	//servo
	ideal_servo_degree = 0;
	angle_error = 0;
	angle_error_sum = 0;
	pre_angle_error = 0;
	s_kp =  1;
	s_ki = 0;
	s_kd = 0;
}


RunMode::~RunMode(){

}

int16_t RunMode::turningPID(CamHandler::Case routeCase ,int8_t routeMidP){
//	if(routeCase == CamHandler::CrossRoute){
//
//	}
	pre_angle_error = angle_error;
	angle_error_sum += angle_error;
	angle_error = 40 -routeMidP;
	ideal_servo_degree = s_kp*angle_error + s_ki*angle_error_sum +  s_kd*(pre_angle_error - angle_error);
	return ideal_servo_degree;
}

int16_t RunMode::motorPID (int16_t ideal_encoder_count){
	motorspeed_error_prev = motorspeed_error;
	motorspeed_error_sum += motorspeed_error;
	motorspeed_error = ideal_encoder_count - encoder_count;
	ideal_motor_speed = ideal_motor_speed + m_kp*motorspeed_error + m_ki*motorspeed_error_sum +  m_kd*(motorspeed_error_prev - motorspeed_error);
	//for 15 ms
	ideal_motor_speed = (ideal_motor_speed) / 15-24;
//	if(encoder_count <=200 )return -1;
	return ideal_motor_speed;
	//check return, if always -1 then stop
}

void RunMode::motor_control(uint16_t power, bool is_clockwise_rotating){
	if(power > maxMotorSpeed) power = maxMotorSpeed;
	if(power < minMotorSpeed) power = minMotorSpeed;
	motor.SetClockwise(is_clockwise_rotating);
	ideal_motor_speed = power;
	motor.SetPower(ideal_motor_speed);
}
//not a good method

void RunMode::servo_control(int16_t degree){
	if(degree > maxServoAngle ) degree = maxServoAngle;
	if(degree < minServoAngle ) degree = minServoAngle;
	ideal_servo_degree = degree*10 + 992;
	servo.SetDegree(ideal_servo_degree);
}

void RunMode::update_encoder(){
	encoder.Update();
}

int32_t RunMode::get_encoder_count(){
	encoder_count = encoder.GetCount();
	return encoder_count;
}

void RunMode::print_case(CamHandler::Case routecase){
//	enum Case{
//			StopCar = 0,StraightRoute,SlightLeft,SlightRight,InLeftCurve,InRightCurve,
//		SFront,CrossRoutetoLeft,CrossRoutetoRight,InComingLeftCurve,InComingRightCurve,
//		InRightBigCurve,InLeftBigCurve,NotInit
//	};
	switch(routecase){
	case 0:
		printvalue("StopCar");
		break;
	case 1:
		printvalue("StraightRoute");
		break;
	case 2:
		printvalue("SlightLeft");
		break;
	case 3:
		printvalue("SlightRight");
		break;
	case 4:
		printvalue("InLeftCurve");
		break;
	case 5:
		printvalue("InRightCurve");
		break;
	case 6:
		printvalue("SFront");
		break;
	case 7:
		printvalue("CrossRoute");
		break;
	case 8:
		printvalue("InComingLeftCurve");
		break;
	case 9:
		printvalue("InComingRightCurve");
		break;
	case 10:
		printvalue("InRightBigCurve");
		break;
	case 11:
		printvalue("InLeftBigCurve");
		break;
	case 12:
		printvalue("NotInit");
		break;
	case 13:
		printvalue("CantRecognize");
		break;
	default:
		printvalue("unexpected value");


	}
}
