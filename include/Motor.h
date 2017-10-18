#pragma once

class Motor
{
public:
	Motor();
	~Motor();
	void motorSwOn(void);
	void motorSwOff(void);
	void ctrMotorHz(int lVal, int rVal);
	void ctrMotorStop(void);
};