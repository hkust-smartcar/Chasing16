#include <iostream>
#include <libutil/looper.h>
#include <libsc/system.h>
#include <functional>
#include <libsc/led.h>
#include <libbase/k60/mcg.h>
#include <libsc/dir_encoder.h>
#include <libsc/ab_encoder.h>
#include <libsc/futaba_s3010.h>
#include <libsc/k60/ov7725.h>
#include <libsc/button.h>
#include <libsc/joystick.h>
#include <libsc/st7735r.h>
#include <libsc/simple_buzzer.h>
#include <libsc/battery_meter.h>
#include <libsc/lcd_console.h>
#include <libsc/lcd_typewriter.h>
#include <libsc/dir_motor.h>
#include <libsc/k60/jy_mcu_bt_106.h>
#include <array>
#include <cstring>
#include <sstream>
#include <string>
#include "libsc/lcd.h"
//#include "img_regression.h"
#include "img_process_compilation.h"
#include <cmath>
#include <math.h>
#include <libutil/pGrapher.h>
#include "chase_method.h"

#define data_size (WIDTH * HEIGHT / 8)

using namespace libsc;

using namespace libbase::k60;

using namespace libutil;

namespace libbase
{
namespace k60
{

Mcg::Config Mcg::GetMcgConfig()
{
	Mcg::Config config;
	config.external_oscillator_khz = 50000;
	config.core_clock_khz = 200000;
	return config;
}

}
}

//libsc::Led Led1();
libsc::Led* Led1 = nullptr;
libsc::Led* Led2 = nullptr;
libsc::Led* Led3 = nullptr;
libsc::Led* Led4 = nullptr;
libsc::DirEncoder* encoder = nullptr;
libsc::FutabaS3010* servo = nullptr;
libsc::DirMotor* motor = nullptr;
libsc::St7735r* LCD = nullptr;
libsc::LcdConsole* LCDconsole = nullptr;
libsc::LcdTypewriter* LCDwriter = nullptr;
libsc::SimpleBuzzer* buzzer = nullptr;
libsc::k60::Ov7725* cam = nullptr;
libsc::Joystick* joystick = nullptr;
//libsc::k60::UartDevice* Bluetooth = nullptr;
//libsc::Us100* USensor = nullptr;
Gpi joy_down, joy_left, joy_right, joy_center, joy_up;

Byte data[WIDTH * HEIGHT / 8];
bool image[HEIGHT][WIDTH];
int8_t midpoints[HEIGHT];
int8_t midpoint_ending_index = HEIGHT;
int8_t absolute_midpoint_ending_index = HEIGHT;

void Config_all();
//void printvalue(int x, int y, int w, int h, int16_t value, int16_t color);
void printvalue(std::string Result);
//void printvalue(int x, int y, int w, int h, std::string Result);
void printRawCamGraph(const int8_t x, const int8_t y, Byte* data);
void print2DCam(const uint xpos, const uint ypos,const bool (array)[HEIGHT][WIDTH]) ;
//void printline(int16_t value, uint16_t color);
void clearLcd(uint16_t color);
//void blinkLED(int8_t id, int delay_time, int persist_time);
void switchLED(int8_t id);
bool GetPixel(const Byte* src, const int8_t x, const int8_t y) ;
void capture_image(void);
//void print_Count(int encoder_count);
//void speed_and_angle_control(RunMode& car, int current_position, double gradient);
void print_Count(int encoder_count);
//void current_position_PD(float& P, float& D, int k);
void print_midpoints_and_edges(int8_t midpoints[HEIGHT],int8_t ending_index);
void print_State(STATE state);
//int check_triangle(bool image[HEIGHT][WIDTH], int midpoint_ending_index, int column);


struct RunMode{

	int16_t ideal_motor_speed = 0;
	int16_t ServoErr = 0;
	int16_t ServoPrevErr = 0;
	int16_t ServoPrev1Err = 0;

	int16_t MotorErr = 0;
	int16_t MotorPrev1Err = 0;
	int16_t MotorPrev2Err = 0;

	STATE current_state = GoStraight;
	int8_t last_midpoint = WIDTH/2;

	int8_t max_consecutive_midpoint_diff = 0;
	int16_t TrackArea = 0;
	int8_t separating_index = 0;
	//	STATE previous_state = GoStraight;
};

// Create ChaseMethod object



int8_t x = 1;
int16_t COUNT = 0;
int main(){

	System::Init();
	Timer::TimerInt t = 0;
	Timer::TimerInt tick = System::Time();
	Timer::TimerInt triangle_time = 0;

	bool triangle_detector_1 = false, triangle_detector_2 = false, triangle_detector_3 = false;
	bool triangle = false, reached_triangle = false;
	int triangle_count_1 = 0, triangle_count_2 = 0, triangle_count_3 = 0;


	// configuration
	Config_all();

	// create RunMode object
	RunMode Runner;



	//CoolTerm: Return encoder count
	/*
		k60::JyMcuBt106::Config config;
		config.id = 1;
		config.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
		k60::JyMcuBt106 fuck(config);
		char *PWM_buffer = new char[120]{0};
		float encoder_counting = 0;
		int motor_speed =0;
		while(1){
			motor_speed += 1;
			motor->SetPower(motor_speed);
			encoder->Update();
			System::DelayMs(30);
			encoder->Update();
			encoder_counting = encoder->GetCount();
			int n = sprintf(PWM_buffer,"%d %d \n",(int)motor_speed,(int)encoder_counting);
			fuck.SendBuffer((Byte*)PWM_buffer,n);
			memset(PWM_buffer,0,n);
			if (motor_speed > 500) {motor->SetPower(0);while(1);}
			System::DelayMs(20);
		}
	 */
	//	float state = 0;

	//	float test_coef_1 = 0.0f; float test_coef_2 = 0.0f; double test_grad = 1; double test_grad_1 = 1;
	bool ispressed = false;
	bool UPDATE_SERVO = 1;
	Runner.ideal_motor_speed = 0;
	int16_t& count =COUNT;
	int16_t Usensor_counter = 0;
	bool run = false;
	int area = 0;
	float encoder_count = 0.0f;
	ChaseMethod Chase(ChaseMethod::Role::leader,0,1,&count);
	Chase.update_Usensor();
	//	float area_f = 0.0f;
	//	int nearest = 0, furthest = 0, left = 0, right = 0;
	//	float run_f = 0.0f, area_f = 0.0f, nearest_f = 0.0f, furthest_f = 0.0f, left_f = 0.0f, right_f = 0.0f;
	//	float max_diff = 0.0f, absolute_midpoint_index_f = 0.0f; float AREA = 0.0f; float separating_index_f = 0.0f;

	//PGrapher: Share and Watch Variables
	//	pGrapher Grapher;

	//	Grapher.addSharedVar(&Kp,"Motor PID Kp");
	//	Grapher.addSharedVar(&Kp,"Motor PID Kp");
	//	Grapher.addSharedVar(&Kp,"Motor PID Kp");
	/*
	float angle_1 = 0; float angle_2 = 0; float angle_3 = 0; float angle_4 = 0;float separating_index_f = 0.0f;
	float mid_point_index_f = 0;
	 */
	float is_triangle = 0.0f;
	int16_t obj_distance = 0;
	//	Grapher.addWatchedVar(&is_triangle, "Check Triangle");
	//	Grapher.addWatchedVar(&encoder_count,"Encoder Count / 5");
	//	Grapher.addWatchedVar(&separating_index_f,"Separating Index");
	//	Grapher.addWatchedVar(&mid_point_index_f,"Mid Point Ending Index");
	//	Grapher.addWatchedVar(&angle_4,"angle_4");
	//	Grapher.addWatchedVar(&angle_3,"angle_3");
	//	Grapher.addWatchedVar(&angle_2,"angle_2");
	//	Grapher.addWatchedVar(&furthest,"furthest triangle");
	//	Grapher.addWatchedVar(&left,"Most left");
	//	Grapher.addWatchedVar(&right,"Most right");


	while(1){

		// Print system time
		//		print_Count(System::Time()-tick);
		if(tick!= System::Time()){
			tick = System::Time();
		}
		if(triangle_time != System::Time()){
			triangle_time = System::Time();
		}

		// Update PGrapher
		//		Grapher.sendWatchData();

		Led1->SetEnable(x%4 == 0);
		Led2->SetEnable(x%4 == 1);
		Led3->SetEnable(x%4 == 2);
		Led4->SetEnable(x%4 == 3);

		//-------------------- IMAGE PROCESSING ----------------------//
		int16_t TCount = 0;
		int16_t WCount = 0;

		capture_image();

		//		print2DCam(10,10,image);	//Print camera image

		//		print_State(Runner.current_state);

		TCount = update_midpoints_and_edges(image,midpoints, midpoint_ending_index,absolute_midpoint_ending_index, Runner.last_midpoint,Runner.max_consecutive_midpoint_diff,Runner.separating_index, Runner.TrackArea, WCount);
		//		separating_index_f = (float) Runner.separating_index;

		//		absolute_midpoint_index_f = absolute_midpoint_ending_index;
		//		max_diff = Runner.max_consecutive_midpoint_diff;
		//		AREA = Runner.TrackArea;

		//		regression_line(midpoints,HEIGHT-1,Runner.separating_index,test_grad,test_coef_1);
		//		regression_line(midpoints,Runner.separating_index-1,midpoint_ending_index,test_grad_1,test_coef_2);
		//		test_grad = ((int(180*atan(test_grad)/PI)+180)%180);
		//		test_grad_1 = ((int(180*atan(test_grad_1)/PI)+180)%180);

		//		if(check_triangle(image,absolute_midpoint_ending_index,WIDTH/2) > 100){ // need to tune
		//			Runner.current_state = Triangle;
		//		}
		//		else{
		Runner.current_state = determine_state(midpoints,midpoint_ending_index, absolute_midpoint_ending_index, TCount, WCount, Runner.separating_index, 0, triangle, reached_triangle);
		//		}
		if(Runner.current_state == Triangle){
			motor->SetPower(0);
			break;
		}

		//		print_State(Runner.current_state);
		//-------------------- SERVO PID ----------------------//
		if(UPDATE_SERVO){

			float P = 0; float D = 0;

			if(Runner.current_state == GoStraight){
				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(9/10)*(-midpoint_ending_index + HEIGHT-1));
				//				P = 15;
				P = 12;
				D = 0;
				count = 190;
				//			count = 300;
			}
			else if(Runner.current_state == SPath){
				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(11/12)*(-midpoint_ending_index + HEIGHT-1));
				//				P = 13 - ((encoder->GetCount() + 90)/20)*2;
				//				P = 13.5;
				P = 19.5;
				D = 0;
				count = 180;
				//			count = 210;
			}
			else if(Runner.current_state == TurnLeft){
				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
				//				P = 16.5 - ((encoder->GetCount() + 90)/20)*2;
				//				P = 20;
				P = 26.5 - 2;
				D = 0;
				count = 170;
			}
			else if(Runner.current_state == TurnRight){
				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
				//				P = 16.5 - ((encoder->GetCount() + 90)/20)*2;
				//				P = 20;
				P = 28.5;
				D = 0;
				count = 170;
			}
			//			else if(Runner.current_state == Triangle){
			//				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
			//				P = 15;
			////				P = 15;
			//				D = 1;
			//				count = 190;
			//			}
			//			else if(Runner.current_state == OvertakeZone){
			//				Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
			//				P = 15;
			////				P = 15;
			//				D = 1;
			//				count = 190;
			//			}

			count = 300;

			//			int32_t ideal_servo_degree = uint16_t(900 + P*Runner.ServoErr + D*(Runner.ServoErr - Runner.ServoPrevErr));
			int32_t ideal_servo_degree = uint16_t(1000 + P*Runner.ServoErr + D*(Runner.ServoErr - Runner.ServoPrevErr));
			Runner.ServoPrevErr = Runner.ServoErr;
			Runner.ServoPrev1Err = Runner.ServoPrevErr;
			servo->SetDegree(servo_cap_ome(ideal_servo_degree));
			//			servo->SetDegree(600);

		}

		// Delay time -> motor PID
		int8_t time = 5 - System::Time() + tick;
		if(time < 0){time = 0;}
		System::DelayMs(time);


		//-------------------- MOTOR PID ----------------------//
		//		float Kd = 0.005f;
		//		float Ki = 0.010f;
		//		float Kp = 0.005f;

		float Kd = 0.005f;
		float Ki = 0.010f;
		float Kp = 0.005f;

		encoder->Update();
		Runner.MotorErr = count - encoder->GetCount();
		Runner.ideal_motor_speed
		+= Kp*(Runner.MotorErr - Runner.MotorPrev1Err) + Ki*Runner.MotorErr + Kd * (Runner.MotorErr - 2*Runner.MotorPrev1Err + Runner.MotorPrev2Err);

		Runner.MotorPrev2Err = Runner.MotorPrev1Err;
		Runner.MotorPrev1Err = Runner.MotorErr;
		motor->SetClockwise(0);
		//		motor->SetPower((Runner.ideal_motor_speed - 434.43)/(2.7824) + 196);	//Transformation from count to motor speed
		motor->SetPower(Runner.ideal_motor_speed);


		//-------------------- BRAKING SYSTEM ----------------------//
		//		if(System::Time() - t < 1000 && abs(encoder->GetCount()) > 3){
		//			run = true;
		//		}
		//
		//		if(System::Time() - t > 5000 && abs(encoder->GetCount()) < 25 && run){
		//			motor->SetPower(0);
		//			break;
		//		}

		absolute_midpoint_ending_index = 0;

		//-------------------- TUNE TRIANGLE ----------------------//
		//		if(check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 - 1))
		//			print_Count(check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 - 1));
		//		else if(check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 + 1))
		//			print_Count(check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 + 1));
		//		else print_Count(0);

		//		encoder->Update()
		encoder_count = -1 * (float) encoder->GetCount() / 50;
		//		check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 - 1, left, right, area, nearest, furthest);
		//		if(!reached_triangle){


		area = check_triangle2(image, absolute_midpoint_ending_index, WIDTH/2);
		if (area == 0){
			area = check_triangle2(image, absolute_midpoint_ending_index, WIDTH/2 - 3);
		}
		if (area == 0){
			area = check_triangle2(image, absolute_midpoint_ending_index, WIDTH/2 + 3);
		}
		if(area > 100 && area < 175 && !triangle_detector_2 && !triangle_detector_3){
			triangle_detector_1 = true;
			triangle_count_1++;
			if(System::Time() - triangle_time < 200){
				if(triangle_count_1 > 5){
					triangle_detector_2 = true;
					//					triangle_time = System::Time();
				}
			}
			else if(triangle_count_1 < 5){
				triangle_detector_1 = false;
				triangle_count_1 = 0;
			}
		}
		if(triangle_detector_2 && !triangle_detector_3){
			if(area > 175 && area < 225){
				triangle_count_2++;
			}
			if(System::Time() - triangle_time < 400){
				if(triangle_count_2 > 5){
					triangle_detector_3 = true;
					//					triangle_time = System::Time();
				}
			}
			else if(triangle_count_2 < 5){
				triangle_count_1 = 0;
				triangle_count_2 = 0;
				triangle_detector_1 = false;
				triangle_detector_2 = false;
			}
		}
		if(triangle_detector_3){
			if(area > 225){
				triangle_count_3++;
			}
			if(System::Time() - triangle_time < 600){
				if(triangle_count_3 > 4){
					triangle = true;
				}
			}
			else if(triangle_count_3 < 4){
				triangle_count_1 = 0;
				triangle_count_2 = 0;
				triangle_count_3 = 0;
				triangle_detector_1 = false;
				triangle_detector_2 = false;
				triangle_detector_3 = false;
			}
		}

		if(!triangle_detector_1 && !triangle_detector_2 && !triangle_detector_3){
			triangle_time = System::Time();
		}
		//		}
		//		area_f = (float) area;



		//		left_f = (float) left;
		//		right_f = (float) right;
		//		area_f = (float) area;
		//		nearest_f = (float) nearest;
		//		furthest_f = (float) furthest;
		//		print_midpoints_and_edges(midpoints,midpoint_ending_index);


		//		Chase.update_Usensor();

//		if (!ispressed) {
//			if (!joy_down.Get()) {
//				count = count + 10;
//				ispressed = true;
//			}
//			else if (!joy_left.Get()) {
//				count = count - 10;
//				ispressed = true;
//			}
//			else if (!joy_right.Get()) {
//				ispressed = true;
//			}
//			else if (!joy_center.Get()) {
//				ispressed = true;
//			}
//			else if (!joy_up.Get()) {
//				ispressed = true;
//			}
//		}
//		else {
//			if (joy_down.Get() && joy_left.Get() && joy_right.Get()
//					&& joy_center.Get() && joy_up.Get())
//				ispressed = false;
//		}

//		if(Chase.checkUSensor()){
//			obj_distance = Chase.getFrontObjDistance();
//			Chase.update_Usensor();
//		}
//		print_Count(obj_distance);

 //		x = int(++x)%4;
//		Usensor_counter = int16_t(++Usensor_counter)%8;
	}
	return 0;
}

void print_Count(int encoder_count){
	char a[12] = "";

	sprintf(a,"%d",encoder_count);
	LCDwriter -> WriteString(a);
	LCDconsole-> Clear(0);
}

void print_State(STATE state){
	if(state == TJunction){
		printvalue("TJunction");
	}
	else if(state == SPath){
		printvalue("SPath");
	}
	else if(state == TurnLeft){
		printvalue("TurnLeft");
	}
	else if(state == TurnRight){
		printvalue("TurnRight");
	}
	else{
		printvalue("GoStraight");
	}
}


void Config_all(){

	Led::Config Led1Config;
	Led1Config.id = 0;
	Led1Config.is_active_low = true;
	Led1 = new Led(Led1Config);

	Led::Config Led2Config;
	Led2Config.id = 1;
	Led2Config.is_active_low = true;
	Led2 = new Led(Led2Config);

	Led::Config Led3Config;
	Led3Config.id = 2;
	Led3Config.is_active_low = true;
	Led3 = new Led(Led3Config);

	Led::Config Led4Config;
	Led4Config.id = 3;
	Led4Config.is_active_low = true;
	Led4 = new Led(Led4Config);

	DirEncoder::Config EncoderConfig;
	EncoderConfig.id = 0;
	encoder = new DirEncoder(EncoderConfig);
	//
	FutabaS3010::Config ServoConfig;
	ServoConfig.id = 0;
	servo = new FutabaS3010(ServoConfig);
	servo->SetDegree(900);

	DirMotor::Config AltmotorConfig;
	AltmotorConfig.id = 0;
	motor = new DirMotor(AltmotorConfig);

	St7735r::Config LcdConfig;
	LcdConfig.is_revert = false;
	LCD = new St7735r(LcdConfig);
	LCD->Clear(0);

	SimpleBuzzer::Config BuzzerConfig;
	BuzzerConfig.id = 0;
	BuzzerConfig.is_active_low = false;
	buzzer = new SimpleBuzzer(BuzzerConfig);

	k60::Ov7725::Config camcfg;
	camcfg.id = 0;
	camcfg.w = WIDTH;
	camcfg.h = HEIGHT;
	camcfg.fps=k60::Ov7725::Config::Fps::kHigh;
	//TODO adjust appropriate FPS
	//	camcfg.contrast = 0x30;
	cam = new k60::Ov7725(camcfg);
	cam->Start();

	while (!cam->IsAvailable()) {
	};

	LcdTypewriter::Config LcdWconfig;
	LcdWconfig.lcd = LCD;
	LCDwriter = new LcdTypewriter(LcdWconfig);
	LCD->SetRegion(Lcd::Rect(0,0,128,160));
	//
	LcdConsole::Config LCDCConfig;
	LCDCConfig.lcd = LCD;
	LCDCConfig.region = Lcd::Rect(0,0,128,160);
	LCDconsole = new LcdConsole(LCDCConfig);

	Joystick::Config config;

	config.id = 0;
	config.is_active_low = true;
	config.listener_triggers[0] = Joystick::Config::Trigger::kBoth;
	config.listener_triggers[1] = Joystick::Config::Trigger::kBoth;
	config.listener_triggers[2] = Joystick::Config::Trigger::kBoth;
	config.listener_triggers[3] = Joystick::Config::Trigger::kBoth;
	config.listener_triggers[4] = Joystick::Config::Trigger::kBoth;
	config.handlers[static_cast<int>(Joystick::State::kUp)] = [&](const uint8_t, const Joystick::State)
	{
		COUNT++;
	};
	config.handlers[static_cast<int>(Joystick::State::kDown)] = [&](const uint8_t, const Joystick::State)
	{

	};
	config.handlers[static_cast<int>(Joystick::State::kLeft)] = [&](const uint8_t, const Joystick::State)
	{

	};
	config.handlers[static_cast<int>(Joystick::State::kRight)] = [&](const uint8_t, const Joystick::State)
	{

	};
	config.handlers[static_cast<int>(Joystick::State::kSelect)] = [&](const uint8_t, const Joystick::State)
	{

	};
	joystick = new Joystick(config);

}

void printvalue(std::string Result) {
	LCD->SetRegion(libsc::Lcd::Rect(0, 0, 128, 40));
	const char *s = Result.c_str();
	LCDwriter->WriteString(s);
}
/*
	void printvalue(int x, int y, int w, int h, std::string Result) {
		LCD->SetRegion(libsc::Lcd::Rect(x, y, w, h));
		const char *s = Result.c_str();
		LCDwriter->WriteString(s);
	}
 */
void printRawCamGraph(const int8_t x, const int8_t y, Byte* data) {
	LCD->SetRegion(Lcd::Rect(x, y, 80, 60));
	LCD->FillBits(0, 0xFFFF, data, 80 * 60);
}

void print2DCam(const uint xpos, const uint ypos,const bool (array)[HEIGHT][WIDTH]) {
	for (int8_t y = 0; y < HEIGHT; y++) {
		for (int8_t x = 0; x < WIDTH; x++) {
			LCD->SetRegion(Lcd::Rect(xpos + x + 20, ypos + y + 20, 1, 1));
			LCD->FillColor((array)[y][x] * 0xFFFF);
		}
	}
}


void clearLcd(uint16_t color) {
	LCD->Clear(color);
}


//void beepbuzzer(uint32_t t) {
//	buzzer->SetBeep(true);
//	System::DelayMs(t);
//	buzzer->SetBeep(false);
//}
//



void switchLED(int8_t id) {
	libsc::Led* LedToBlink;
	switch (id) {
	case 1:
		LedToBlink = Led1;
		break;
	case 2:
		LedToBlink = Led2;
		break;
	case 3:
		LedToBlink = Led3;
		break;
	case 4:
		LedToBlink = Led4;
		break;
	}
	LedToBlink->Switch();
}

bool GetPixel(const Byte* src, const int8_t x, const int8_t y) {
	//	const int offset = x/8 + (y * image_width / 8);
	const int offset = x / 8 + (y * WIDTH / 8);

	//	return (src[offset] << (x%8) & 0x80) ? 0 : 1;
	return (src[offset] << (x % 8) & 0x80) ? false : true;
}

void capture_image(void) {

	// capture raw image
	memcpy(data, cam->LockBuffer(), data_size);
	cam->UnlockBuffer();

	// divide image
	for (int8_t col = 0; col < WIDTH; col++) {
		for (int8_t row = 0; row < HEIGHT; row++) {
			image[row][col] = GetPixel(data, col, row);
		}
	}
}
//
//void speed_and_angle_control(RunMode& car, int current_position, double gradient){
//
////	encoder->Update();
//
//	if(car.states[1] == ACCELERATING){
//
//		float P = 2;
//		float D = 0;
//
//		//--------------------------------servo control-----------------------------------------//
//		car.ServoErr = current_position;
//		int32_t ideal_servo_degree = uint16_t(900 + P  car.ServoErr  car.ServoErr  car.ServoErr+ D  (car.ServoErr - car.ServoPrevErr));
//		// need to test, servoerr^3 may be too large for straight path
//
//		car.ServoPrevErr = car.ServoErr;
////		servo->SetDegree(servo_cap(ideal_servo_degree));
//
////		speed_control(ACCELERATING, car);
//	}
//	else if(car.states[1] == NEAR_TURN){
//
//		float P = 5;
//		float D = 0;
//
//		if(car.states[2] == TURNING_RIGHT){
//			car.ServoErr = current_position - 5; // this value needs to be tuned as well
//		}
//		else{
//			car.ServoErr = current_position + 5;
//		}
//
//		// SERVO PD for approaching turn
//		car.ServoErr = current_position;
//		int32_t ideal_servo_degree = uint16_t(900 + P  car.ServoErr  car.ServoErr  car.ServoErr+ D  (car.ServoErr - car.ServoPrevErr));
//		// need to test, servoerr^3 may be too large for approaching turn
//
//		car.ServoPrevErr = car.ServoErr;
////		servo->SetDegree(servo_cap(ideal_servo_degree));
//
////		speed_control(NEAR_TURN, car);
//
//	}
//	else if(car.states[1] == TURNING_LEFT || car.states[1] == TURNING_RIGHT){
//		float P = 8;
//		float D = 0;
//
//		gradient = gradient_cap(gradient);
//
//		// SERVO PD for straight line
////		if(gradient > 0)
////			gradient = 20 - gradient;
////		else if (gradient < 0)
////			gradient = gradient - 20;
////
////		car.ServoErr = current_position + gradient;
//
//		int32_t ideal_servo_degree = uint16_t(900 + P  car.ServoErr  car.ServoErr  car.ServoErr+ D  (car.ServoErr - car.ServoPrevErr));
//		// need to test, servoerr^3 may be too large for straight path
//
//		car.ServoPrevErr = car.ServoErr;
////		servo->SetDegree(servo_cap(ideal_servo_degree));
//
////		speed_control(TURNING_RIGHT, car);
//	}
//
//}

/*
	void current_position_PD(float& P, float& D, int k){
		// P-D values for current_position()

		if(abs(k) < 3){
			P = 50;
			D = 0;
		}
		else if (abs(k) < 5){
			P = 150;
			D = 0;
		}
		else if (abs(k) < 7){
			P = 200;
			D = 0;
		}
		else if (abs(k) < 9){
			P = 210;
			D = 0;
		}
		else if (abs(k) < 11){
			P = 230;
			D = 0;
		}
		else if (abs(k) < 13){
			P = 250;
			D = 0;
		}
		else{
			P = 250;
			D = 0;
		}
	}
 */
void print_midpoints_and_edges(int8_t midpoints[HEIGHT],int8_t ending_index){
	bool img[HEIGHT][WIDTH];
	for(int i=0 ; i<HEIGHT; i++){
		for(int j=0; j<WIDTH; j++){
			img[i][j] = 0;
		}
	}
	for(int i=HEIGHT-1; i >= ending_index; i--){
		img[i][midpoints[i]] = 1;
	}
	//	for(int i=1; HEIGHT - i >= ending_index; i++){
	//		if(edges[HEIGHT-i] != 0){
	//		img[HEIGHT-i][edges[HEIGHT-i]] = 1;
	//		}
	//		if(edges[2*HEIGHT-i] != WIDTH-1){
	//		img[HEIGHT-i][edges[2*HEIGHT-i]] = 1;
	//		}
	//	}
	print2DCam(10, 10, img);
}
