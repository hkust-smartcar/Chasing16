/*
 * chase_method.cpp
 *
 *  Created on: 2016¦~6¤ë3¤é
 *      Author: yungc
 */
#include "chase_method.h"


using namespace libsc;
using namespace libsc::k60;

ChaseMethod *ChaseMethod::m_instance = nullptr;

ChaseMethod::ChaseMethod():
	HC12(getBluetoothConfig(0)),
	USsensor(getUS100Config(1))
{
	distance_kp = 1;
	distance_kd = 0;
	idealDistance = 500;
	role = solo;
	if (!m_instance)
		m_instance = this;
}

ChaseMethod::ChaseMethod(Role carRole,uint8_t Hc12_id,uint8_t Usensor_id ,int16_t* speed):
		HC12(getBluetoothConfig(Hc12_id)),
		USsensor(getUS100Config(Usensor_id))
{
	distance_kp = 1;
	distance_kd = 0;
	idealDistance = 500;
	role = carRole;
	motorSpeed = speed;
	if (!m_instance)
		m_instance = this;


}

ChaseMethod::~ChaseMethod(){

}

UartDevice::Config ChaseMethod::getBluetoothConfig(uint8_t id){
	UartDevice::Config JyCfg;
	JyCfg.id = id;
	JyCfg.baud_rate = libbase::k60::Uart::Config::BaudRate::k115200;
	JyCfg.rx_isr = &listener;
	return JyCfg;
}

Us100::Config ChaseMethod::getUS100Config(uint8_t id){
	Us100::Config Uscfg;
	Uscfg.id = id;
	return Uscfg;
}

int16_t ChaseMethod::distanceControl(){
	distanceErrorPrev = distanceError;
	if(objDistance >0 && objDistance < 800)distanceError = idealDistance -objDistance;
	else if(objDistance == 65535) distanceError = idealDistance;
	distancePIDOutput = distance_kp*distanceError + distance_kd*(distanceErrorPrev-distanceError);
	if(objDistance <200) distancePIDOutput = TOO_CLOSE_FROM_FRONT_CAR;
	return distancePIDOutput;
}

void ChaseMethod::excuteCommand(){
	if(command == shiftRole) {
		if(role == leader) role = follower;
		else role = leader;
	}
	else if (command == stopCar) motorSpeed = 0;

	// not action for other,
}

ChaseMethod::Command ChaseMethod::getCommand(){

	switch(DataReceive){
	case '1':
		command = noAction;
		break;
	case '2':
		command = shiftRole;
		break;
	case '3':
		command = traingleForward;
		break;
	case '4':
		command = straightPass;
		break;
	case '5':
		command = stopCar;
		break;
	default:
		command = error;
		break;
	}

	return command;
}

void ChaseMethod::sendCommand(ChaseMethod::Command DataSend){

	switch(DataSend){
		case noAction:
			HC12.SendStr("1");
			break;
		case shiftRole:
			HC12.SendStr("2");
			break;
		case traingleForward:
			HC12.SendStr("3");
			break;
		case straightPass:
			HC12.SendStr("4");
			break;
		case stopCar:
			HC12.SendStr("5");
			break;
		default:
			HC12.SendStr("6");
			break;
		}
}


void ChaseMethod::update_Usensor(){
	USsensor.Stop();
	USsensor.Start();
}

bool ChaseMethod::checkUSensor(){

	if(USsensor.IsAvailable()){
		objDistance = USsensor.GetDistance();
		sensorWorked = true;
	}
	else {
		objDistance = 65535;
		sensorWorked = false;

	}

	return sensorWorked;
}

bool ChaseMethod::listener(const Byte *data, const size_t size)
{
	m_instance->DataReceive = data[0];
	return true;
}
