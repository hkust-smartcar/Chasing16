/*
 * RunMode.h
 *
 *  Created on: 2016¦~2¤ë20¤é
 *      Author: yungc
 */

#pragma once
#include "car.h"
#include "cameraDataHandler.h"
#include <array>
#define WHITE 1
#define BLACK 0
class RunMode: public Car
{
	// tips for choosing datatype, dont use too large( it will slow down your program)
	//and dont use too small (otherwise underflow occur)
	// you may check 8bit,16bit,and 32bit int & uint range fromthe link:
	//			http://www.societyofrobots.com/member_tutorials/book/export/html/341
	//or just google that if u are not sure
public:
	RunMode();
	~RunMode();

	int16_t turningPID (CamHandler::Case ,int8_t routeMidP);
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error, in smartcar, ki can be neglected as its too small


	int16_t motorPID (int16_t ideal_encoder_count);
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error, in smartcar, ki can be neglected as its too small

	void motor_control(uint16_t power, bool is_clockwise_rotating);	//0~1000
	//apply motor PID to improve the acceleration

	void motor_control(CamHandler::Case ,uint16_t power);
	// speed boost and reduce when straight route or big curve route

	void servo_control(int16_t degree); //-90 to 90

	void adjust_image();	//correct the image

	void print_case(CamHandler::Case routecase);

	std::array<std::array<bool,80>,60> getRawData(Byte cam_data);

	void extract_line();	//

	void identify_road();	//

	void update_encoder();

	int32_t get_encoder_count();

	void update_Usensor();
	bool checkUSensor();
	uint16_t getFrontObjDistance(){return objDistance;}
	//for "GetCount()" of encoder, it return the difference of value between two update
	//i.e. update ->process-with-2s->update->GetCount, the result is the encoder count within that process with 2s


	//--------------------------variable below---------------------------//
	//to access the public variable, you can use (obj_name).(var_name) to access

	/* motor PID */
	int32_t motor_speed_buff = 0, ideal_motor_speed = 0;
	int32_t motorspeed_error_prev = 0,motorspeed_error = 0,
			 PID_Output = 0;

	float m_kp, m_ki, m_kd;
	int32_t encoder_count = 0 ,motorspeed_error_sum = 0;

	int16_t motor_boost = 0, motor_reduce = 0;

	/* turning PID */
	int16_t ideal_servo_degree;
	int16_t angle_error , angle_error_sum ;
	int16_t pre_angle_error;
	float s_kpBCurve, s_kdBCurve;
	float s_kpBCurveR, s_kdBCurveR;
	float s_kpSCurve, s_kdSCurve;
	float s_kpSCurveR, s_kdSCurveR;
	float s_kpSRoute, s_kdSRoute;
	float s_kpStraight, s_kdStraight;
	float s_kpCross, s_kdCross;
	bool image[4800]={false};
	CamHandler::Case currentCase = CamHandler::Case::NotInit;
	CamHandler::Case previousCase = CamHandler::Case::NotInit;

	/* motor control */
	int16_t straight_counter =0;

	/* Usensor */
	uint16_t objDistance = 0;
	bool sensorWorked = false;

private: //yes, I add these variable as private, because they are not important
	// Moreover, variable can be declare in header(.h), and define in either header(.h) or source(.cpp)
	int16_t maxServoAngle = 420;
	int16_t minServoAngle = -420;		// give a maximun& minimun angle for servo to turn
	int32_t maxMotorSpeed, minMotorSpeed; // give a maximun& minimun PWM for motor to run

};
