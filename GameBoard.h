#pragma once
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonListener.h"
#include "../SexyAppFramework/Rect.h"
#include "../SexyAppFramework/SexyMatrix.h"
#include "gameapp.h"
#include "sliderbutton.h"
#include "tile.h"
#include "levelpak.h"
#include "globals.h"



namespace Sexy
{

// Used in checkboard() to look for matches
struct CHECKNODE
{
	// Board location
	int x;	
	int y;

	char color;
	int childno[4];
	int parentno;
	char numofchildren;
};


enum CHECK_BOARD_RESULT
{
	KEEP_PLAYING,
	WIN_GAME,
	NO_MORE_MATCHES,
};


class WidgetManager;
class Transform;

class GameBoard :
	public Sexy::Floater
{
private:
	bool		mAnimating;
	int			mMaxAlpha;

	bool		mDemo;
	Transform	mTransform;

	GameApp*	mApp;


public:
	SliderButton mSliderButtons[MAX_GRID_SIZE*4];

	float		mTimer;

	float		mNextArcadeSlideInTime;
	float		mArcadeSlideInDelay;


	Tile		mBoardTiles[MAX_BOARD_TILES];

	int			emptySpaceChance;
	int			emptyFillTileChance;
	int			wildTileChance;
	int			tileColors;
	int			startingTileColor;

	GameScreen* mGameScreen;

	enum State
	{
		WaitingForUser,
		SlidingTiles,
		TileDisappear,
		LoadingLevel,
		WinLevel,
		FadeIn,
		FadingIn,
		FadeOut,
		TileAppear,
		EditingLevel
	};

	bool		mIsAutoFadeIn;

	float		mFadeTimer;
	float		mFadeLength;
	int			mFadeHomeX;
	int			mFadeHomeY;

	int			mHomeX;
	int			mHomeY;

	float		mAlpha;

	int			mGridSize;

	State		mState;
	State		mStoredState;

	int			mFocusedSliderNumber;

	struct CHECKNODE checknode[MAX_GRID_SIZE*MAX_GRID_SIZE];
	int nextavailablenode;
	int numofmatches;
	int numofchildrenneededformatch;
	int numofstrands;

	bool mIsAddingNewTiles;

	bool mIsMonkey;

	// int currfloatnumber;

	int higheststrand;

	char flagdelete[MAX_GRID_SIZE*MAX_GRID_SIZE];
	char matched[MAX_GRID_SIZE*MAX_GRID_SIZE];

	short int	nextfloatnumber;

	Level		myLevel;
	int			mCurrentEditColor;
	Tile*		mEditTileSelected;

	Force		mFloatingTileLogoForce;

	bool		mIsColorUsed[8];		


	Force		mBoardTileForces[MAX_BOARD_TILES];
	float		mTileTransitionEndTime;


	int			mNumOfClicks;
	int			mNumOfTimesLost;

	int			mLimitToSliderNumber;

	bool		mCanScore;


//
// Function Declarations
//


	GameBoard(GameApp* theApp);
	virtual ~GameBoard();

	//////////////////////////////////////////////////////////////////////////
	//	Function: KeyChar
	//	Parameters:
	//		theChar - The ASCII character pressed
	//
	//	Purpose: Called by the WidgetManager if a key is pressed and the Board
	//	has focus. Used to see if space was pressed, to toggle pause state.
	//////////////////////////////////////////////////////////////////////////		
	virtual void KeyChar(char theChar);

	//////////////////////////////////////////////////////////////////////////
	//	Function: Draw
	//	Parameters:
	//		g	- Graphics object used to draw all images and fonts to the screen.
	//	
	//	Returns: none
	//
	//	Purpose: Called automatically by GameApp's WidgetManager, this function
	//	is the main method that is responsible for all graphical and textual
	//	displaying.
	//////////////////////////////////////////////////////////////////////////
	virtual void Draw(Graphics* g);		

	//////////////////////////////////////////////////////////////////////////
	//	Function: Update
	//	Parameters: none
	//	Returns: none
	//
	//	Purpose: Called automatically by GameApp's WidgetManager, this method
	//	is GUARANTEED to be called 100 times per second (100FPS) and is where
	//	all main game logic is performed. Of course, if you had a larger more
	//	complex game, you'd most likely divide your logic between several
	//	other files, but this is commonly the central place where all game
	//	logic begins and is executed.
	//////////////////////////////////////////////////////////////////////////
	virtual void Update();

	////////////////////////////////////////////////////////////////////////////
	////	Function: UpdateF
	////	Parameters: 
	////		theFrac	- The number of updates this time slice represents.
	////
	////	Returns: none
	////
	////	Purpose: Like Update, but allows for smooth motion. See previous
	////	demos or the doc for a lengthy explanation.
	////////////////////////////////////////////////////////////////////////////
	//virtual void Update(void);

	//////////////////////////////////////////////////////////////////////////
	//	Function: ButtonDepress
	//	Parameters:
	//		theId	- Integer ID of the button that was clicked
	//
	//	Returns: none
	//
	//	Purpose: This method is called by the WidgetManager when a button widget
	//	is first pressed and THEN released. You can use ButtonPress if you want
	//	to know when the button is first pressed (before it is released).
	//	theId is the integer ID that was assigned to the button when it was
	//	first created. 
	//////////////////////////////////////////////////////////////////////////		
	virtual void	ButtonDepress(int theId);

	//////////////////////////////////////////////////////////////////////////
	//	Function: AddedToManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the Board class is
	//	added to its list of widgets. Every widget gets this function
	//	called when it is first added. It useful to use this function to
	//	set up any other widgets that the class might contain, such as buttons.
	//////////////////////////////////////////////////////////////////////////		
	virtual void	AddedToManager(WidgetManager* theWidgetManager);

	//////////////////////////////////////////////////////////////////////////
	//	Function: RemovedFromManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the Board class is
	//	removed from its list of widgets. Every widget gets this function
	//	called when it is finally removed. It useful to use this function to
	//	also remove any widgets that were added and created in AddedToManager.
	//////////////////////////////////////////////////////////////////////////
	virtual void	RemovedFromManager(WidgetManager* theWidgetManager);
		
	//////////////////////////////////////////////////////////////////////////
	//	Function: MouseMove
	//	Parameters:
	//		x - X coordinate relative to the application of the mouse 
	//		y - Y coordinate relative to the application of the mouse
	//
	//	Returns: none
	//
	//	Purpose: Called by the WidgetManager automatically anytime the
	//	mouse moves. The topmost widget (i.e. the widget most immediately
	//	under the cursor) is the one who gets the function call, and any
	//	widgets underneath by default are ignored.
	//////////////////////////////////////////////////////////////////////////		
	virtual void	MouseMove(int x, int y);

	//////////////////////////////////////////////////////////////////////////
	//	Function: MouseDown
	//	Parameters:
	//		x - X coordinate relative to the application of the mouse 
	//		y - Y coordinate relative to the application of the mouse
	//		theClickCount - An integer indicating which mouse button
	//						was pressed. One of the following:
	//			1:  Left button
	//			2:  Double-left-click
	//			3:  Middle button
	//			-1: Right button
	//			-2: Double-right-click
	//
	//		IMPORTANT: Because you can't have a double click with a
	//		single click, you will receive BOTH a left (or right) button as well
	//		as a double left (or right) button message whenever you double-click.
	//
	//	Returns: none
	//
	//	Purpose: Called by the WidgetManager automatically anytime 
	//	a mouse button is in the down state.
	//	The topmost widget (i.e. the widget most immediately
	//	under the cursor) is the one who gets the function call, and any
	//	widgets underneath by default are ignored.
	//////////////////////////////////////////////////////////////////////////
	virtual void	MouseDown(int x, int y, int theClickCount);

	//////////////////////////////////////////////////////////////////////////
	//	Function: MouseDrag
	//	Parameters:
	//		x - X coordinate relative to the application of the mouse 
	//		y - Y coordinate relative to the application of the mouse
	//
	//	Returns: none
	//
	//	Purpose: Called by the WidgetManager automatically anytime the
	//	mouse moves AND a button is held down. This is the same thing as
	//	MouseMove except that it represents a drag operation. When dragging,
	//	MouseDrag will be called INSTEAD OF MouseMove.
	//	The topmost widget (i.e. the widget most immediately
	//	under the cursor) is the one who gets the function call, and any
	//	widgets underneath by default are ignored.
	//////////////////////////////////////////////////////////////////////////
	virtual void	MouseDrag(int x, int y);

	virtual void	MouseEnter(void);
	virtual void	MouseLeave(void);

	virtual void	MouseWheel(int theDelta);


	//////////////////////////////////////////////////////////////////////////
	//	Function: Pause
	//	Parameters:
	//		p - Boolean indicating pause or unpause
	//
	//	Purpose: Pauses/unpauses the game
	//////////////////////////////////////////////////////////////////////////		
	void			Pause(bool p);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void			DrawGrid(Graphics* g);

	void			DrawTiles(Graphics* g);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void			DrawUI(Graphics* g);

	void			ShiftTiles(float theFrac);

	void			StartTileShift(void);
	void			StartTileShiftByOne(Direction theDirection);

	CHECK_BOARD_RESULT				CheckBoard(bool canScore);

	int				getchildren(int nodeno);

	void			removeblock(int nodeno);

	int				checkspace(int x, int y);

	BOOL			nomorematches(void);

	void			RandomizeBoard(bool includeBlocks, int numberOfColors = 3);

	void			KeyDown(Sexy::KeyCode theKey);

	void			FocusStolen(void);
	void			SetBackgroundTiles(void);

	void			CreateBoardObjects(void);

	void			LoadLevel(Level &theLevel);
	void			ChangeToNewTiles(void);

	void			SaveLevel(Level &theLevel);

	void			RandomizeFillTiles(void);
	void			SetArcadeSlideInDelay(int theLevel);
	void			AddNewFillColor(void);


	void			FadeInTiles(void);
	void			FadeOutTiles(void);
	void			ClearTiles(void);

};

}