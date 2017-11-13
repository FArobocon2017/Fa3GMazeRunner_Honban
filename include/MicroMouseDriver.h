#pragma once
#include <thread>
#include <mutex>
#include "Motor.h"

#define	STEP_ANG				0.9		//モータが1ステップ辺りに回転する角度
#define	STEP_SLOW				200		//1秒あたりにモーターに与えるステップ数
#define	STEP_MIDDLE				400		//1秒あたりにモーターに与えるステップ数
#define	STEP_FAST				1500	//1秒あたりにモーターに与えるステップ数
#define	PI					    3.14159	//π
#define	WHEEL_RADIUS			47		//車輪の直径(mm)
#define	DEGREE					360		//360°
#define	BLOCK					185		//1ブロックの大きさ(mm)
#define	WHEEL_AXLE				95		//車軸の長さ(mm)
#define	RADIAN_TO_ANG			57.2958	
#define	ANG_TO_RADIAN			0.0174	
#define	HOSEI5					5		
#define	HOSEI10					10		
#define	HOSEI15					15		
#define	HOSEI20					20		
#define	HOSEI30					30		
#define	HOSEI50					50	

class MicroMouseDriver
{
	Motor motor;
	mutable std::mutex m_mutex;
	int m_lVal, m_rVal;
	bool m_bStopCorrection;
	int calculateSleepTime(int distance, int stepNum);
	int calculateTurnSleepTime(int turnDegree, int stepNum);

public:
	MicroMouseDriver() : m_lVal(0), m_rVal(0), m_bStopCorrection(false){};
	~MicroMouseDriver() {};

	void setMotor(int lVal, int rVal)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_lVal = lVal;
		m_rVal = rVal;
	}

	void getMotor(int& lVal, int& rVal) const
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		lVal = m_lVal;
		rVal = m_rVal;
	}

	void adjust(int lDiff, int rDiff);
	void controlMotorPrecisely(int sleepTime);
	void stop();
	void driveNBlock(int N);
	void riverseNBlock(int N);
	void spinLeft();
	void spinRight();
	void inverse();
	void turnLeft();
	void turnRight();
	void slideMM(int distance);
	void driveMM(int distance);
	void turnNAngle(int angle);
};
