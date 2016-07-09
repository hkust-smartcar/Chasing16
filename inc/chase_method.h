#pragma once
#include <libsc/k60/jy_mcu_bt_106.h>
#include <libsc/system.h>

class ChaseMethod{
public:
	enum Role{
		leader = 0, follower
	};
	enum Commend{
		cameraImage = 0, shiftRole, changePeriod
	};
	ChaseMethod();
	ChaseMethod(Role carRole,uint8_t id);
	~ChaseMethod();

	 bool init(libsc::Timer::TimerInt period);	// to synchronize between cars, if return true then both car has syn, else keep check




protected:
	Role role;
	libsc::k60::JyMcuBt106* HC12 = nullptr;


};
