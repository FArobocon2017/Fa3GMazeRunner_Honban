#include <iostream>
#include <unistd.h>
#include "../include/WallDetector.h"

using namespace std;

int WallDetector::chkFrontWall()
{
	int	senSts[4] = {0};
	int	frontWall = 0;
	sensor.getSensorSts(senSts);
	if(senSts[0] > FL_TH && senSts[3] > FR_TH)
	{
		frontWall = 1;
	}
	else
	{
		frontWall = 0;
	}
	return frontWall;
}


int WallDetector::chkLeftWall()
{
	int	senSts[4]={0};
	int	leftWall = 0;
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
	int	rightWall = 0;
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


// senSts[4]	= {r、rf、lf、l}
// Wall[3]		= {左、正面、右}
void WallDetector::getWallData(int *wall)
{
	int	senSts[4]={0};
	sensor.getSensorSts(senSts);
	
	if(senSts[0] > FL_TH || senSts[3] > FR_TH)
	{
		wall[1] = 1;
	}
	cout  << "sen0:" << senSts[0] << " sen3:" << senSts[3] << endl;
}

// 光センサの取得
void WallDetector::getLightAverage()
{
	// sensor num
	int sensorNum = 4;

	// trial num
	int trialNum = 10;

	// 計測値を一時格納
	int tempSensor[4] = { 0 };

	// 計測値の平均
	int aveSensor[4] = { 0 };

	// スリープタイム
	int sleepTime = 3000000;

	//// 以下、処理 ////
	// ブザー鳴らす（3秒）
	//usleep(sleepTime);

	// forループ5回でセンサ平均値取得（壁と判定する距離）
	for (int i = 0; i < trialNum; i++)
	{
		// 取得
		sensor.getSensorSts(tempSensor);
		for (int j = 0; j < sensorNum; j++)
		{
			aveSensor[j] += tempSensor[j];
		}
		// 標準出力（1回分の値）
		cout << tempSensor[0] << " "
		 << tempSensor[1] << " "
		 << tempSensor[2] << " "
		 << tempSensor[3] << endl;
		usleep(1000000);
	}
	for (int j = 0; j < sensorNum; j++)
	{
		aveSensor[j] /= trialNum;
	}	

	// 標準出力（平均値）
	cout << "[ave]" << endl;
	cout << aveSensor[0] << " "
	<< aveSensor[1] << " "
	<< aveSensor[2] << " "
	<< aveSensor[3] << endl;
	cout << endl;
}
