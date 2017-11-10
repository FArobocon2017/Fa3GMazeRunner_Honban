#include <iostream>
#include <vector>
#include <unistd.h>

#include "../include/Maze.h"
#include "../include/Agent.h"
#include "../include/MazeData.h"
#include "../include/MazeRunner.h"
#include "../include/ThreadPool.h"

#include "../include/WallDetector.h"
#include "../include/MicroMouseDriver.h"

using namespace std;

//探索した迷路の壁情報がはいる
Maze maze;
//探索の指示を出す
Agent agent(maze);

int main()
{
	cout << "--Start Maze Runner--" << endl;
	
	// 壁情報（東西南北）
	WallDetector wallDetector;
		
	// マウスの現在の進行方向
	Direction nowDir(NORTH);
	
	// マウスの現在位置（X,Y）
	IndexVec nowPos(0,0);
	
	// 座標変換用
	MazeRunner mazeRunner;
	
	// キャリブレーション実施
	if(!mazeRunner.calibration())
	{
		return 0;
	}
	
	// カメラ監視スレッド作成
	cout << "--Start Camera Thread--" << endl;
	ThreadPool pool(1);
	auto ftr = pool.enqueue([&mazeRunner](){
		mazeRunner.startMonitorCamera();
	});
	
	/*
	// dbg
	mazeRunner.dbg();
	ftr.wait();
	return 0;
	//*/
	
	// 迷路探索開始
	usleep(2000000);
	cout << "--Start Search Process--" << endl;
	mazeRunner.exploreMaze(agent);
	
	//ロボットを停止させ、スタートする向きに戻す
	mazeRunner.robotPositionInit();
	cout << "" << endl;
	cout << "--End Search Process--" << endl;
	usleep(3000000);
	
	//最短経路の計算 割と時間がかかる(数秒)
	//引数は斜め走行をするかしないか
	//trueだと斜め走行をする
	cout << "" << endl;
	cout << "--Calculate OperationList--" << endl;
	agent.calcRunSequence(false);

	//コマンドリストみたいなやつを取り出す
	cout << "" << endl;
	cout << "--Run Sequence--" << endl;
	const OperationList &runSequence = agent.getRunSequence();
	//mazeRunner.robotMoveSequence(runSequence); //robotMode関数はOperation型を受け取ってそれを実行する関数

	ftr.wait();
	
	//fin
	cout << "--Finished--" << endl;

	return 0;
}
