#pragma once
#include <memory>
#include "Maze.h"
#include "Operation.h"
#include "RaspiCam.h"
#include "ThreadPool.h"
#include "MicroMouseDriver.h"

class Agent;

class MazeRunner
{
	//クラッシュした時のためのバックアップ
	Maze maze_backup;
	// For ThreadPool
	std::mutex m_mutex;
	bool m_cameraPermission = true;
	int m_wall[3];
	EstimatedErrors m_errors;
	MicroMouseDriver m_microMouseDriver;

	void setWall(int* wall, EstimatedErrors errors)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_wall[0] = wall[0];
		m_wall[1] = wall[1];
		m_wall[2] = wall[2];
		m_errors.x = errors.x;
		m_errors.y = errors.y;
		m_errors.degree = errors.degree;
	}
	
	void getWall(int* wall, EstimatedErrors* errors)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		wall[0] = m_wall[0];
		wall[1] = m_wall[1];
		wall[2] = m_wall[2];
		errors->x = m_errors.x;
		errors->y = m_errors.y;
		errors->degree = m_errors.degree;
	}
	

	void setCameraPermission(bool cameraPermission)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cameraPermission = cameraPermission;
	}

	bool getCameraPermission()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		return m_cameraPermission;
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
	void dbg();
};
