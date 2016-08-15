#pragma once

#include "floater.h"
#include "loadingprogress.h"
#include "floatingnumber.h"
#include "tile.h"

#include "gametile.h"
#include "tilemanager.h"
#include "actormanager.h"
#include "map.h"
#include "gameproperties.h"
#include "eventmanager.h"

extern int		gPauseLevel;
extern float	gGraphicsScale;

extern Sexy::Floater			gTitleLogo;
extern Sexy::LoadingProgress	gLoadingProgress;

extern Sexy::FloatingNumber floatingnumber[100];

extern Sexy::Tile*			floatingtile[100];

extern TileManager			gTileManager;
extern ActorManager			gActorManager;
extern Map*					gCurrentMap;
extern GameProperties		gGameProperties;
extern EventManager			gEventManager;

#define MATCHBLOX_ASSERT

#ifdef MATCHBLOX_ASSERT
#define Assert(x,y) if(!(x)) { MessageBox(NULL,y,y,MB_OK); }
extern char debug[1024];
#endif


#define		__EPICEDITOR


#define		MAX_GRID_SIZE		20
#define		MAX_BOARD_TILES		(MAX_GRID_SIZE*MAX_GRID_SIZE)

#define		MAX_BACKGROUND_TILES		32

namespace Sexy
{
	// old mb2 directions!!!
	enum Direction
	{
		Up,
		Right,
		Down,
		Left,
		None
	};


	void Split(const std::string& str, const std::string& delim, std::vector<std::string>& output);
	void convert( std::string &in, std::wstring &out );

	bool IsArmadilloDefaultTrial(void);
	bool IsArmadillo5MinRepeatingTrial(void);
	bool IsArmadilloWrapperInstalled(void);
	int UpdateArmadilloEnvironment(void);
	int InstallArmadilloCode(const char *name, const char *codestring);

	int SetArmadilloDefaultKey(void);

}




