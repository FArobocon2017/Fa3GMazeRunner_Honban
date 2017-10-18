#include <iostream>
#include <vector>
#include <unistd.h>
#include "../include/Maze.h"
#include "../include/Agent.h"
#include "../include/MazeData.h"
#include "../include/WallDetector.h"
#include "../include/MicroMouseDriver.h"
#include "../include/MazeRunner.h"

using namespace std;

//�T���������H�̕Ǐ�񂪂͂���
Maze maze;
//�N���b�V���������̂��߂̃o�b�N�A�b�v
Maze maze_backup;

//�T���̎w�����o��
Agent agent(maze);
//�O���Agent�̏�Ԃ�ۑ����Ƃ�
Agent::State prevState = Agent::IDLE;

int main()
{
	WallDetector wallDetector;
	Direction nowDir(NORTH);
	IndexVec nowPos(0,0);
	MazeRunner mazeRunner;
	int wall[3] = {0};
	int prev_State = 0;
	// ���H�T���J�n
	while(1)
	{
		//�Z���T����擾�����Ǐ�������
		wallDetector.getWallData(wall);
		Direction wallData = mazeRunner.setWallData(wall, nowDir);
		
		//���{�b�g�̍��W���擾
		IndexVec robotPos = nowPos;
		
		//�Ǐ����X�V ���ɐi�ނׂ��������v�Z
		agent.update(robotPos, wallData);


		cout << "robotPos" << static_cast<int16_t>(robotPos.x) << "," <<  static_cast<int16_t>(robotPos.y) << endl;
		cout << "wallData" << wallData[7] << "" << wallData[6]<< "" << wallData[5]<< "" << wallData[4]<< endl;
		cout << "agent.getState()" << agent.getState() << endl;

		//Agent�̏�Ԃ��m�F
		//FINISHED�ɂȂ�����v�����s�ɂ���
		if (agent.getState() == Agent::FINISHED)
		{
			break;
		}
		//�S�[���ɂ��ǂ蒅�����u�ԂɈ�x����maze�̃o�b�N�A�b�v���Ƃ�
		//Maze�N���X��operator=����`���Ă��邩��a = b�ŃR�s�[�ł���
		if (prev_State == Agent::SEARCHING_NOT_GOAL && agent.getState() == Agent::SEARCHING_REACHED_GOAL) 
		{
			maze_backup = maze;
		}
		prev_State = agent.getState();
		//��x�̓S�[���ɂ��ǂ蒅���A���Ȃ��Ƃ��S�[���ł����ԂŒǉ��̒T�������Ă��邪�A
		//�������Ԃ���������T����������߂ăX�^�[�g�n�_�ɖ߂�
		//if (isTimeOut() && agent.getState() == Agent::SEARCHING_REACHED_GOAL)
		if (agent.getState() == Agent::SEARCHING_REACHED_GOAL)
		{
			agent.forceGotoStart();
		}
		//Agent�̏�Ԃ��T�����̏ꍇ�͎��ɐi�ނׂ��������擾����
		Direction nextDir = agent.getNextDirection();
		//nextDir�̎��������ɐi��
		//�ˑR����180�x�t�̕����������Ă���ꍇ������̂Œ���
		//�~�܂�Ȃ��ƕǂɂԂ���
		mazeRunner.robotMove(nowDir, nextDir);  //robotMove�֐���Direction�^���󂯎���ă��{�b�g���������ɓ������֐�
		
		nowPos = mazeRunner.setRobotPos(nowPos, nextDir);
		nowDir = nextDir;
usleep(100000);
	}
	
	//���{�b�g���~�����A�X�^�[�g��������ɖ߂�
	mazeRunner.robotPositionInit();
	
	/*
	//�ŒZ�o�H�̌v�Z ���Ǝ��Ԃ�������(���b)
	//�����͎΂ߑ��s�����邩���Ȃ���
	//true���Ǝ΂ߑ��s������
	agent.calcRunSequence(false);
	
	// �v�����s
	//�R�}���h���X�g�݂����Ȃ�����o��
	const OperationList &runSequence = agent.getRunSequence();
	//Operation��擪���珇�ԂɎ��s���Ă���
	for (size_t i=0;i<runSequence.size();i++) 
	{
		//Operation�̎��s���I���܂ő҂�(n�}�X�i��,�E�ɋȂ�����)
		while(!operationFinished());
		//i�Ԗڂ̂����s
		robotMove(runSequence[i]); //robotMode�֐���Operation�^���󂯎���Ă�������s����֐�
	}
	*/
	//fin
	
	
	for(int i=0;i<100;i++)
	{
		cout << "i:" << i << endl;
		wallDetector.getWallData(wall);
		cout << "wall:" << "[" << wall[0] << "," << wall[1] << ","<< wall[2] << "]"<< endl;
		usleep(100000);
	}
	return 0;
}