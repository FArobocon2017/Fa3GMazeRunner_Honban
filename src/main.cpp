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

//探索した迷路の壁情報がはいる
Maze maze;
//クラッシュした時のためのバックアップ
Maze maze_backup;

//探索の指示を出す
Agent agent(maze);
//前回のAgentの状態を保存しとく
Agent::State prevState = Agent::IDLE;

int main()
{
	WallDetector wallDetector;
	Direction nowDir(NORTH);
	IndexVec nowPos(0,0);
	MazeRunner mazeRunner;
	int wall[3] = {0};
	int prev_State = 0;
	// 迷路探索開始
	while(1)
	{
		//センサから取得した壁情報を入れる
		wallDetector.getWallData(wall);
		Direction wallData = mazeRunner.setWallData(wall, nowDir);
		
		//ロボットの座標を取得
		IndexVec robotPos = nowPos;
		
		//壁情報を更新 次に進むべき方向を計算
		agent.update(robotPos, wallData);


		cout << "robotPos" << static_cast<int16_t>(robotPos.x) << "," <<  static_cast<int16_t>(robotPos.y) << endl;
		cout << "wallData" << wallData[7] << "" << wallData[6]<< "" << wallData[5]<< "" << wallData[4]<< endl;
		cout << "agent.getState()" << agent.getState() << endl;

		//Agentの状態を確認
		//FINISHEDになったら計測走行にうつる
		if (agent.getState() == Agent::FINISHED)
		{
			break;
		}
		//ゴールにたどり着いた瞬間に一度だけmazeのバックアップをとる
		//Mazeクラスはoperator=が定義してあるからa = bでコピーできる
		if (prev_State == Agent::SEARCHING_NOT_GOAL && agent.getState() == Agent::SEARCHING_REACHED_GOAL) 
		{
			maze_backup = maze;
		}
		prev_State = agent.getState();
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
		mazeRunner.robotMove(nowDir, nextDir);  //robotMove関数はDirection型を受け取ってロボットをそっちに動かす関数
		
		nowPos = mazeRunner.setRobotPos(nowPos, nextDir);
		nowDir = nextDir;
usleep(100000);
	}
	
	//ロボットを停止させ、スタートする向きに戻す
	mazeRunner.robotPositionInit();
	
	/*
	//最短経路の計算 割と時間がかかる(数秒)
	//引数は斜め走行をするかしないか
	//trueだと斜め走行をする
	agent.calcRunSequence(false);
	
	// 計測走行
	//コマンドリストみたいなやつを取り出す
	const OperationList &runSequence = agent.getRunSequence();
	//Operationを先頭から順番に実行していく
	for (size_t i=0;i<runSequence.size();i++) 
	{
		//Operationの実行が終わるまで待つ(nマス進んだ,右に曲がった)
		while(!operationFinished());
		//i番目のを実行
		robotMove(runSequence[i]); //robotMode関数はOperation型を受け取ってそれを実行する関数
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