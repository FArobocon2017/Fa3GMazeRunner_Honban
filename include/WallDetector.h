#pragma once

#include "Sensor.h"

#define FL_TH	1000
#define L_TH	120
#define R_TH	140
#define FR_TH	1000

#define M	150

class WallDetector
{
	Sensor sensor;
	
public:
	WallDetector(){};
	~WallDetector(){};
	
	int chkFrontWall();
	int chkLeftWall();
	int chkRightWall();
	void getWallData(int*);
	void getLightAverage();
};