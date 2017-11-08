#pragma once

#include "Motor.h"

#define	STEP_ANG				0.9		//���[�^��1�X�e�b�v�ӂ�ɉ�]����p�x
#define	STEP_SLOW				200		//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	STEP_MIDDLE				400		//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	STEP_FAST				1500		//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	PI					    3.14159	//��
#define	WHEEL_RADIUS				47		//�ԗւ̒��a(mm)
#define	DEGREE					360		//360��
#define	BLOCK					180		//1�u���b�N�̑傫��(mm)
#define	WHEEL_AXLE				95		//�Ԏ��̒���(mm)
#define	RADIAN_TO_ANG				57.2958		
#define	ANG_TO_RADIAN				0.0174		
#define	HOSEI5					5		
#define	HOSEI10					10		
#define	HOSEI20					20		
#define	HOSEI30					30		
#define	HOSEI50					50	

class MicroMouseDriver
{
	Motor motor;
	mutable std::mutex m_mutex;
	int m_lVal, m_rVal;
	int calculateSleepTime(int distance, int stepNum);
	int calculateTurnSleepTime(int turnDegree, int stepNum);

public:
	MicroMouseDriver() : m_lVal(0), m_rVal(0){};
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