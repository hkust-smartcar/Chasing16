#pragma once
#include <libsc/k60/jy_mcu_bt_106.h>
#include <libsc/us_100.h>
#include <libsc/system.h>

using namespace libsc;
using namespace libsc::k60;

#define TOO_CLOSE_FROM_FRONT_CAR -32760

class ChaseMethod{
public:
	enum Role{
		leader = 0, follower, solo
	};
	enum Command{
		noAction = 0, shiftRole, traingleForward , straightPass, stopCar, error
	};

	ChaseMethod();
	ChaseMethod(Role carRole,uint8_t Hc12_id,uint8_t Usensor_id ,int16_t* speed);
	~ChaseMethod();
	Role getCurrentRole(){ return role;}

	/* Bluetooth */
	Command getCommand();	// get the command from other car
	Command getPreviousCommand(){ return command;}
	void sendCommand(ChaseMethod::Command);		// send command to all channel
	void excuteCommand();

	/* Usensor */
	void update_Usensor();	//re-shoot unltra sonic wave
	bool checkUSensor();
	uint16_t getFrontObjDistance(){return objDistance;}
	int16_t distanceControl();



	static bool listener(const Byte *data, const size_t size);

private:
	 UartDevice::Config getBluetoothConfig(uint8_t id);
	 Us100::Config getUS100Config(uint8_t id);
	 static ChaseMethod *m_instance;

protected:
	Role role = solo;
	JyMcuBt106 HC12;
	Us100 USsensor;
	Byte DataReceive = 0;
	Command command = noAction;
	uint16_t objDistance = 65535;
	bool sensorWorked = false;
	int16_t* motorSpeed ;
	int16_t distanceError = 0, distanceErrorPrev = 0, distancePIDOutput = 0, idealDistance;
	float distance_kp, distance_kd;


};
