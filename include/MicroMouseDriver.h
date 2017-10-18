#pragma once

#include "Motor.h"

#define	STEP_ANG				0.9		    //���[�^��1�X�e�b�v�ӂ�ɉ�]����p�x
#define	STEP_SLOW				500			//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	STEP_MIDDLE				400		//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	STEP_FAST				1500		//1�b������Ƀ��[�^�[�ɗ^����X�e�b�v��
#define	PI						3.14159		//��
#define	WHEEL_RADIUS			47		    //�ԗւ̒��a(mm)
#define	DEGREE					360		    //360��
#define	BLOCK					180		    //1�u���b�N�̑傫��(mm)
#define	WHEEL_AXLE				95		    //�Ԏ��̒���(mm)

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