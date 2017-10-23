#pragma once

#include "Sensor.h"

#define FL_TH	1500
#define L_TH	200
#define R_TH	200
#define FR_TH	1500

#define M	500

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