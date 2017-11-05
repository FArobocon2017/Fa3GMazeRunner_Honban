#pragma once

#include "Maze.h"
#include "Operation.h"
#include "ThreadPool.h"

class Agent;

class MazeRunner
{
	//クラッシュした時のためのバックアップ
	Maze maze_backup;
	std::mutex m_mutex;
	int m_wall[3];

	void setWall(int* wall)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_wall[0] = wall[0];
		m_wall[1] = wall[1];
		m_wall[2] = wall[2];
	}
	void getWall(int* wall)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		wall[0] = m_wall[0];
		wall[1] = m_wall[1];
		wall[2] = m_wall[2];
	}

public:
	MazeRunner():m_wall(){};
	~MazeRunner(){};
	Direction setWallData(int* wall, Direction nowDir);
	IndexVec setRobotPos(IndexVec nowPos, Direction nextDir);
	bool calibration(); //!< キャリブレーションを実施
	void startMonitorCamera();
	void exploreMaze(Agent& agent); //!< 迷路探索実施
	void robotPositionInit();
	void robotMove(Direction nowDir, Direction nextDir);
	void robotMoveSequence(Operation runSequence);
	void adjustMove(int x, int y, int degTheta);
};
