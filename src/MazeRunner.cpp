#include <iostream>
#include "../include/MazeRunner.h"
#include "../include/MicroMouseDriver.h"

using namespace std;

// Int[3]→Direction
Direction MazeRunner::setWallData(int* wall, Direction nowDir)
{
	Direction dir;
	
	if(nowDir.bits.North)
	{
		if(wall[0])
		{
			dir.bits.West = 1;
			dir.bits.DoneWest = 1;
		}
		if(wall[1])
		{
			dir.bits.North = 1;
			dir.bits.DoneNorth = 1;
		}
		if(wall[2])
		{
			dir.bits.East = 1;
			dir.bits.DoneEast = 1;
		}
	}
	else if(nowDir.bits.East)
	{
		if(wall[0])
		{
			dir.bits.North = 1;
			dir.bits.DoneNorth = 1;
		}
		if(wall[1])
		{
			dir.bits.East = 1;
			dir.bits.DoneEast = 1;
		}
		if(wall[2])
		{
			dir.bits.South = 1;
			dir.bits.DoneSouth = 1;
		}
	}
	else if(nowDir.bits.South)
	{
		if(wall[0])
		{
			dir.bits.East = 1;
			dir.bits.DoneEast = 1;
		}
		if(wall[1])
		{
			dir.bits.South = 1;
			dir.bits.DoneSouth = 1;
		}
		if(wall[2])
		{
			dir.bits.West = 1;
			dir.bits.DoneWest = 1;
		}
	}
	else if(nowDir.bits.West)
	{
		if(wall[0])
		{
			dir.bits.South = 1;
			dir.bits.DoneSouth = 1;
		}
		if(wall[1])
		{
			dir.bits.West = 1;
			dir.bits.DoneWest = 1;
		}
		if(wall[2])
		{
			dir.bits.North = 1;
			dir.bits.DoneNorth = 1;
		}
	}
	
	return dir;
}


IndexVec MazeRunner::setRobotPos(IndexVec nowPos, Direction nextDir)
{
	IndexVec pos;
	
	if(nextDir.bits.North)
	{
		pos  = nowPos + IndexVec::vecNorth;
	}
	else if(nextDir.bits.East)
	{
		pos  = nowPos + IndexVec::vecEast;
	}
	else if(nextDir.bits.South)
	{
		pos  = nowPos + IndexVec::vecSouth;
	}
	else if(nextDir.bits.West)
	{
		pos  = nowPos + IndexVec::vecWest;
	}
	
	return pos;
}


void MazeRunner::robotPositionInit()
{
	MicroMouseDriver microMouseDriver;
	microMouseDriver.inverse();
}

void MazeRunner::robotMove(Direction nowDir, Direction nextDir)
{
	MicroMouseDriver microMouseDriver;

	if(nowDir.bits.North)
	{
		if(nextDir.bits.North)
		{
			cout << "driveN" << endl;
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			cout << "turnR" << endl;
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.South)
		{
			cout << "inv" << endl;
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			cout << "turnL" << endl;
			microMouseDriver.turnLeft();
		}
	}
	else if(nowDir.bits.East)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
	}
	else if(nowDir.bits.South)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.turnRight();
		}
	}
	else if(nowDir.bits.West)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.driveNBlock(1);
		}
	}
}


void MazeRunner::robotMoveSequence(Operation runSequence)
{
	MicroMouseDriver microMouseDriver;
	int N = static_cast<int16_t>(runSequence.n);

	switch(runSequence.op)
	{
		// 直進
		case Operation::OperationType::FORWARD:
			microMouseDriver.driveNBlock(N);
			break;
		// 斜めに直進(普通の直進とは進む距離が違う)
		case Operation::OperationType::FORWARD_DIAG:
			break;
		// 右に90度旋回
		case Operation::OperationType::TURN_RIGHT90:
			for(int i=0;i<N;i++)
			{
				microMouseDriver.spinRight();
			}
			break;
		// 右に45度旋回
		case Operation::OperationType::TURN_RIGHT45:
			break;
		// 左に90度旋回
		case Operation::OperationType::TURN_LEFT90:
			for(int i=0;i<N;i++)
			{
				microMouseDriver.spinLeft();
			}
			break;
		// 	左に45度旋回
		case Operation::OperationType::TURN_LEFT45:
			break;
		// 停止
		case Operation::OperationType::STOP:
			microMouseDriver.stop();
			break;
		default:
			break;
	}
}