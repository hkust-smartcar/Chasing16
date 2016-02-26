/*
 * RunMode.h
 *
 *  Created on: 2016¦~2¤ë20¤é
 *      Author: yungc
 */

#pragma once
#include "car.h"

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

	int16_t turningPID (int16_t mid_line);
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error, in smartcar, ki can be neglected as its too small


	int16_t motorPID (int16_t ideal_encoder_count);
	//PID = kp *error +kd *(error_prev - error) + ki * sum of error, in smartcar, ki can be neglected as its too small

	void motor_control(uint16_t power, bool is_clockwise_rotating);	//0~1000
	//apply motor PID to improve the acceleration

	void servo_control(int16_t degree); //-90 to 90

	void adjust_image();	//correct the image

	void extract_line();	//

	void identify_road();	//

	void update_encoder();
	int32_t get_encoder_count();
	//for "GetCount()" of encoder, it return the difference of value between two update
	//i.e. update ->process-with-2s->update->GetCount, the result is the encoder count within that process with 2s


	//--------------------------variable below---------------------------//
	//to access the public variable, you can use (obj_name).(var_name) to access

	int16_t ideal_servo_degree, ideal_motor_speed;
	int32_t encoder_count;


private: //yes, I add these variable as private, because they are not important
	// Moreover, variable can be declare in header(.h), and define in either header(.h) or source(.cpp)
	int16_t maxServoAngle = 60;
	int16_t minServoAngle = -60;		// give a maximun& minimun angle for servo to turn
	int16_t maxMotorSpeed, minMotorSpeed; // give a maximun& minimun PWM for motor to run

};
