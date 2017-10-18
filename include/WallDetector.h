#pragma once

#include "Sensor.h"

#define FL_TH	300
#define L_TH		300
#define R_TH		300
#define FR_TH	300

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
};