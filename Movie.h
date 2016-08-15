#pragma once
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonWidget.h"
#include "../SexyAppFramework/DialogButton.h"
#include "../SexyAppFramework/Rect.h"
#include "../SexyAppFramework/SexyMatrix.h"
#include "../SexyAppFramework/ImageFont.h"
#include "tile.h"
#include "Floater.h"
#include "Force.h"
#include "globals.h"

namespace Sexy
{

class MovieHost
{	
public:
	virtual void			OnMovieEnd(void) {}
};

class Movie :
	public Widget, ButtonListener
{
public:
	Movie(MovieHost* theHost);
	~Movie(void);

	enum MovieType
	{
		FLOATINGIMAGE,
		TEXT
	};

	enum Widgets
	{
		CONTINUE_BUTTON
	};

	MovieType	mType;

	std::string mText;

	int			mLength;
	float		mMovieTimer;

	bool		mHasContinueButton;
	DialogButton* mContinueButton;
	int			mContinueButtonX;
	int			mContinueButtonY;

	MovieHost*	mHost;

	Floater*	mMainTextFloater;
	Force*		mMainTextForce;

	Image*		mImage;

	// functions
	virtual void ButtonDepress(int theId);

	void Update(void);
	void Draw(Graphics* g);

	void AddedToManager(WidgetManager* theWidgetManager);
	void RemovedFromManager(WidgetManager* theWidgetManager);


};

}