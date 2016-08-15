#include "Actor.h"

#include "globals.h"

Actor::Actor(void)
{
	mName = "";

	mapx = 0;
	mapy = 0;

	mLife = 0;

	mActorTile = &gTileManager.mActorTiles[0];

}

Actor::~Actor(void)
{
}
