#pragma once

#include "../SexyAppFramework/SexyAppBase.h"

#include "res.h"

using namespace Sexy;

class GameTile
{
public:
	GameTile(void);
public:
	~GameTile(void);

public:
	int		id;

	Image*	image;
	int		imagerow;
	int		imagecol;
};
