#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/SexyMatrix.h"
#include "../SexyAppFramework/Widget.h"
#include <vector>

namespace Sexy
{

class Widget;

class Floater :
	public Widget
{
public:
	Floater(void);
public:
	~Floater(void);

public:
	bool						mIsActive;
	int							mId;

	SexyVector3					mPosition;
	SexyVector3					mPositionNext;

	SexyVector3					mAcceleration;
	SexyVector3					mAccelerationNext;

	SexyVector3					mVelocity;
	SexyVector3					mVelocityNext;

	float						mFalloff;

	float						mMaxVelocity;


	float						mWidthF;
	float						mHeightF;

	float						mScaleF;

	Sexy::Image*				mImage;


	virtual void	Update(void);

	virtual void	Draw(Graphics* g);

	void			Stop(void);
};

}