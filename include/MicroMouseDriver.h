#pragma once

#include "Motor.h"

#define	STEP_ANG				0.9		//モータが1ステップ辺りに回転する角度
#define	STEP_SLOW				500		//1秒あたりにモーターに与えるステップ数
#define	STEP_MIDDLE				500		//1秒あたりにモーターに与えるステップ数
#define	STEP_FAST				1500		//1秒あたりにモーターに与えるステップ数
#define	PI					3.14159	//π
#define	WHEEL_RADIUS				47		//車輪の直径(mm)
#define	DEGREE					360		//360°
#define	BLOCK					180		//1ブロックの大きさ(mm)
#define	WHEEL_AXLE				95		//車軸の長さ(mm)

class MicroMouseDriver
{
	Motor motor;
	int calculateSleepTime(int stepNum);
	int calculateTurnSleepTime(int turnDegree, int stepNum);
	
public:
	MicroMouseDriver(){};
	~MicroMouseDriver(){};
	
	void stop();
	void driveNBlock(int N);
	void riverseNBlock(int N);
	void spinLeft();
	void spinRight();
	void inverse();
	void turnLeft();
	void turnRight();
};