#include "../include/MazeRunner.h"
#include "../include/MicroMouseDriver.h"

// Int[3]→Direction
Direction MazeRunner::setWallData(int* wall, Direction nowDir)
{
	Direction dir;
	
	if(nowDir == NORTH)
	{
		if(wall[0])
		{
			dir |= WEST;
			dir |= DONE_WEST;
		}
		if(wall[1])
		{
			dir |= NORTH;
			dir |= DONE_NORTH;
		}
		if(wall[2])
		{
			dir |= EAST;
			dir |= DONE_EAST;
		}
	}
	else if(nowDir == EAST)
	{
		if(wall[0])
		{
			dir |= NORTH;
			dir |= DONE_NORTH;
		}
		if(wall[1])
		{
			dir |= EAST;
			dir |= DONE_EAST;
		}
		if(wall[2])
		{
			dir |= SOUTH;
			dir |= DONE_SOUTH;
		}
	}
	else if(nowDir == SOUTH)
	{
		if(wall[0])
		{
			dir |= EAST;
			dir |= DONE_EAST;
		}
		if(wall[1])
		{
			dir |= SOUTH;
			dir |= DONE_SOUTH;
		}
		if(wall[2])
		{
			dir |= WEST;
			dir |= DONE_WEST;
		}
	}
	else if(nowDir == WEST)
	{
		if(wall[0])
		{
			dir |= SOUTH;
			dir |= DONE_SOUTH;
		}
		if(wall[1])
		{
			dir |= WEST;
			dir |= DONE_WEST;
		}
		if(wall[2])
		{
			dir |= NORTH;
			dir |= DONE_NORTH;
		}
	}
	
	return dir;
}


IndexVec MazeRunner::setRobotPos(IndexVec nowPos, Direction nextDir)
{
	IndexVec pos;
	
	if(nextDir == NORTH)
	{
		pos  = nowPos + IndexVec::vecNorth;
	}
	else if(nextDir == EAST)
	{
		pos  = nowPos + IndexVec::vecEast;
	}
	else if(nextDir == SOUTH)
	{
		pos  = nowPos + IndexVec::vecSouth;
	}
	else if(nextDir == WEST)
	{
		pos  = nowPos + IndexVec::vecWest;
	}
	
	return pos;
}



void MazeRunner::robotMove(Direction nowDir, Direction nextDir)
{
	MicroMouseDriver microMouseDriver;

	if(nowDir.bits.North)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.turnLeft();
		}
	}
	else if(nowDir.bits.East)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
	}
	else if(nowDir.bits.South)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.turnRight();
		}
	}
	else if(nowDir.bits.West)
	{
		if(nextDir.bits.North)
		{
			microMouseDriver.turnRight();
		}
		else if(nextDir.bits.East)
		{
			microMouseDriver.inverse();
			microMouseDriver.driveNBlock(1);
		}
		else if(nextDir.bits.South)
		{
			microMouseDriver.turnLeft();
		}
		else if(nextDir.bits.West)
		{
			microMouseDriver.driveNBlock(1);
		}
	}
}

void MazeRunner::robotPositionInit()
{
	MicroMouseDriver microMouseDriver;
	microMouseDriver.inverse();
}