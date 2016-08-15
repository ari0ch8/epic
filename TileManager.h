#pragma once

#include "groundtile.h"
#include "actortile.h"
#include "walltile.h"

#include "res.h"

namespace Sexy
{

class TileManager
{
public:
	TileManager(void);
	~TileManager(void);

	void Init(void);

public:
	GroundTile*		mGroundTiles;
	int				mGroundTileCount;
	WallTile*		mWallTiles;
	int				mWallTileCount;
	ActorTile*		mActorTiles;

};

}