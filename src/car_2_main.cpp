#include <cassert>
#include <cstring>
#include <libsc/system.h>
#include <stdint.h>
#include "car_2_img_process.h"
#include <cmath>


using namespace libsc;
using namespace libbase::k60;

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

Byte data[WIDTH * HEIGHT / 8];
bool image[HEIGHT][WIDTH];
int8_t midpoints[HEIGHT];
int8_t midpoint_ending_index = HEIGHT;
int8_t absolute_midpoint_ending_index = HEIGHT;

void Config_all();
void printRawCamGraph(const int8_t x, const int8_t y, Byte* data);
void print2DCam(const uint xpos, const uint ypos,const bool (array)[HEIGHT][WIDTH]) ;
void clearLcd(uint16_t color);
void switchLED(int8_t id);
bool GetPixel(const Byte* src, const int8_t x, const int8_t y) ;
void capture_image(void);
void print_Count(int encoder_count);
void print_midpoints_and_edges(int8_t midpoints[HEIGHT],int8_t ending_index);

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

};

int main(void)
{
	System::Init();//must init for using LCD
	bool jobsdone[] = {0,0,0,0,0,0};
	Timer::TimerInt current_time = 0;
	Timer::TimerInt triangle_time = 0;
	Timer::TimerInt past_time = 0, past_time1 = 0, past_time2 = 0, past_time3 = 0, past_time4 = 0, past_time5 = 0;
	int8_t x = 1;

	bool triangle_detector_1 = false, triangle_detector_2 = false, triangle_detector_3 = false;
	bool triangle = false, reached_triangle = false;
	int triangle_count_1 = 0, triangle_count_2 = 0, triangle_count_3 = 0;
	int area = 0;int16_t count = 0;
	RunMode Runner;

	while(1){

		if(current_time !=System::Time()){
			current_time = System::Time();

			Led1->SetEnable(x%4 == 0);
			Led2->SetEnable(x%4 == 1);
			Led3->SetEnable(x%4 == 2);
			Led4->SetEnable(x%4 == 3);

			// ticks one
			if((int32_t)(current_time - past_time) >= 0 && !jobsdone[0] && !jobsdone[1] && jobsdone[2]){
				past_time = current_time;

				int16_t TCount = 0;
				int16_t WCount = 0;

				capture_image();
				TCount = update_midpoints_and_edges(image,midpoints, midpoint_ending_index,absolute_midpoint_ending_index, Runner.last_midpoint,Runner.max_consecutive_midpoint_diff,Runner.separating_index, Runner.TrackArea, WCount);
				Runner.current_state = determine_state(midpoints,midpoint_ending_index, absolute_midpoint_ending_index, TCount, WCount, Runner.separating_index, 0, triangle, reached_triangle);

				float P = 0; float D = 0;

				if(Runner.current_state == GoStraight){
					Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(9/10)*(-midpoint_ending_index + HEIGHT-1));
					//				P = 15;
					P = 12;
					D = 0;

					//			count = 300;
				}
				else if(Runner.current_state == SPath){
					Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(11/12)*(-midpoint_ending_index + HEIGHT-1));
					//				P = 13 - ((encoder->GetCount() + 90)/20)*2;
					//				P = 13.5;
					//				P = 17.5;
					P = 18;
					D = 0;

					//			count = 210;
				}
				else if(Runner.current_state == TurnLeft){
					Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
					//				P = 16.5 - ((encoder->GetCount() + 90)/20)*2;
					//				P = 20;
					//				P = 26.5;
					P = 27.5;
					D = 0;

				}
				else if(Runner.current_state == TurnRight){
					Runner.ServoErr = -average_midpoint_error(midpoints,HEIGHT-1,midpoint_ending_index +(10/11)*(-midpoint_ending_index + HEIGHT-1));
					//				P = 16.5 - ((encoder->GetCount() + 90)/20)*2;
					//				P = 20;
					//				P = 28.5;
					P = 28.5;
					D = 0;

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

				count = 360;

				int32_t ideal_servo_degree = uint16_t(1000 + P*Runner.ServoErr + D*(Runner.ServoErr - Runner.ServoPrevErr));
				Runner.ServoPrevErr = Runner.ServoErr;
				Runner.ServoPrev1Err = Runner.ServoPrevErr;
				servo->SetDegree(servo_cap_ome(ideal_servo_degree));

			}

			jobsdone[0] = true;
		}

		// ticks two
		if((int32_t)(current_time - past_time1) >= 5 && jobsdone[0] && !jobsdone[1] && !jobsdone[2]){
			past_time1 = current_time;

			float Kd = 0.008f;
			float Ki = 0.009f;
			float Kp = 0.014;

			encoder->Update();
			Runner.MotorErr = count + encoder->GetCount();
			Runner.ideal_motor_speed
			+= Kp*(Runner.MotorErr - Runner.MotorPrev1Err) + Ki*Runner.MotorErr + Kd * (Runner.MotorErr - 2*Runner.MotorPrev1Err + Runner.MotorPrev2Err);

			Runner.MotorPrev2Err = Runner.MotorPrev1Err;
			Runner.MotorPrev1Err = Runner.MotorErr;
			motor->SetClockwise(1);
			//		motor->SetPower((Runner.ideal_motor_speed - 434.43)/(2.7824) + 196);	//Transformation from count to motor speed
			motor->SetPower(Runner.ideal_motor_speed);

			jobsdone[1] = true;
		}

		// ticks three
		if((int32_t)(current_time - past_time2) >= 6 && jobsdone[0] && jobsdone[1] && !jobsdone[2]){
			past_time2 = current_time;

			absolute_midpoint_ending_index = 0;
			if(!reached_triangle){
				area = check_triangle(image, absolute_midpoint_ending_index, WIDTH/2);
				if (area == 0){
					area = check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 - 3);
				}
				if (area == 0){
					area = check_triangle(image, absolute_midpoint_ending_index, WIDTH/2 + 3);
				}
				if(area > 150 && area < 225 && !triangle_detector_2 && !triangle_detector_3){
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
					if(area > 225 && area < 300){
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
					if(area > 300){
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
			}


			jobsdone[2] = true;
		}

		// ticks four
		if((int32_t)(current_time - past_time3) >= 1){
			past_time3 = current_time;




			jobsdone[3] = true;
		}

		// can have more ticks below




		//to give condition for ticks, like  below, ticks 1 and 2 must both excute once before any of them run second time
		if(jobsdone[0] == true && jobsdone[1] == true){
			jobsdone[0] = false;
			jobsdone[1] = false;
		}

		x = int(++x)%4;
	}
	return 0;
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
	LcdConfig.is_revert = true;
	LCD = new St7735r(LcdConfig);
	LCD->Clear(0);

	//	SimpleBuzzer::Config BuzzerConfig;
	//	BuzzerConfig.id = 0;
	//	BuzzerConfig.is_active_low = false;
	//	buzzer = new SimpleBuzzer(BuzzerConfig);

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

	//	memset(data, 0, WIDTH * HEIGHT);
	//	memset(image, false, true *WIDTH *HEIGHT);

}

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



