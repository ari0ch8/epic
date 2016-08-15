#include "ActorManager.h"

#include "globals.h"

ActorManager::ActorManager(void)
{
	mActors = NULL;
}

ActorManager::~ActorManager(void)
{
	if( mActors != NULL )
	{
		delete[] mActors;
	}
}

void ActorManager::Init(void)
{
	mActors = new Actor[256];

	mActors[1].mActorTile = &gTileManager.mActorTiles[1];
	mActors[1].mName = "Guard";

	mActors[2].mActorTile = &gTileManager.mActorTiles[2];
	mActors[2].mName = "Guard";
}
