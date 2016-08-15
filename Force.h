#pragma once

#include "../SexyAppFramework/SexyMatrix.h"

#include "Floater.h"

namespace Sexy
{
//enum ForceType
//{
//	Attractor,
//	Magnet
//};


class Force
{
public:
	Force(void);
public:
	~Force(void);

	SexyVector3		mSource;
	float			mMass;
	float			mMagnetism;

	bool			mActive;

	void UpdateFloater(Floater& theFloater, float theFrac);
};

}