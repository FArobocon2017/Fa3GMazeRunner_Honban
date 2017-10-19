#include <iostream>

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

	// 光センサの取得
	void WallDetector::geLightAverage()
	{
		// 計測回数最大値
		int counterSenseMax = 5;

		// 計測値を一時格納
		int tempLight[4] = { 0 };

		// 計測値の平均
		int aveCloseantLight[4] = { 0 };

		// スリープタイム
		int sleeptime = 300000;


		//// 以下、処理 ////
		// ブザー鳴らす（3秒）
		usleep(sleeptime);
 
		// forループ5回でセンサ平均値取得（壁と判定する距離）
		for (int i = 0; i < counterSenseMax; i++)
		{
			// 取得
			sensor.getSensorSts(tempLight);

			// 標準出力（1回分の値）
			cout << tempLight[0] << " " <<  tempLight[1] << " " << tempLight[2] << " " << tempLight[3] << endl;

			for (int j = 0; j < counterSenseMax; j++)
			{
				aveCloseantLight[j] += tempLight[j] / counterSenseMax;
			}
		}

		// 標準出力（平均値）
		cout << "[ave]" << endl;
		cout << tempLight[0] << " " << tempLight[1] << " " << tempLight[2] << " " << tempLight[3] << endl;
		cout << endl;
	}
}