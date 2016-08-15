#include "Level.h"

using namespace Sexy;

Level::Level(void)
{
	for( int i=0; i<MAX_GRID_SIZE*MAX_GRID_SIZE; i++ )
	{
		mTiles[i] = -1;
		mMatchLevel[i] = 3;
		mBlock[i] = 0;
		//// TODO: Remove testing
		// if( Rand()%3 > 0 ) mTiles[1] = 0;
	}

	mGridSize = 10;
	ClearTiles();
}

Level::~Level(void)
{
}



void Level::ClearTiles(void)
{
	for( int i=0; i<mGridSize*mGridSize; i++ )
	{
		mTiles[i] = -1;
		mBlock[i] = 0;
		mMatchLevel[i] = 3;
	}
}


int Level::CountTiles(void)
{
	int tileNo;
	int tileCount = 0;

	for( tileNo = 0; tileNo < mGridSize * mGridSize; tileNo++ )
	{
		if( mBlock[tileNo] == 0 &&
			mTiles[tileNo] >= 0 &&
			mTiles[tileNo] <= 7 )
		{
			tileCount++;
		}
	}

	return tileCount;
}