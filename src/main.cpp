#include <iostream>
#include <vector>
#include <unistd.h>

#include "../include/Maze.h"
#include "../include/Agent.h"
#include "../include/MazeData.h"
#include "../include/MazeRunner.h"

#include "../include/WallDetector.h"
#include "../include/MicroMouseDriver.h"

using namespace std;

//探索した迷路の壁情報がはいる
Maze maze;
//探索の指示を出す
Agent agent(maze);

int main()
{
	// 壁情報（東西南北）
	WallDetector wallDetector;

	// 補正走行
	int x, y, theta;
	adjustMove(x, y, theta);
		
		// マウスの現在の進行方向
	Direction nowDir(NORTH);
	// マウスの現在位置（X,Y）
	IndexVec nowPos(0,0);
	// 座標変換用
	MazeRunner mazeRunner;
	
	// キャリブレーション実施
	mazeRunner.calibration();

	// 迷路探索開始
	usleep(2000000);
	mazeRunner.exploreMaze(agent);
	
	//ロボットを停止させ、スタートする向きに戻す
	mazeRunner.robotPositionInit();
	usleep(3000000);
	
	//最短経路の計算 割と時間がかかる(数秒)
	//引数は斜め走行をするかしないか
	//trueだと斜め走行をする
	agent.calcRunSequence(false);
	
	// 計測走行
	cout << "Opelation" << endl;
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
	// 壁情報（左,前,右）
	int wall[3] = {0};
	// センサ機能確認用
	for(int i=0;i<10;i++)
	{
		cout << "i:" << i << endl;
		wallDetector.getWallData(wall);
		cout << "wall:" << "[" << wall[0] << "," << wall[1] << ","<< wall[2] << "]"<< endl;
		usleep(1000000);
	}
	return 0;
}



/// 必要な定数 ///

// 許容範囲内
int xTolerance = 10;
int yTolerance = 10;
int thetaTolerance = 5;


// マス内での補正を実施する
void adjustMove(int x, int y, int degTheta)
{
	// 変数定義
	MicroMouseDriver mRunner; // ドライバ
						
	// 角度をラジアンに変換
	double radTheta = (degTheta * PI) / 180.0;
	
	// 分岐（x,y,degThetaが許容範囲内)
	if(abs(x) < xTolerance && abs(y) < yTolerance && abs(degTheta) < thetaTolerance)
	{
		return;	 // 何もしない
	}
	
	// 分岐(X方向ずれあり、角度ずれなし)
	if (abs(x) > xTolerance && abs(degTheta) < thetaTolerance)
	{
		mRunner.slideMM(-x); // 車線変更
		return;
	}

	// 他のパターン（前に詰まりそうで、後ろ側に空間があるパタン）
	/// ①後ろ方向に移動
	double moveBack = (double)x / cos(radTheta); // 移動距離
	mRunner.driveMM(-moveBack); // moveBack(mm)分だけ後ろに下がる

	/// ②回転
	mRunner.turnNAngle(-degTheta); // degTheta分だけ逆回転（実装済）
	
	/// ③前進
	double moveForward = (double)y - (moveBack * cos(radTheta));
	mRunner.driveMM(moveForward);  // moveForward(cm? mm?)分だけ前に進む

	return; // 不要
}