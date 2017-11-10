#include <iostream>
#include <unistd.h>
#include <chrono>
#include <opencv2/opencv.hpp>

#include "../include/MazeRunner.h"
#include "../include/MicroMouseDriver.h"
#include "../include/TactSW.h"
#include "../include/Agent.h"
#include "../include/WallDetector.h"

using namespace std;

namespace{
	// これどこか他の場所に定義してほしい
	// Int[3]→Direction
	Direction determineDirection(int* wall, Direction nowDir)
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
}


bool MazeRunner::calibration()
{
	// タクトスイッチ
	TactSW tactsw;
	int tswSts0 = 1;
	int tswSts1 = 1;
	int tswSts2 = 1;
	
	int cnt =0;
	while(tswSts2)
	{
		tswSts0 = tactsw.getTactSwSts0();
		tswSts1 = tactsw.getTactSwSts1();
		tswSts2 = tactsw.getTactSwSts2();
		
		if(tswSts0 == 0)
		{
			cout << "create" << endl;
			cnt++;
		}
		if(tswSts1 == 0)
		{
			return false;
		}
	}

	return true;
}


void MazeRunner::startMonitorCamera()
{
	TactSW tactsw;
	int tswSts0 = 1;
	WallDetector wallDetector;
	RaspiCam raspiCam;
	raspiCam.open();
	raspiCam.createWindow();
	
	// タクトスイッチ1が0になるまで1秒間隔で画像を撮り続ける
	while(tswSts0!=0)
	{
		// 壁情報（左,前,右）
		int wall[3] = {0};
		EstimatedErrors mouseErr{0,0,0};
		// 
		tswSts0 = tactsw.getTactSwSts0();
		
		if(this->getCameraPermission())
		{
			//cout << "getWallData" << endl;
			raspiCam.capture();
			raspiCam.getSideWallData(wall, &mouseErr);
			wallDetector.getWallData(wall);
			if(wall[1] == 1)
			{
				raspiCam.getFrontWallData(wall, &mouseErr);
			}
			this->setWall(wall, mouseErr);
			

			//raspiCam.showImg();

			// DEBUG
			//cout << "mouseErr.x:" << mouseErr.x <<endl;
			//cout << "mouseErr.y:" << mouseErr.y <<endl;
			//cout << "mouseErr.degree:" << mouseErr.degree <<endl;
			if(mouseErr.x >10)
			{
				m_microMouseDriver.adjust(HOSEI10, 0);
			}
			else if(mouseErr.x <-10)
			{
				m_microMouseDriver.adjust(0, HOSEI10);
			}
			
			// WAIT
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}
}


// For Debug
void MazeRunner::dbg()
{
	EstimatedErrors mouseErr;
	
	
	for(int i=0; i<20; i++)
	{
		// 壁情報（左,前,右）
		int wall[3] = {0};
	
		// 誤差情報の取得（カメラ機能使用）
		this->getWall(wall, &mouseErr);
		
		// disp
		cout << "main thread:" << i << endl;
		cout << "wall:[" << wall [0] << ", " << wall [1] << ", "<< wall [2]<< "]" << endl;
		cout << "mouseErr:" << mouseErr.x << ", " << mouseErr.y << ", " << mouseErr.degree << endl;
		usleep(1000000);
		
		/*
		// Light Sensor Proc....
		this->setCameraPermission(false);
		// proc
		for(int j=0; j<3; j++)
		{
			cout << "Light Sensor Proc:" << j << endl;
			usleep(500000);
		}
		//
		this->setCameraPermission(true);
		*/
	}
}

void MazeRunner::exploreMaze(Agent& agent)
{
	// メインループ数
	int loopNum = 0;

	// マウスの現在位置（X,Y）
	IndexVec nowPos(0,0);
	
	// マウスの現在の進行方向
	Direction nowDir(NORTH);

	//前回のAgentの状態を保存しとく
	Agent::State prevState = Agent::IDLE;

	// マス原点からの誤差情報
	int x, y, theta = { 0 };

	while(1)
	{
		cout << "loopNum:" << loopNum << endl;
		
		// 壁情報（左,前,右）
		int wall[3] = {0};
		EstimatedErrors mouseErr = {0,0,0};

		// 誤差情報の取得（カメラ機能使用）
		this->getWall(wall, &mouseErr);


		// 位置補正
		if(loopNum > 0){
			cout << "mouseErr:" << 
			 mouseErr.x << ", " <<
			 mouseErr.y << ", " <<
			 mouseErr.degree << endl;

			//this->adjustMove(mouseErr.x, mouseErr.y, mouseErr.degree);
		}
		
		// 壁情報変換（左前右　→　東西南北）
		Direction wallData = ::determineDirection(wall, nowDir);
		
		//ロボットの座標を取得
		IndexVec robotPos = nowPos;
		
		//壁情報を更新 次に進むべき方向を計算
		agent.update(robotPos, wallData);

		// デバッグ用
		cout << "robotPos[x,y]:" << static_cast<int16_t>(robotPos.x) << "," <<  static_cast<int16_t>(robotPos.y) << endl;
		cout << "wall[L,F,R]:"
			<< wall[0] << " " 
			<< wall[1] << " " 
			<< wall[2] << endl;
		cout << "wallData[N,E,S,W]:" 
			<< static_cast<int16_t>(wallData.bits.North) << " " 
			<< static_cast<int16_t>(wallData.bits.East) << " " 
			<< static_cast<int16_t>(wallData.bits.South) << " " 
			<< static_cast<int16_t>(wallData.bits.West) << endl;
		cout << "agent.getState()" << agent.getState() << endl;

		//Agentの状態を確認
		//FINISHEDになったら計測走行にうつる
		if (agent.getState() == Agent::FINISHED)
		{
			break;
		}
		//ゴールにたどり着いた瞬間に一度だけmazeのバックアップをとる
		//Mazeクラスはoperator=が定義してあるからa = bでコピーできる
		if (prevState == Agent::SEARCHING_NOT_GOAL && agent.getState() == Agent::SEARCHING_REACHED_GOAL) 
		{
			maze_backup = agent.getCurrentMaze();
		}
		prevState = agent.getState();
		//一度はゴールにたどり着き、少なくともゴールできる状態で追加の探索をしているが、
		//もう時間が無いから探索をうちやめてスタート地点に戻る
		//if (isTimeOut() && agent.getState() == Agent::SEARCHING_REACHED_GOAL)
		if (agent.getState() == Agent::SEARCHING_REACHED_GOAL)
		{
			agent.forceGotoStart();
		}

		//Agentの状態が探索中の場合は次に進むべき方向を取得する
		Direction nextDir = agent.getNextDirection();
		//nextDirの示す方向に進む
		//突然今と180度逆の方向を示してくる場合もあるので注意
		//止まらないと壁にぶつかる
		this->robotMove(nowDir, nextDir);  //robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		nowPos = this->setRobotPos(nowPos, nextDir);
		nowDir = nextDir;
		//usleep(100000);
		
		cout << "nextDir[N,E,S,W]:" 
			<< static_cast<int16_t>(nextDir.bits.North) << " " 
			<< static_cast<int16_t>(nextDir.bits.East) << " " 
			<< static_cast<int16_t>(nextDir.bits.South) << " " 
			<< static_cast<int16_t>(nextDir.bits.West) << endl;

		cout << endl;
		loopNum++;
	}
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
	m_microMouseDriver.inverse();
}

void MazeRunner::robotMove(Direction nowDir, Direction nextDir)
{
	if(nowDir.bits.North)
	{
		if(nextDir.bits.North)
		{
			cout << "driveN" << endl;
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			cout << "turnR" << endl;
			m_microMouseDriver.turnRight();
		}
		else if(nextDir.bits.South)
		{
			cout << "inv" << endl;
			m_microMouseDriver.inverse();
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			cout << "turnL" << endl;
			m_microMouseDriver.turnLeft();
		}
	}
	else if(nowDir.bits.East)
	{
		if(nextDir.bits.North)
		{
			m_microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.East)
		{
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			m_microMouseDriver.turnRight();
		}
		else if(nextDir.bits.West)
		{
			m_microMouseDriver.inverse();
			m_microMouseDriver.driveNBlock(1);
		}
	}
	else if(nowDir.bits.South)
	{
		if(nextDir.bits.North)
		{
			m_microMouseDriver.inverse();
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			m_microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.South)
		{
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			m_microMouseDriver.turnRight();
		}
	}
	else if(nowDir.bits.West)
	{
		if(nextDir.bits.North)
		{
			m_microMouseDriver.turnRight();
		}
		else if(nextDir.bits.East)
		{
			m_microMouseDriver.inverse();
			m_microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			m_microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.West)
		{
			m_microMouseDriver.driveNBlock(1);
		}
	}
}


void MazeRunner::robotMoveSequence(Operation runSequence)
{
	/*
	cout << "runSequence.size" << runSequence.size()<< endl;
	int opSize =　runSequence.size();
	// opIdx番目のオペレーションを実行
	for(int opIdx=0; opIdx<opSize; opIdx++)
	{
		cout << "runSequence[" << opIdx <<"]" << runSequence[opIdx].op << ","<< static_cast<int16_t>(runSequence[opIdx].n) << endl;
		
		int N = static_cast<int16_t>(runSequence[opIdx].n);
		switch(runSequence[opIdx].op)
		{
			// 直進
			case Operation::OperationType::FORWARD:
				m_microMouseDriver.driveNBlock(N);
				break;
			// 斜めに直進(普通の直進とは進む距離が違う)
			case Operation::OperationType::FORWARD_DIAG:
				break;
			// 右に90度旋回
			case Operation::OperationType::TURN_RIGHT90:
				for(int i=0;i<N;i++)
				{
					m_microMouseDriver.spinRight();
				}
				break;
			// 右に45度旋回
			case Operation::OperationType::TURN_RIGHT45:
				break;
			// 左に90度旋回
			case Operation::OperationType::TURN_LEFT90:
				for(int i=0;i<N;i++)
				{
					m_microMouseDriver.spinLeft();
				}
				break;
			// 	左に45度旋回
			case Operation::OperationType::TURN_LEFT45:
				break;
			// 停止
			case Operation::OperationType::STOP:
				m_microMouseDriver.stop();
				break;
			default:
				break;
		}
	}
	*/
}


/// 必要な定数 ///

// 許容範囲内
int xTolerance = 10;
int yTolerance = 10;
int thetaTolerance = 5;

void MazeRunner::adjustMove(int x, int y, int degTheta)
{
	// 変数定義
	MicroMouseDriver mRunner; // ドライバ
	
	// 角度をラジアンに変換
	//double radTheta = (degTheta * PI) / 180.0;
	double radTheta = degTheta;
	
	// 分岐（x,y,degThetaが許容範囲内)
	if (abs(x) < xTolerance && abs(y) < yTolerance && abs(degTheta) < thetaTolerance)
	{
		return;	 // 何もしない
	}
	
	///*

	// 他のパターン（前に詰まりそうで、後ろ側に空間があるパタン）
	/// ①後ろ方向に移動
	double moveBack = (double)x / cos(radTheta); // 移動距離
	mRunner.driveMM(-moveBack); // moveBack(mm)分だけ後ろに下がる

	/// ②回転
	mRunner.turnNAngle(-degTheta); // degTheta分だけ逆回転（実装済）

    /// ③前進
	double moveForward = (double)y - (moveBack * cos(radTheta));
	mRunner.driveMM(moveForward);  // moveForward(mm)分だけ前に進む

//*/

	return; // 不要
}
