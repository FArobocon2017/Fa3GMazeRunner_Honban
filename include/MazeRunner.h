#pragma once

#include "Maze.h"
#include "Operation.h"

class Agent;

class MazeRunner
{
	//クラッシュした時のためのバックアップ
	Maze maze_backup;
	public:
	MazeRunner(){};
	~MazeRunner(){};
	Direction setWallData(int* wall, Direction nowDir);
	IndexVec setRobotPos(IndexVec nowPos, Direction nextDir);
	void calibration(); //!< キャリブレーションを実施
	void exploreMaze(Agent& agent); //!< 迷路探索実施
	void robotPositionInit();
	void robotMove(Direction nowDir, Direction nextDir);
	void robotMoveSequence(Operation runSequence);
	voud adjustMove(int x, int y, int degTheta);
};
