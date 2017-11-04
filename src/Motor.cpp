#include <iostream>
#include <fstream>
#include "../include/Motor.h"

using namespace std;

Motor::Motor()
{
	ofstream motorSw("/dev/rtmotoren0");
	motorSw << 1;
	motorSw.close();
}

Motor::~Motor()
{
	ofstream motorSw("/dev/rtmotoren0");
	motorSw << 0;
	motorSw.close();
}


void Motor::motorSwOn(void)
{
	ofstream motorSw("/dev/rtmotoren0");
	motorSw << 1;
	motorSw.close();
}

void Motor::motorSwOff(void)
{
	ofstream motorSw("/dev/rtmotoren0");
	motorSw << 0;
	motorSw.close();
}

void Motor::ctrMotorHz(int lVal, int rVal)
{
	ofstream lMotor("/dev/rtmotor_raw_l0");
	ofstream rMotor("/dev/rtmotor_raw_r0");
	lMotor << lVal;
	rMotor << rVal;
	lMotor.close();
	rMotor.close();
}

void Motor::ctrMotorStop(void)
{
	ofstream lMotor("/dev/rtmotor_raw_l0");
	ofstream rMotor("/dev/rtmotor_raw_r0");
	lMotor << 0;
	rMotor << 0;
	lMotor.close();
	rMotor.close();
}