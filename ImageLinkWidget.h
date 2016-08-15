#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"

namespace Sexy
{

class ImageLinkWidget :
	public Widget
{
public:
	ImageLinkWidget(void);
	~ImageLinkWidget(void);

public:
	Image*	mImage;
	Image*	mHoverImage;

	SexyString mLinkTarget;

	float	mAnimateCurrentTime;
	float	mAnimationLength;

	Color	mBlendColor;
	Color	mHoverBlendColor;
	float	mFadeLevel;
	int		mFadeState; // -1, 0, 1

	void	DrawBoardHover(Graphics* g);

	virtual void Draw(Graphics* g);
	virtual void Update(void);

	virtual void MouseEnter(void);
	virtual void MouseLeave(void);
	virtual void MouseDown(int x, int y, int theClickCount);
};

}