#pragma once

#include "tilemanager.h"
#include "groundtile.h"
#include "actor.h"

namespace Sexy
{

class Map
{
private:

	static const int DistanceLookup[5][5];


public:
	Map(void);
	~Map(void);


	enum Dir
	{
		North,
		Northeast,
		East,
		Southeast,
		South,
		Southwest,
		West,
		Northwest
	};


	char* GetDirectionAsText(Dir theDirection)
	{
		switch( theDirection )
		{
		case North:
			return "North";
		case Northeast:
			return "Northeast";
		case East:
			return "East";
		case Southeast:
			return "Southeast";
		case South:
			return "South";
		case Southwest:
			return "Southwest";
		case West:
			return "West";
		case Northwest:
			return "Northwest";
		}
		return "";
	}


	int GetDistance(int x1, int y1, int x2, int y2)
	{
		int x, y;

		x = x2 - x1;
		y = y2 - y1;

		// TODO: precalculate more close values!

		if( x >= -2 && x <= 2 && y >= -2 && y <= 2 )
		{
			return DistanceLookup[x+2][y+2];
		}
		else
		{
			return (x*x) + (y*y);
		}
	}

	void Create(int theWidth, int theHeight);
	bool MoveActor(int actorx, int actory, Dir theDirection);

	void Save(void);
	void Load(std::string theMapName);


public:
	int	mID;

	int	mWidth;
	int mHeight;
	int mTileCount;

	std::string		mName;

	WallTile**		mWallTiles;
	GroundTile**	mGroundTiles;
	Actor**			mActors;

};



}

