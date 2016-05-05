#pragma once
#include <cassert>
#include <cstring>
#include <stdio.h>
#include <libsc/system.h>
#include <libsc/timer.h>

class Tick{
public:

	struct TickObj{
		libsc::Timer::TimerInt persistTime = NULL;
		libsc::Timer::TimerInt startTime = NULL;
		int8_t priority = 20; // 0 to 20, 0 the most important
		int (*job)();
	};

	Tick();

	Tick(libsc::Timer::TimerInt period);

	TickObj getTimeOfAFunction();//return timeCounter, time consumed by a specific function

	void queueJob( TickObj input );

	void Loop();//run this in main

	void updateRemainTime(){	RemainingTime =libsc::System::Time();	}

	libsc::Timer::TimerInt getRemainTime(){	return RemainingTime;	}

	TickObj getCurTick(){	return Tick;	}//return tick

	int8_t getCurJobInQueue(){	return curJobInQueue; }

	void setPeriod(libsc::Timer::TimerInt input){	Period = input;	}

	libsc::Timer::TimerInt getPeriod(){	return Period;	}

private:

	libsc::Timer::TimerInt Period = NULL;

	libsc::Timer::TimerInt Tick = NULL;

	libsc::Timer::TimerInt TimeCounter = NULL;

	libsc::Timer::TimerInt RemainingTime = NULL;

	int8_t curJobInQueue = 0;

	int8_t totalJobQueue = 0;

	TickObj jobList[20] = {NULL};




};
