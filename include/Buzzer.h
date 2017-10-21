#pragma once

#define OFF	0
#define DO	261
#define RE	293
#define MI	329
#define FA	349
#define SO	392
#define RA	440
#define SI	493
#define DO_U	523

class Buzzer
{
	public:
	Buzzer(){};
	void putBuzzerOff();
	void putBuzzerPattern(int pattern);
	void putBuzzerHz(int hz);
};