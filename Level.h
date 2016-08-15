#pragma once

#include "globals.h"


namespace Sexy
{

class Level
{
public:
	Level(void);
public:
	~Level(void);

public:
	int mGridSize;

	char mTiles[MAX_GRID_SIZE*MAX_GRID_SIZE];
	char mBlock[MAX_GRID_SIZE*MAX_GRID_SIZE];
	char mMatchLevel[MAX_GRID_SIZE*MAX_GRID_SIZE];


	void ClearTiles(void);
	int Level::CountTiles(void);

};

}