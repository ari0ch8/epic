#include "LevelPak.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <ios>

using namespace Sexy;


LevelPak::LevelPak(void)
{
	mName = "New Level Pak";
	mFilename = "";
//	ZeroMemory(&mLevels,MAX_LEVEL_COUNT*sizeof(Level));
	mCurrentLevel = 1;	
	mNewCurrentLevel = 1;
	
	// populate default 1 level
	mLevelCount = 1;

	for( int i=1; i<MAX_LEVEL_COUNT; i++ )
	{
		mLevels[i].mGridSize = 10;
//		mLevels[i].ClearTiles();
	}

	mPuzzlePakItem = NULL;
}

LevelPak::~LevelPak(void)
{
}


Level*	LevelPak::GetNextLevel(void)
{
	if( mCurrentLevel < mLevelCount ) mNewCurrentLevel++;
		else mNewCurrentLevel = 1;

	return &mLevels[mNewCurrentLevel];
}

void	LevelPak::SaveToFile(const std::string filename)
{
	std::ofstream levelPakFile;

	char levelName[_MAX_FNAME+1];
	memset(levelName,0,_MAX_FNAME+1);
	memcpy(levelName, mName.c_str(), mName.size());

	try
	{
		levelPakFile.open( filename.c_str(), std::ios_base::out );
		levelPakFile.write( (char *)(L"MATCHBLOX2_LEVELPAK_V2.0"), 48 );
		levelPakFile.write( (char *)levelName, _MAX_FNAME+1 );
		levelPakFile.write( (char *)&mLevelCount, sizeof(int) );

		// BAD! moving backwards on last level to write item name!
		levelPakFile.write( (char *)mLevels, (sizeof(Level)*MAX_LEVEL_COUNT)-64 );

		char tempName[64];
		memset( tempName, 0, 64 );
		strcpy( tempName, mItemName.c_str() );

		levelPakFile.write( tempName, 64 );

		levelPakFile.write( (char *)(L"MATCHBLOX2_LEVELPAK_V2.0"), 48 );
		levelPakFile.close();
	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}


void	LevelPak::LoadLevelsFromMB1(std::string filename)
{
	std::ifstream levelPakFile;

//	wchar_t fileId[24];

	char levelName[_MAX_FNAME+1];
	memset(levelName,0,_MAX_FNAME+1);
	memcpy(levelName, mName.c_str(), mName.size());;

	char puzzleboard[100*100L];
	char puzzletype[100];

	mLevelCount = 100;

	mName = "MatchBlox";

	try
	{
		levelPakFile.open( filename.c_str(), std::ios_base::in );

		// TODO: validate level count
		levelPakFile.read( (char *)puzzleboard, 100L*100L  );
		levelPakFile.read( (char *)puzzletype, 100  );

		// dump to my levels
		for( int i = 0; i < 100; i++ )
		{
			for( char c = 0; c < 100; c++ )
			{
				if( puzzleboard[c+(i*100)] != 0 && puzzleboard[c+(i*100)] <= 8 )
					mLevels[i+1].mTiles[c] = (puzzleboard[c+(i*100)] - 1);
				else
					mLevels[i+1].mTiles[c] = -1;
			}
		}

		levelPakFile.close();
	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}

void	LevelPak::LoadBonusLevelsFromMB1(std::string filename)
{
	std::ifstream levelPakFile;

//	wchar_t fileId[24];

	char levelName[_MAX_FNAME+1];
	memset(levelName,0,_MAX_FNAME+1);
	memcpy(levelName, mName.c_str(), mName.size());

	char puzzleboard[10*100L];
	char puzzletype[10];

	mLevelCount = 10;

	mName = "MatchBlox Bonus";

	try
	{
		levelPakFile.open( filename.c_str(), std::ios_base::in );

		// bypass first 100
		levelPakFile.seekg( 10100L );

		// TODO: validate level count
		levelPakFile.read( (char *)puzzleboard, 10L*100L  );
		levelPakFile.read( (char *)puzzletype, 10  );

		// dump to my levels
		for( int i = 0; i < 10; i++ )
		{
			for( char c = 0; c < 100; c++ )
			{
				if( puzzleboard[c+(i*100)] != 0 && puzzleboard[c+(i*100)] <= 8 )
					mLevels[i+1].mTiles[c] = (puzzleboard[c+(i*100)] - 1);
				else
					mLevels[i+1].mTiles[c] = -1;
			}
		}

		levelPakFile.close();
	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}




void	LevelPak::LoadFromFile(std::string filename)
{
	std::ifstream levelPakFile;

	wchar_t fileId[24];
	char itemName[sizeof(Level)];

	char levelName[_MAX_FNAME+1];

	try
	{
		memset(itemName,8,sizeof(Level));

		memset(levelName,0,_MAX_FNAME+1);
		memcpy(levelName, mName.c_str(), mName.size());;


		levelPakFile.open( filename.c_str(), std::ios_base::in );
		levelPakFile.read( (char *)fileId, 48 );

//		if( std::wstring(fileId) == std::wstring(L"MATCHBLOX2_LEVELPAK_V2.0") )
//		{
			levelPakFile.read( (char *)levelName, _MAX_FNAME+1 );
			mName = levelName;
			levelPakFile.read( (char *)&mLevelCount, sizeof(int) );

			// TODO: validate level count
			levelPakFile.read( (char *)mLevels, (sizeof(Level)*(mLevelCount+1)) );

			// skip blank levels
			levelPakFile.seekg( 0-(48+64), std::ios_base::end );

			// Changing end version sig to storage for item name
			// levelPakFile.read( (char *)fileId, 48 );

			levelPakFile.read( (char *)itemName, 48 );

			// force null termination for up to 16 bits
			itemName[47] = NULL;
			itemName[48] = NULL;

			// TODO: valiadte item name! old level paks will have invalid data here
			mItemName = std::string(itemName);
			
			// TODO: use last 24 reserved bytes?
//		}
//		else if( std::wstring(fileId) == std::wstring(L"MATCHBLOX2_LEVELPAK_V2.1") )
//		{
//		}

		levelPakFile.close();
	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}



void	LevelPak::LoadLevelsFromFolder(void)
{
	std::ifstream levelFile;
	std::string filename;

	int levelNumber;

	try
	{
		// TODO: validate level count
		for( levelNumber = 1; levelNumber < mLevelCount+1; levelNumber++ )
		{
			filename = "LevelPaks\\";
			filename += mName;
			filename += ".Levels\\Level";

			// old school
			if( levelNumber < 10 ) filename += "0";
			if( levelNumber < 100 ) filename += "0";
			if( levelNumber < 1000 ) filename += "0";

			std::stringstream s;
			s << levelNumber;
			filename += s.str();

			levelFile.open( filename.c_str(), std::ios_base::in );

			levelFile.read( ((char *)mLevels) + (sizeof(Level)*levelNumber),
				sizeof(Level) );

			levelFile.close();
		}

	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}


void	LevelPak::SaveLevelsToFolder(void)
{
	std::ofstream levelFile;
	std::string filename;
	std::string foldername;

	int levelNumber;

	try
	{
		foldername = "LevelPaks\\";
		foldername += mName;
		foldername += ".Levels";

		CreateDirectory( foldername.c_str(), NULL );

		// TODO: validate level count
		for( levelNumber = 1; levelNumber < mLevelCount+1; levelNumber++ )
		{
			filename = foldername;
			filename += "\\Level";

			// old school
			if( levelNumber < 10 ) filename += "0";
			if( levelNumber < 100 ) filename += "0";
			if( levelNumber < 1000 ) filename += "0";

			std::stringstream s;
			s << levelNumber;
			filename += s.str();

			levelFile.open( filename.c_str(), std::ios_base::out );

			levelFile.write( ((char *)mLevels) + (sizeof(Level)*levelNumber),
				sizeof(Level) );

			levelFile.close();
		}
	}
	catch(std::exception ex)
	{
		// TODO: add message handler
		throw( ex );
	}
}



int LevelPak::GetTotalTileCount()
{
	int levelNumber = 0;
	int tileCount = 0;
	int tileNo = 0;


	for( levelNumber = 1; levelNumber <= mLevelCount; levelNumber++ )
	{
		tileCount += mLevels[levelNumber].CountTiles();
	}

	return tileCount;
}


float LevelPak::GetPartCompleteToLevel(int theLevel)
{
	int tileCount = 0;
	int totalTileCount = 0;
	int levelNumber = 0;
	float partComplete = 0.0f;


	totalTileCount = GetTotalTileCount();

	for( levelNumber = 1; levelNumber < theLevel; levelNumber++ )
	{
		tileCount += mLevels[levelNumber].CountTiles();
	}

	partComplete = (float)tileCount / (float)totalTileCount;

	return partComplete;
}




void LevelPak::FillLevelPakList(std::vector<std::string> &theList)
{
	// load order
	std::string levelPakOrder = gSexyAppBase->GetString("LEVEL_PAK_ORDER",
		"How To Play|Reflections|Find Your Way|Spinning Circles|Due North|Stay The Course|Rough Waters|Land, Ho!|The Dark Path|Time To Spare|A Foul Stench|Yo, Ho, Ho!|Lost Treasures|MatchBlox");

	Split( levelPakOrder, "|", theList );
}




