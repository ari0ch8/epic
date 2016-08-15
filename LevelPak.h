#pragma once

#include "Level.h"
#include "PuzzlePakItem.h"

#include <string>

namespace Sexy
{

#define MAX_LEVEL_COUNT		1001

class LevelPak
{
public:
	LevelPak(void);
public:
	~LevelPak(void);

public:
	std::string mName;
	std::string mFilename;

	std::string mItemName;

	int mLevelCount;
	Level mLevels[MAX_LEVEL_COUNT];
	
	int mCurrentLevel;
	int mNewCurrentLevel;

	PuzzlePakItem*		mPuzzlePakItem;


	Level*	GetNextLevel(void);

	void	SaveToFile(const std::string filename);
	void	LoadFromFile(const std::string filename);

	void	LoadLevelsFromMB1(std::string filename);
	void	LoadBonusLevelsFromMB1(std::string filename);
	
	void	LoadLevelsFromFolder(void);
	void	SaveLevelsToFolder(void);

	int		GetTotalTileCount(void);
	float	LevelPak::GetPartCompleteToLevel(int theLevel);


	static void LevelPak::FillLevelPakList(std::vector<std::string> &theList);

};


}