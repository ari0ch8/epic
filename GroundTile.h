#pragma once
#include "gametile.h"

#include "../SexyAppFramework/SexyAppBase.h"

class GroundTile :
	public GameTile
{
public:
	GroundTile(void);
public:
	~GroundTile(void);

public:
	int		id;
};
