#pragma once
#include "gametile.h"

#include "../SexyAppFramework/SexyAppBase.h"

class WallTile :
	public GameTile
{
public:
	WallTile(void);
public:
	~WallTile(void);

	int id;
};
