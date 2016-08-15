#pragma once

#include "actortile.h"
#include <string>

class Actor
{
public:
	Actor(void);
public:
	~Actor(void);

	int mapx;
	int mapy;

	int	mLife;

	int mStrength;
	int mBalance;
	int mIntellect;
	int mFaith;
	int mFocus;

	std::string mName;

	ActorTile*	mActorTile;
};
