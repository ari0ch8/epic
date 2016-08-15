#pragma once
#include "floater.h"

namespace Sexy
{

class PuzzlePakItem :
	public Sexy::Floater
{
public:
	PuzzlePakItem(void);
public:
	~PuzzlePakItem(void);

	std::string			mName;

	float				mPartUncovered;
	float				mCurrentPartUncovered;

	Image*				mLargeImage;


	void				PuzzlePakItem::SetImage(void);


	void				Draw(Graphics* g);
	void				Update(void);
};

}