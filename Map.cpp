#include "Map.h"

#include <sstream>

#include "globals.h"

#include "sqlite3.h"

using namespace Sexy;



const int Map::DistanceLookup[5][5] = {
	{ 3,2,2,2,3 }, 
	{ 2,1,1,1,2 },
	{ 2,1,0,1,2 },
	{ 2,1,1,1,2 },
	{ 3,2,2,2,3 }
};



static int ActorLoadCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	int id = 0;
	int x, y;
	int life;

	for(i=0; i<argc; i++)
	{
		if( strcmp(azColName[i],"id") == 0 )
		{
			id = atoi(argv[i]);
		}
		else if( strcmp(azColName[i],"name") == 0 )
		{
			gActorManager.mActors[id].mName = argv[i];
		}
		else if( strcmp(azColName[i],"x") == 0 )
		{
			gActorManager.mActors[id].mapx = atoi(argv[i]);
		}
		else if( strcmp(azColName[i],"y") == 0 )
		{
			gActorManager.mActors[id].mapy = atoi(argv[i]);
		}
		else if( strcmp(azColName[i],"life") == 0 )
		{
			gActorManager.mActors[id].mLife = atoi(argv[i]);
		}
	}

	// set player
	// TODO: set based on entry point
	Actor* actor = &gActorManager.mActors[id];
	gCurrentMap->mActors[((actor->mapy)*256)+actor->mapx] = actor;

	return 0;
}



Map::Map(void)
{
	mWidth = 0;
	mHeight = 0;
	mTileCount = 0;
	mGroundTiles = NULL;
	mWallTiles = NULL;
	mActors = NULL;

	mID = 0;
}

Map::~Map(void)
{
	if (mWallTiles != NULL)
	{
		delete[] mWallTiles;
	}
	if (mGroundTiles != NULL)
	{
		delete[] mGroundTiles;
	}
	if (mActors != NULL)
	{
		delete[] mActors;
	}
}

void Map::Create(int theWidth, int theHeight)
{
//	int tileno;

	mName = "New Map";

	mWidth = theWidth;
	mHeight = theHeight;
	mTileCount = mWidth*mHeight;

	mGroundTiles = new GroundTile*[mTileCount];

	for( int i=0; i<mTileCount; i++ )
	{
		mGroundTiles[i] = &gTileManager.mGroundTiles[22];
	}

	mActors = new Actor*[mTileCount];

	for( int i=0; i<mTileCount; i++ )
	{
		mActors[i] = NULL;
	}

	mWallTiles = new WallTile*[mTileCount];

	for( int i=0; i<mTileCount; i++ )
	{
		mWallTiles[i] = NULL;
	}


}

bool Map::MoveActor(int actorx, int actory, Dir theDirection)
{
	bool wasSuccessful = true;

	bool isBlocked = false;

	Actor*	srcActor = NULL;
	Actor*  destActor = NULL;
	WallTile* wallTile = NULL;

	int srctileno = 0;
	int desttileno = 0;

	srctileno = (actory*mWidth)+actorx;
	srcActor = mActors[srctileno];

	if( srcActor != NULL )
	{
		switch( theDirection )
		{
		case North:
			desttileno = ((actory-1)*mWidth)+actorx;
			break;
		case Northeast:
			desttileno = ((actory-1)*mWidth)+actorx+1;
			break;
		case East:
			desttileno = (actory*mWidth)+actorx+1;
			break;
		case Southeast:
			desttileno = ((actory+1)*mWidth)+actorx+1;
			break;
		case South:
			desttileno = ((actory+1)*mWidth)+actorx;
			break;
		case Southwest:
			desttileno = ((actory+1)*mWidth)+actorx-1;
			break;
		case West:
			desttileno = (actory*mWidth)+actorx-1;
			break;
		case Northwest:
			desttileno = ((actory-1)*mWidth)+actorx-1;
			break;
		}

		destActor = mActors[desttileno];

		if( destActor != NULL )
		{
			isBlocked = true;
		}

		wallTile = mWallTiles[desttileno];

		if( wallTile != NULL )
		{
			isBlocked = true;
		}

		if( !isBlocked )
		{
			mActors[desttileno] = srcActor;
			mActors[srctileno] = NULL;
		}
		else
		{
			wasSuccessful = false;
		}
	}
	else
	{
		wasSuccessful = false;
	}

	if( wasSuccessful )
	{
		Event anEvent;
		std::stringstream textStream;
		textStream << srcActor->mName << " moved " << GetDirectionAsText(theDirection) << ".";
		anEvent.mDescription = textStream.str();

		gEventManager.AddEventToQueue(anEvent);
	}

	return wasSuccessful;
}


void Map::Save(void)
{
	std::string pathToMap;
	std::string pathToBackup;

	pathToMap = "maps\\";
	pathToMap += mName;
	pathToMap += ".map";

	pathToBackup = pathToMap;
	pathToBackup += ".backup";

	CopyFile(pathToMap.c_str(), pathToBackup.c_str(), FALSE);

	FILE* fp = NULL;

	int ver = 3;
	char buf[1024];
	int tileNumber = 0;
	int tempInt = 0;

	memset(buf,0,1024);

	fp = fopen(pathToMap.c_str(), "w");

	if( fp != NULL )
	{
		fwrite("EPICMAP",1,7,fp);
		// ver
		fwrite(&ver,4,1,fp);

		memset(buf,0,1024);
		strcpy_s(buf,1024,mName.c_str());

		fwrite(buf, 1, 32,fp);

		fwrite(&mID, 1, sizeof(int), fp);

		fwrite(&mWidth, 1, 4,fp);
		fwrite(&mHeight, 1, 4,fp);

		for( tileNumber=0; tileNumber<mTileCount; tileNumber++ )
		{
			tempInt = mGroundTiles[tileNumber]->id;
			fwrite(&tempInt,4,1,fp);
		}

		for( tileNumber=0; tileNumber<mTileCount; tileNumber++ )
		{
			if( mWallTiles[tileNumber] != NULL )
			{
				tempInt = mWallTiles[tileNumber]->id;
			}
			else
			{
				tempInt = -1;
			}
			fwrite(&tempInt,4,1,fp);
		}

		fclose(fp);
	}

}


void Map::Load(std::string theMapName)
{
	std::string pathToMap;
	int tileNumber = 0;

	mName = theMapName;

	pathToMap = "maps\\";
	pathToMap += theMapName;
	pathToMap += ".map";

	FILE* fp = NULL;

	int ver;
	int tempInt = 0;

	char buf[1024];
	memset(buf,0,1024);

	fp = fopen(pathToMap.c_str(), "r");

	if( fp != NULL )
	{
		fread(buf,1,7,fp);
		// TODO: compare to EPICMAP for verification

		// ver
		fread(&ver,4,1,fp);

		memset(buf,0,1024);
		fread(buf,1,32,fp);

		if( ver > 2 )
		{
			fread(&mID,1,sizeof(int),fp);
		}

		fread(&mWidth, 1, 4,fp);
		fread(&mHeight, 1, 4,fp);

		Create(mWidth,mHeight);

		// set name after create
		mName = buf;

		for( tileNumber=0; tileNumber<mTileCount; tileNumber++ )
		{
			fread(&tempInt,4,1,fp);

			mGroundTiles[tileNumber] = &gTileManager.mGroundTiles[tempInt];
		}

		if( ver > 1 )
		{
			for( tileNumber=0; tileNumber<mTileCount; tileNumber++ )
			{
				fread(&tempInt,4,1,fp);

				if( tempInt == -1 )
				{
					mWallTiles[tileNumber] = NULL;
				}
				else
				{
					mWallTiles[tileNumber] = &gTileManager.mWallTiles[tempInt];
				}
			}
		}

		fclose(fp);

	}

	// load actors
	sqlite3* db;
	sqlite3_open("db\\epic", &db);
	std::stringstream ss;

	char *zErrMsg = NULL;

	ss << "SELECT * FROM ACTORS WHERE map=" << mID;

	sqlite3_exec(db, ss.str().c_str(), ActorLoadCallback, 0, &zErrMsg);

	sqlite3_close(db);
}

