#pragma once

#include "..\sexyappframework\image.h"

#include "hgeParticle.h"

#include "Floater.h"
#include "Force.h"
#include "Digit.h"

namespace Sexy
{

class Tile :
	public Floater
{
public:
	Tile(void);

public:
	~Tile(void);

public:
	int		mColor;
	Color	mBlendColor;

	int		mDirection;
	int		mThresholdX;
	int		mThresholdY;
	int		mShiftCount;
	bool	mIsSliding;

	int		mHomeX;
	int		mHomeY;

	bool	mIsBoardTile;
	bool	mIsEditTile;

	bool	mIsWild;
	float	mWildTick;

	bool	mIsBlock;

	char	mMatchLevel;

	Force*	mForce;

	Digit*	mNumberOverlay;

	HGE::hgeParticleSystem*	mParticleSystem;


	virtual void	Draw(Graphics* g);

	virtual void	MouseMove(int x, int y);
	virtual void	MouseDown(int x, int y, int theClickCount);

	virtual void	Update(void);
};

}