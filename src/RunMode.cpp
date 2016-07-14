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

	maxMotorSpeed = 850;
	minMotorSpeed = 0;
	ideal_motor_speed = 0;
	PID_Output = 0;
	encoder_count = 0;
	m_kp = 0.10100000351667404;
	m_ki = 0.01;
	m_kd = 0.038;

	//servo
	ideal_servo_degree = 0;
	angle_error = 0;
	angle_error_sum = 0;
	pre_angle_error = 0;
	s_kpBCurve = 18.5;
	s_kdBCurve = 0;
	s_kpBCurveR = 10.6;
	s_kdBCurveR = 0;
	s_kpSCurve = 15;
	s_kdSCurve = 1.5;
	s_kpSCurveR = 18;
	s_kdSCurveR= 1.5;
	s_kpSRoute = 18;
	s_kdSRoute = 2;
	s_kpStraight = 8;
	s_kdStraight = 1;
	s_kpCross = 3.6;
	s_kdCross = 1.3;

}

RunMode::~RunMode(){

}

int16_t RunMode::turningPID(CamHandler::Case routeCase ,int8_t routeMidP){
	previousCase = currentCase;
	currentCase = routeCase;
	pre_angle_error = angle_error;
	angle_error_sum += angle_error;
	angle_error = 40 -routeMidP;
	if(routeCase == CamHandler::Case::CrossRoute){
		ideal_servo_degree = s_kpSRoute*angle_error + s_kdSRoute*(pre_angle_error - angle_error);
	}
	else if(routeCase == CamHandler::Case::InLeftBigCurve ){
		ideal_servo_degree = s_kpBCurve*angle_error + s_kdBCurve*(pre_angle_error - angle_error);
	}
	else if(routeCase == CamHandler::Case::InRightBigCurve){
			ideal_servo_degree = s_kpBCurveR*angle_error + s_kdBCurveR*(pre_angle_error - angle_error);
	}
	else if( routeCase == CamHandler::Case::InLeftCurve ){
		ideal_servo_degree = s_kpSCurve*angle_error + s_kdSCurve*(pre_angle_error - angle_error);
	}
	else if(routeCase == CamHandler::Case::InRightCurve){
			ideal_servo_degree = s_kpSCurveR*angle_error + s_kdSCurveR*(pre_angle_error - angle_error);
	}
	else if(routeCase == CamHandler::Case::StraightRoute){
		ideal_servo_degree = s_kpStraight*angle_error + s_kdStraight*(pre_angle_error - angle_error);
	}
	else {
		ideal_servo_degree = s_kpStraight*angle_error + s_kdStraight*(pre_angle_error - angle_error);
	}

	return ideal_servo_degree;
}

int16_t RunMode::motorPID (int16_t ideal_encoder_count){
	motorspeed_error_prev = motorspeed_error;
	motorspeed_error = ideal_encoder_count - encoder_count;
	motorspeed_error_sum += motorspeed_error;
//	PID_Output = ideal_encoder_count +m_kp*motorspeed_error + m_ki*motorspeed_error_sum +  m_kd*(motorspeed_error_prev - motorspeed_error);
	ideal_motor_speed = m_kp*motorspeed_error + m_ki*motorspeed_error_sum +  m_kd*(motorspeed_error_prev - motorspeed_error);
//	ideal_motor_speed = (PID_Output+837.77)/32.664;
	if(ideal_encoder_count == 0 || ideal_motor_speed <0)ideal_motor_speed = 0;
	return ideal_motor_speed;
}

void RunMode::motor_control(CamHandler::Case caseForward, uint16_t ideal_encoder_count, uint16_t frontObjDist){
	buff_speed = ideal_encoder_count;
	if(buff_speed != 0){
		if(caseForward == CamHandler::Case::StraightRoute){
			buff_speed = ideal_encoder_count* 1.4;
		}
		else if (caseForward == CamHandler::Case::InLeftBigCurve  || caseForward == CamHandler::Case::InRightBigCurve ){
			buff_speed = ideal_encoder_count*0.9;
		}

		if(frontObjDist != 0 && frontObjDist <800){
			if(frontObjDist < 200) buff_speed = 0;
			else if(frontObjDist < 400) buff_speed *= (1 - (frontObjDist-200) /200) ;
			else if(frontObjDist < 700) buff_speed *=   ( 1 - (frontObjDist-400) /400) ;
		}
	}
	motor_control(motorPID(buff_speed),true);
}

void RunMode::motor_control(uint16_t power, bool is_clockwise_rotating){
	if(power > maxMotorSpeed) power = maxMotorSpeed;
	if(power < minMotorSpeed) power = minMotorSpeed;
	motor.SetClockwise(!is_clockwise_rotating);
	ideal_motor_speed = power;
	motor.SetPower(ideal_motor_speed);
}
//not a good method

void RunMode::servo_control(int16_t degree){
	if(degree > maxServoAngle ) degree = maxServoAngle;
	if(degree < minServoAngle ) degree = minServoAngle;
	ideal_servo_degree = degree + 970;
	servo.SetDegree(ideal_servo_degree);
}

void RunMode::update_encoder(){
	encoder.Update();
}

int32_t RunMode::get_encoder_count(){
	encoder_count = -1 *encoder.GetCount();
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


void RunMode::print_commend(ChaseMethod::Command command){
	switch(command){
	case 0:
		printvalue("noAction");
		break;
	case 1:
		printvalue("shiftRole");
		break;
	case 2:
		printvalue("traingleForward");
		break;
	case 3:
		printvalue("straightPass");
		break;
	case 4:
		printvalue("stopCar");
		break;
	default:
		printvalue("error");
		break;
	}
}


