#pragma once

#include "Maze.h"

class MazeRunner
{
	public:
	MazeRunner(){};
	~MazeRunner(){};
	Direction setWallData(int* wall, Direction nowDir);
	IndexVec setRobotPos(IndexVec nowPos, Direction nextDir);
	void robotMove(Direction nowDir, Direction nextDir);
	void robotPositionInit();
};
