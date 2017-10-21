#include <iostream>
#include <vector>
#include <unistd.h>

#include "../include/Maze.h"
#include "../include/Agent.h"
#include "../include/MazeData.h"
#include "../include/MazeRunner.h"

#include "../include/TactSW.h"
#include "../include/WallDetector.h"
#include "../include/MicroMouseDriver.h"

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
	int loopNum = 0;

	TactSW tactsw;
	int tswSts1 = 1;
	int tswSts2 = 1;
	int tswSts3 = 1;
	while(tswSts3)
	{
		tswSts1 = tactsw.getTactSwSts1();
		tswSts2 = tactsw.getTactSwSts2();
		tswSts3 = tactsw.getTactSwSts3();
		//cout << "tswSts1" << tswSts1 << endl;
		//cout << "tswSts2 " << tswSts2 << endl;
		if(tswSts1 == 0)
		{
			usleep(1000000);
			wallDetector.getLightAverage();
		}
		if(tswSts2 == 0)
		{
			return 0;
		}
	}
	
	
	// 迷路探索開始
	usleep(1000000);
	while(1)
	{
		cout << "loopNum:" << loopNum << endl;
		
		//センサから取得した壁情報を入れる
		wallDetector.getWallData(wall);
		Direction wallData = mazeRunner.setWallData(wall, nowDir);
		
		//ロボットの座標を取得
		IndexVec robotPos = nowPos;
		
		//壁情報を更新 次に進むべき方向を計算
		agent.update(robotPos, wallData);


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
		//usleep(100000);
		
		cout << "nextDir[N,E,S,W]:" 
			<< static_cast<int16_t>(nextDir.bits.North) << " " 
			<< static_cast<int16_t>(nextDir.bits.East) << " " 
			<< static_cast<int16_t>(nextDir.bits.South) << " " 
			<< static_cast<int16_t>(nextDir.bits.West) << endl;

		cout << endl;
		loopNum++;
	}
	
	//ロボットを停止させ、スタートする向きに戻す
	mazeRunner.robotPositionInit();
	usleep(3000000);
	
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
		//while(!operationFinished());

		//i番目のを実行
		cout << "runSequence.size" << runSequence.size() << endl;
		cout << "runSequence[" << i <<"]" << runSequence[i].op << ","<< static_cast<int16_t>(runSequence[i].n) << endl;
		mazeRunner.robotMoveSequence(runSequence[i]); //robotMode関数はOperation型を受け取ってそれを実行する関数
	}
	
	//fin
	
	
	for(int i=0;i<10;i++)
	{
		cout << "i:" << i << endl;
		wallDetector.getWallData(wall);
		cout << "wall:" << "[" << wall[0] << "," << wall[1] << ","<< wall[2] << "]"<< endl;
		usleep(100000);
	}
	return 0;
}