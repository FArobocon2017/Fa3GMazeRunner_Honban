#pragma once

#include "Maze.h"
#include "Operation.h"

class MazeRunner
{
	public:
	MazeRunner(){};
	~MazeRunner(){};
	Direction setWallData(int* wall, Direction nowDir);
	IndexVec setRobotPos(IndexVec nowPos, Direction nextDir);
	void robotPositionInit();
	void robotMove(Direction nowDir, Direction nextDir);
	void robotMoveSequence(Operation runSequence);
};
