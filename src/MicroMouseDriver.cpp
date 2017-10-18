#include <iostream>
#include <unistd.h>
#include "../include/MicroMouseDriver.h"

using namespace std;

//ロボットを直進させるときのスリープ時間を計算
//double stepNum = ステップ数
//return スリープさせる時間
int MicroMouseDriver::calculateSleepTime(int stepNum)
{
	//式：STEP_ANG*ステップ数/360*直径*π*秒=進む距離
	if (stepNum < 0)
	{
		stepNum = stepNum*-1;
	}
	//1秒あたりの回転角度
	double rotationAng = STEP_ANG * stepNum;
cout << "rotationAng" << rotationAng << endl;
	//1秒あたりの回転数
	double rotatioNum = rotationAng / DEGREE;
cout << "rotatioNum" << rotatioNum<< endl;
	//車輪の周
	double wheelCircumference = WHEEL_RADIUS * PI;
cout << "wheelCircumference" << wheelCircumference << endl;
	
	return BLOCK / rotatioNum / wheelCircumference;
}


//ロボットを旋回させるときのスリープ時間を計算
//double turnDegree = 旋回させる角度
//double stepNum = ステップ数
//return スリープさせる時間
int MicroMouseDriver::calculateTurnSleepTime(int turnDegree, int stepNum)
{
	//式：STEP_ANG*ステップ数/360*直径*π*秒=進む距離()
	if (stepNum < 0)
	{
		stepNum = stepNum*-1;
	}
	//1秒あたりの回転角度
	double rotationAng = STEP_ANG * stepNum;
	//1秒あたりの回転数
	double rotatioNum = rotationAng / DEGREE;
	//車輪の周
	double wheelCircumference = WHEEL_RADIUS * PI;
	//車軸の周
	double wheelAxleCircumference = WHEEL_AXLE * PI;
	//車輪の回転する距離
	double turnDistance = wheelAxleCircumference * turnDegree / DEGREE;
	
	return turnDistance / rotatioNum / wheelCircumference;
}


//ロボットを停止させる関数
void MicroMouseDriver::stop()
{
	motor.ctrMotorStop();
}


//ロボットを指定ブロック数直進させる関数
void MicroMouseDriver::driveNBlock(int N)
{
	motor.ctrMotorHz(STEP_MIDDLE, STEP_MIDDLE);
	usleep((int)calculateSleepTime(STEP_MIDDLE)*N*1000000);
	stop();
cout << "usleep" << STEP_MIDDLE << endl;
cout << "usleep" << calculateSleepTime(STEP_MIDDLE) << endl;
cout << "usleep" << calculateSleepTime(STEP_MIDDLE)*N*1000000<< endl;
}


//ロボットを指定ブロック数後退させる関数
void MicroMouseDriver::riverseNBlock(int N)
{
	motor.ctrMotorHz(-STEP_MIDDLE, -STEP_MIDDLE);
	usleep((int)calculateSleepTime(STEP_MIDDLE)*N*1000000);
	stop();
}


//ロボットを左旋回させる関数
void MicroMouseDriver::spinLeft()
{
	motor.ctrMotorHz(-STEP_MIDDLE, STEP_MIDDLE);
	usleep((int)calculateTurnSleepTime(90, STEP_MIDDLE)*1000000);
	stop();
}


//ロボットを右旋回させる関数
void MicroMouseDriver::spinRight()
{
	motor.ctrMotorHz(STEP_MIDDLE, -STEP_MIDDLE);
	usleep((int)calculateTurnSleepTime(90, STEP_MIDDLE)*1000000);
	stop();
}


//ロボットを180度転回させる関数
void MicroMouseDriver::inverse()
{
	motor.ctrMotorHz(STEP_MIDDLE, -STEP_MIDDLE);
	usleep((int)calculateTurnSleepTime(180, STEP_MIDDLE)*1000000);
	stop();
}


//ロボットを左に1マス進めさせる関数
void MicroMouseDriver::turnLeft()
{
	spinLeft();
	driveNBlock(1);
}


//ロボットを右に1マス進めさせる関数
void MicroMouseDriver::turnRight()
{
	spinRight();
	driveNBlock(1);
}
