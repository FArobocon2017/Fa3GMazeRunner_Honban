#pragma once

/***************************************
 * 迷路に関するパラメータ
 ***************************************/
//迷路の大きさ
#define MAZE_SIZE 16	//16

//迷路のゴール座標
//ゴールとしたい座標を並べる
//4個でなくてもよい
#define MAZE_GOAL_LIST {IndexVec(3,7),IndexVec(3,8),IndexVec(4,7),IndexVec(4,8)}
//#define MAZE_GOAL_LIST {IndexVec(4,7)}
//#define MAZE_GOAL_LIST {IndexVec(0,5)}


/****************************************
 * 探索アルゴリズムに関するパラメータ
 ****************************************/
//一旦ゴールに到達したあとのk最短経路を計算するときのk
#define SEARCH_DEPTH1 1

//探索が終了し、最終的な走行ルートを計算するときのk
#define SEARCH_DEPTH2 20


//経路のコストを計算するときに使うロボットの走行性能
//90度曲がるブロックを進むのにかかる時間[s]
#define TURN90_TIME 	0.5//0.3

//45度曲がるブロックを進むのにかかる時間[s]
#define TURN45_TIME 	0.5//0.2

//加速度[m/s^2]
#define ACCELERATION 	6.0

//最大速度[m/s]
#define MAX_VELOCITY 	2.0

//最小速度[m/s]
#define MIN_VELOCITY 	0.5

//1区画の長さ[m]
#define MAZE_1BLOCK_LENGTH 	0.18

