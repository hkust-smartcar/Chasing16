#include <cassert>
#include <cstring>
#include <libsc/system.h>
#include <stdint.h>


using namespace libsc;
using namespace libbase::k60;



int main(void)
{
	System::Init();//must init for using LCD
	bool jobsdone[] = {0,0,0,0,0,0};
	Timer::TimerInt current_time = 0;
	Timer::TimerInt past_time = 0, past_time1 = 0, past_time2 = 0, past_time3 = 0, past_time4 = 0, past_time5 = 0;


	while(1){

		if(current_time !=System::Time()){
			current_time = System::Time();

			// ticks one
			if((int32_t)(current_time - past_time) >= 1){
				past_time = current_time;



				jobsdone[0] = true;
			}

			// ticks two
			if((int32_t)(current_time - past_time1) >= 1){
				past_time1 = current_time;




				jobsdone[1] = true;
			}

			// ticks three
			if((int32_t)(current_time - past_time2) >= 1){
				past_time2 = current_time;




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

		}

	}


	return 0;
}
