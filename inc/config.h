/*
 * config.h
 *
 *  Created on: 2016¦~1¤ë15¤é
 *      Author: yungc
 */

#pragma once

//put all config here

#include <functional>
#include <libsc/system.h>
#include <libsc/led.h>
#include <libbase/k60/mcg.h>
#include <libsc/ab_encoder.h>
#include <libsc/futaba_s3010.h>
#include <libsc/k60/ov7725.h>
#include <libsc/button.h>
#include <libsc/joystick.h>
#include <libsc/st7735r.h>
#include <libsc/simple_buzzer.h>
#include <libsc/dir_motor.h>

using namespace libsc;

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

//LED 1~4
Led::Config GetLed1Config()
{
	Led::Config Led1Config;
	Led1Config.id = 0;
	Led1Config.is_active_low = true;
	return Led1Config;
}
Led::Config GetLed2Config()
{
	Led::Config Led2Config;
	Led2Config.id = 1;
	Led2Config.is_active_low = true;
	return Led2Config;
}
Led::Config GetLed3Config()
{
	Led::Config Led3Config;
	Led3Config.id = 2;
	Led3Config.is_active_low = true;
	return Led3Config;
}
Led::Config GetLed4Config()
{
	Led::Config Led4Config;
	Led4Config.id = 3;
	Led4Config.is_active_low = true;
	return Led4Config;
}

//encoder
AbEncoder::Config GetAbEncoderConfig()
{
	AbEncoder::Config EncoderConfig;
	EncoderConfig.id = 0;
	return EncoderConfig;
}

//servo for turning
FutabaS3010::Config GetServoConfig()
{
	FutabaS3010::Config ServoConfig;
	ServoConfig.id = 0;
	return ServoConfig;
}

//AlternateMotor::Config GetAltmotorConfig()
//{
//	AlternateMotor::Config AltmotorConfig;
//	AltmotorConfig.id = 0;
//	return AltmotorConfig;
//}

DirMotor::Config GetDirmotorConfig()
{
	DirMotor::Config DirmotorConfig;
	DirmotorConfig.id = 0;
	return DirmotorConfig;
}

Button::Config GetButton1Config()
{

	Button::Config Button1Config;
	Button1Config.id = 0;
	Button1Config.is_active_low = true;
	Button1Config.is_use_pull_resistor = false;
//	Button1Config.listener_trigger = Button::Config::Trigger::kDown;
//	Button1Config.listener = &blinkLed;
	//TODO add ISR for Button
	return Button1Config;
}

Button::Config GetButton2Config()
{
	Button::Config Button2Config;
	Button2Config.id = 1;
	Button2Config.is_active_low = true;
	Button2Config.is_use_pull_resistor = false;
	return Button2Config;
}

Joystick::Config GetJoystickConfig()
{
	Joystick::Config JoystickConfig;
	JoystickConfig.id = 0;
	JoystickConfig.is_active_low = true;
//	JoystickConfig.listener_triggers[static_cast<int>(Joystick::State::kDown)] =
//			Joystick::Config::Trigger::kDown;
//	JoystickConfig.listeners[static_cast<int>(Joystick::State::kDown)] =
//						[&](const uint8_t)
//				{};
	//TODO add ISR for every joystick state
	return JoystickConfig;
}

St7735r::Config GetLcdConfig()
{
	St7735r::Config LcdConfig;
	return LcdConfig;
}
LcdTypewriter::Config GetLcdWriterConfig(libsc::St7735r* LCD){
	LcdTypewriter::Config LcdWconfig;
	LcdWconfig.lcd = LCD;
	return LcdWconfig;
}
SimpleBuzzer::Config GetBuzzerConfig()
{
	SimpleBuzzer::Config BuzzerConfig;
	BuzzerConfig.id = 0;
	BuzzerConfig.is_active_low = false;
	return BuzzerConfig;
}

k60::Ov7725::Config GetCameraConfig(){
	k60::Ov7725::Config camcfg;
	camcfg.id=0;
	camcfg.w=80;
	camcfg.h=60;
	camcfg.fps=k60::Ov7725::Config::Fps::kHigh;
//	camcfg.contrast = 0x35;
//	camcfg.brightness=0x35;
	return camcfg;
}

