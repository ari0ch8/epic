#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"

namespace Sexy
{

class SliderButton :
	public Widget
{
public:
	SliderButton(void);
	~SliderButton(void);

public:
	float	mAnimateCurrentTime;
	float	mAnimationLength;

	int		mRotate;
	Color	mBlendColor;
	Color	mHoverBlendColor;
	float	mFadeLevel;
	int		mFadeState; // -1, 0, 1

	bool	mIsAttractor;
	int		mRow;
	int		mColumn;

	Color	mBoardRowColor;
	Color	mBoardRowCurrentColor;

	void	DrawBoardHover(Graphics* g);

	virtual void Draw(Graphics* g);
	virtual void Update(void);

	virtual void MouseEnter(void);
	virtual void MouseLeave(void);
	virtual void MouseDown(int x, int y, int theClickCount);

	virtual void KeyDown(KeyCode theKey);
};

}