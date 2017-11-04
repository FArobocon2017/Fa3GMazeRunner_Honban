#include <iostream>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "MicroMouseDriver.h"

using namespace std;

//ロボットを直進させるときのスリープ時間を計算
//double stepNum = ステップ数
//return スリープさせる時間(micro sec)
int MicroMouseDriver::calculateSleepTime(int distance, int stepNum)
{
	int sleepTime = 0;
	//式：STEP_ANG*ステップ数/360*直径*π*秒=進む距離
	if (stepNum < 0)
	{
		stepNum = stepNum*-1;
	}

	//1秒あたりの回転角度
	double rotationAng = STEP_ANG * stepNum;
	//cout << "rotationAng" << rotationAng << endl;
	//1秒あたりの回転数
	double rotatioNum = rotationAng / DEGREE;
	//cout << "rotatioNum" << rotatioNum << endl;
	//車輪の周
	double wheelCircumference = WHEEL_RADIUS * PI;
	//cout << "wheelCircumference" << wheelCircumference << endl;
	//スリープさせる時間
	sleepTime = (1000000 * distance) / rotatioNum / wheelCircumference;
	//cout << "sleepTime" << wheelCircumference << endl;

	return sleepTime;
}


//ロボットを旋回させるときのスリープ時間を計算
//double turnDegree = 旋回させる角度
//double stepNum = ステップ数
//return スリープさせる時間(micro sec)
int MicroMouseDriver::calculateTurnSleepTime(int turnDegree, int stepNum)
{
	int sleepTime = 0;
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
	//スリープさせる時間
	sleepTime = (turnDistance * 1000000) / rotatioNum / wheelCircumference;

	return sleepTime;
}


//ロボットを停止させる関数
void MicroMouseDriver::stop()
{
	motor.ctrMotorStop();
}


//ロボットを指定ブロック数直進させる関数
void MicroMouseDriver::driveNBlock(int N)
{
	cout << "driveNBlock = " << N << endl;
	int sleepTime = (int)calculateSleepTime(BLOCK, STEP_MIDDLE)*N;
	motor.ctrMotorHz(STEP_MIDDLE-HOSEI20, STEP_MIDDLE-HOSEI20);
	usleep(sleepTime);
	stop();
}


//ロボットを指定ブロック数後退させる関数
void MicroMouseDriver::riverseNBlock(int N)
{
	cout << "riverseNBlock = " << N << endl;
	int sleepTime = (int)calculateSleepTime(BLOCK, STEP_MIDDLE)*N;
	motor.ctrMotorHz(-STEP_MIDDLE, -STEP_MIDDLE);
	usleep(sleepTime);
	stop();
}

//ロボットを左旋回させる関数
void MicroMouseDriver::spinLeft()
{
	cout << "spinLeft"<< endl;
	int sleepTime = (int)calculateTurnSleepTime(80, STEP_MIDDLE);
	motor.ctrMotorHz(-STEP_MIDDLE-HOSEI10, STEP_MIDDLE+HOSEI10);
	usleep(sleepTime);
	stop();
}


//ロボットを右旋回させる関数
void MicroMouseDriver::spinRight()
{
	cout << "spinRight"<< endl;
	int sleepTime = (int)calculateTurnSleepTime(80, STEP_MIDDLE);
	motor.ctrMotorHz(STEP_MIDDLE+HOSEI10, -STEP_MIDDLE-HOSEI10);
	usleep(sleepTime);
	stop();
}


//ロボットを180度転回させる関数
void MicroMouseDriver::inverse()
{
	cout << "inverse"<< endl;
	int sleepTime = (int)calculateTurnSleepTime(175, STEP_MIDDLE);
	motor.ctrMotorHz(STEP_MIDDLE, -STEP_MIDDLE);
	usleep(sleepTime);
	stop();
}



//ロボットを左に1マス進めさせる関数
void MicroMouseDriver::turnLeft()
{
	cout << "turnLeft"<< endl;
	spinLeft();
	driveNBlock(1);
}


//ロボットを右に1マス進めさせる関数
void MicroMouseDriver::turnRight()
{
	cout << "turnRight"<< endl;
	spinRight();
	driveNBlock(1);
}


//ロボットを右に1マス進めさせる関数
void MicroMouseDriver::slideMM(int distance)
{
	cout << "slideMM = " << distance << endl;
	int absDistance = 0;
	if (distance < 0) absDistance = distance*(-1);
	else absDistance = distance;

	//回転
	double turnAng = atan2((double)absDistance, 60)*RADIAN_TO_ANG;
	int turnAngInt = (int)turnAng;
	//cout << "rotationAng=" << turnAng << endl;
	//cout << "rotationAng=" << turnAngInt << endl;
	if(distance > 0) turnNAngle(-turnAngInt);
	else		turnNAngle(turnAngInt);
	stop();

	//バック
	double backDistance = acos(turnAng*ANG_TO_RADIAN) * 60;
	//cout << "driveMM=" << backDistance << endl;
	driveMM((int)-backDistance);
	stop();

	//回転
	if(distance > 0) turnNAngle(turnAngInt);
	else turnNAngle(-turnAngInt);
	stop();

	//戻る
	driveMM(60);
	stop();
}


//ロボットを指定距離進ませる関数
void MicroMouseDriver::driveMM(int distance)
{
	cout << "driveMM = " << distance << endl;
	//距離が0の場合は停止して終わり
	if (distance == 0)
	{
		stop();
		return;
	}

	int sleepTime = 0; 

	if (distance > 0)
	{
		sleepTime= (int)calculateSleepTime(distance, STEP_SLOW);
		motor.ctrMotorHz(STEP_SLOW, STEP_SLOW);
	}
	else
	{
		sleepTime= (int)calculateSleepTime(-distance, STEP_SLOW);
		motor.ctrMotorHz(-STEP_SLOW, -STEP_SLOW);
	}

	usleep(sleepTime);

	stop();
}


//ロボットを指定確度旋回させる関数
void MicroMouseDriver::turnNAngle(int angle)
{
	cout << "turnNAngle = " << angle << endl;
	int sleepTime = 0;
	if(angle < 0)
	{
		sleepTime = (int)calculateTurnSleepTime(-angle, STEP_MIDDLE);
		motor.ctrMotorHz(STEP_MIDDLE, -STEP_MIDDLE);
	}
	else if(angle > 0)
	{
		sleepTime = (int)calculateTurnSleepTime(angle, STEP_MIDDLE);
		motor.ctrMotorHz(-STEP_MIDDLE, STEP_MIDDLE);
	}

	usleep(sleepTime);

	motor.ctrMotorStop();
}

