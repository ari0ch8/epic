#include "TileManager.h"

TileManager::TileManager(void)
{
	mGroundTiles = NULL;
	mGroundTileCount = 0;

	mActorTiles = NULL;
	
}

TileManager::~TileManager(void)
{
	if( mGroundTiles != NULL )
	{
		delete[] mGroundTiles;
	}
	if( mActorTiles != NULL )
	{
		delete[] mActorTiles;
	}
}

void TileManager::Init(void)
{
	int x, y;

	mGroundTileCount = 12*12;
	mGroundTiles = new GroundTile[mGroundTileCount];

	mWallTileCount = 12*12;
	mWallTiles = new WallTile[mWallTileCount];

	for( y=0; y<12; y++ )
	{
		for( x=0; x<12; x++ )
		{
			mGroundTiles[(y*12)+x].imagecol = x;
			mGroundTiles[(y*12)+x].imagerow = y;
			mGroundTiles[(y*12)+x].id = (y*12)+x;

			mWallTiles[(y*12)+x].imagecol = x;
			mWallTiles[(y*12)+x].imagerow = y;
			mWallTiles[(y*12)+x].id = (y*12)+x;
		}
	}

	mActorTiles = new ActorTile[12*12];
	mActorTiles[1].imagecol = 1;
}
