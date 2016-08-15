#pragma once


#include "../SexyAppFramework/SexyAppBase.h"
#include "Tile.h"
#include "GameBoard.h"
#include "res.h"
#include "globals.h"

#include "GameScreen.h"
#include "gameapp.h"

namespace Sexy
{

class FancyTextWidget :
	public Widget
{

#define NUM_OF_TEXT_LINES	8
#define TEXT_LINE_HEIGHT	30

	class TextLine
	{
	public:
		float mTextX;
		float mTextY;
		std::string	mText;
	};

public:
	FancyTextWidget(void);
public:
	~FancyTextWidget(void);

public:
	// public members
	TextLine			mTextLines[NUM_OF_TEXT_LINES];

	bool				mIsDrawingTalkBubble;

	int					mFillX;
	int					mFillY;
	int					mFillWidth;
	int					mFillHeight;

	// functions
	void				Draw(Graphics* g);
	void				SetText(std::string text, int maxlength);
	void				SetPosition(int x, int y);
};

};