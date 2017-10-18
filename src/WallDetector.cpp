#include "../include/WallDetector.h"

using namespace std;

int WallDetector::chkFrontWall()
{
	int	senSts[4] = {0};
	int frontWall = 0;
	sensor.getSensorSts(senSts);
	if(senSts[0] > FL_TH && senSts[3] > FR_TH)
	{
		frontWall = 1;
	}
	else{
		frontWall = 0;
	}
	return frontWall;
}


int WallDetector::chkLeftWall()
{
	int	senSts[4]={0};
	int leftWall = 0;
	sensor.getSensorSts(senSts);
	if(senSts[1] > L_TH)
	{
		leftWall = 1;
	}
	else
	{
		leftWall = 0;
	}
	return leftWall;
}


int WallDetector::chkRightWall()
{
	int	senSts[4]={0};
	int rightWall = 0;
	sensor.getSensorSts(senSts);
	if(senSts[2] > R_TH)
	{
		rightWall = 1;
	}
	else
	{
		rightWall = 0;
	}
	return rightWall;
}


// senSts[4]	= {左、左前、右前、右}
// Wall[3]		= {左、正面、右}
void WallDetector::getWallData(int *wall)
{
	int	senSts[4]={0};
	sensor.getSensorSts(senSts);
	
	if(senSts[0] < FL_TH && senSts[1] < L_TH && senSts[2] < R_TH && senSts[3] < FR_TH)
	{
		wall[0] = 0;		wall[1] = 0;		wall[2] = 0;
	}
	else if(senSts[0] < FL_TH && senSts[1] > L_TH && senSts[2] < R_TH && senSts[3] < FR_TH)
	{
		wall[0] = 1;		wall[1] = 0;		wall[2] = 0;
	}
	else if(senSts[0] > FL_TH && senSts[1] < L_TH && senSts[2] < R_TH && senSts[3] > FR_TH)
	{
		wall[0] = 0;		wall[1] = 1;		wall[2] = 0;
	}
	else if(senSts[0] > FL_TH && senSts[1] > L_TH && senSts[2] < R_TH && senSts[3] > FR_TH)
	{
		wall[0] = 1;		wall[1] = 1;		wall[2] = 0;
	}
	else if(senSts[0] < FL_TH && senSts[1] < L_TH && senSts[2] > R_TH && senSts[3] < FR_TH)
	{
		wall[0] = 0;		wall[1] = 0;		wall[2] = 1;
	}
	else if(senSts[0] < FL_TH && senSts[1] > L_TH && senSts[2] > R_TH && senSts[3] < FR_TH)
	{
		wall[0] = 1;		wall[1] = 0;		wall[2] = 1;
	}
	else if(senSts[0] > FL_TH && senSts[1] < L_TH && senSts[2] > R_TH && senSts[3] > FR_TH)
	{
		wall[0] = 0;		wall[1] = 1;		wall[2] = 1;
	}
	else if(senSts[0] > FL_TH && senSts[1] > L_TH && senSts[2] > R_TH && senSts[3] > FR_TH)
	{
		wall[0] = 1;		wall[1] = 1;		wall[2] = 1;
	}
}