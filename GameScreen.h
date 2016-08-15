#ifndef __TITLE_SCREEN_H__
#define __TITLE_SCREEN_H__

#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonListener.h"
#include "../SexyAppFramework/EditWidget.h"
#include "../SexyAppFramework/EditListener.h"
#include "../SexyAppFramework/SexyMatrix.h"
#include "OptionsDialogHost.h"
#include "LevelPakDialog.h"
#include "Movie.h"
#include "Attractor.h"
#include "Tile.h"
#include "Floater.h"
#include "Force.h"
#include "Digit.h"
#include "FloatingNumber.h"
#include "PauseBGTint.h"
#include "globals.h"

#include "FancyTextWidget.h"
#include "ImageLinkWidget.h"
#include "PuzzlePakItem.h"

#include "mapview.h"
#include "gamepanel.h"
#include "action.h"

namespace Sexy
{


class GameApp;
class Graphics;
class WidgetManager;
class ButtonWidget;
class Transform;


class GameScreen :
	public Widget,
	public ButtonListener,
	public OptionsDialogHost,
	public LevelPakDialogHost,
	public MovieHost,
	public EditListener,
	public DialogListener
{
public:


private:
	GameApp*				mApp;
	DialogButton*			mStartGameBtn;	
	DialogButton*			mOptionsBtn;	
	DialogButton*			mLevelPaksBtn;	
	DialogButton*			mEditorBtn;	
	DialogButton*			mExitBtn;
	DialogButton*			mGameModeBtn;
	EditWidget*				mSaveFilenameEdit;
	EditWidget*				mLoadFilenameEdit;

	ButtonWidget*			mEditorButtons[11];

	Color					mBackgroundColor;
	Color					mSourceBackgroundColor;



	float					mScoreDropStartTime;
	float					mScoreRaiseStartTime;

	float					mCloudx;
	float					mCloudy;



	Force					mBackgroundTileForce;
	Force					mTitleLogoForce;
	Force					mFloatingScoreForce;

	bool					mIsDrawingScore;
	Digit*					mScoreDigits[10];

public:
	MapView*				mMapView;
	GamePanel*				mGamePanel;

	int						mState;


	bool					mDoneLoading;

	PauseBGTint*			mPauseBGTint;

	Tile*					mTiles[MAX_BACKGROUND_TILES];
	float					mBackgroundTileTimer[MAX_BACKGROUND_TILES];
	short int				mBackgroundTileCount;
	float					mMaxBGTileHeight;

	int						mTitleAlpha;
	int						mLoadingAlpha;


	int						mBGTileCenterX;
	int						mBGTileCenterY;


	bool mIsFillingGrid;
	float mNextFillTime;
	Tile* mFillTiles[MAX_GRID_SIZE];

	Tile		mEditTile;

	Movie*		mMovie;		
	MemoryImage*		mTempImage;

	std::string		mLevelPakName;
	std::string		mNewLevelPakName;
	
	int				mArcadeLevel;
	int				mArcadeNextLevelGap;
	
	
	FancyTextWidget		mText;

	float					mAnimateStartTime;
	float					mAnimateCurrentTime;


	std::vector<std::string>		mItemNames;

	PuzzlePakItem*					mPuzzlePakItem;
	std::vector<PuzzlePakItem*>		mPuzzlePakItems;

	// puzzle pak item effects
	HGE::hgeParticleSystem* ps;
	HGE::hgeParticleSystem* ps2;
	HGE::hgeParticleSystem* ps3;

	bool							mIsItemUncoverEnabled;

	bool							mIsEndGame;

	ImageLinkWidget*				mNSPGamesLink;


	// enums


	enum State
	{
		STATE_LOADING,
		STATE_OPEN_MENU,
		STATE_PLAYING_TITLE,
		STATE_PLAYING_GAME,
		STATE_EDIT_MAP
	};

	enum GameMode
	{
		Classic,
		Arcade
	};

	enum InterfaceButtonIDs
	{
		EXIT_BUTTON_ID,
		EDITOR_BUTTON_ID,
		OPTIONS_BUTTON_ID,
		RESTART_BUTTON_ID,
		PAUSE_BUTTON_ID,
		LEVELPAKS_BUTTON_ID,
		GAMEMODE_BUTTON_ID,
		SAVELEVEL,
		NEWLEVELPAK,
		CLEARLEVEL,
		NEXTLEVEL,
		PREVLEVEL,
		ADDLEVEL,
		SPLITLEVELS,
		UNSPLITLEVELS,
		TILE4,
		TILE5,
		BLOCK,
		INSTRUCTIONS_DIALOG_ID,
		STARTGAME_BUTTON_ID
	};

	enum DialogIDs
	{
		CONFIRM_QUIT_ID,
		CONFIRM_PAUSE_ID
	};


	enum MOVIE_ID
	{
		LOSE_MOVIE_ID,
		WIN_MOVIE_ID,
		WIN_2_MOVIE_ID,
		WIN_3_MOVIE_ID,
		INSTRUCTIONS_MOVIE_ID,
		WIN_PAK_MOVIE_ID
	};

	GameMode	mGameMode;
	bool			mIsShowingEndGame;

	int						mScore;
	int						mCurrentScore;

	int						mHighScore;

	int						mHighestLevelPakCompleteNumber;

	int						mHappyLength;

	bool					mIsCheckingFor5MinDemoSwitch;
	bool					mRandomReset;


	//////////////////////////////////////////////////////////////////////////
	//	Function: GameScreen
	//	Parameters:
	//		theApp	- Pointer to the main application class
	//	
	//	Returns: none
	//////////////////////////////////////////////////////////////////////////
	GameScreen(GameApp* pApp);

	virtual ~GameScreen();

	//////////////////////////////////////////////////////////////////////////
	//	Function: Init
	//	Parameters: none
	//	Returns: none
	//
	//	Purpose: Called BEFORE the title screen is added to the widget manager
	//	by GameApp. This initializes some things like the images used for
	//	our hyperlink widget.
	//////////////////////////////////////////////////////////////////////////	
	void Init(void);

	//////////////////////////////////////////////////////////////////////////
	//	Function: AddedToManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the GameScreen class is
	//	added to its list of widgets. Every widget gets this function
	//	called when it is first added. It useful to use this function to
	//	set up any other widgets that the class might contain, such as buttons.
	//////////////////////////////////////////////////////////////////////////	
	void AddedToManager(WidgetManager* theWidgetManager);

	//////////////////////////////////////////////////////////////////////////
	//	Function: RemovedFromManager
	//	Parameters:
	//		theWidgetManager	- Pointer to the main widget manager from
	//								GameApp.
	//
	//	Returns: none
	//
	//	Purpose: This function is automatically called by the widget manager
	//	which also passes a pointer to itself, when the GameScreen class is
	//	removed from its list of widgets. Every widget gets this function
	//	called when it is finally removed. It useful to use this function to
	//	also remove any widgets that were added and created in AddedToManager.
	//////////////////////////////////////////////////////////////////////////
	void RemovedFromManager(WidgetManager* theWidgetManager);

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
	virtual void ButtonDepress(int theId);

	//////////////////////////////////////////////////////////////////////////
	//	Function: Draw
	//	Parameters:
	//		g	- Graphics object used to draw all images and fonts to the screen.
	//	
	//	Returns: none
	//
	//	Purpose: Called automatically by GameApp's WidgetManager. This is where
	//	we'll do all our display routines for the loading screen.
	//////////////////////////////////////////////////////////////////////////
	void Draw(Graphics* g);

	//////////////////////////////////////////////////////////////////////////
	//	Function: LoadingComplete
	//	Parameters: none
	//	Returns: none
	//
	//	Purpose: Called manually by GameApp when we are done loading all
	//	resources, to let the title screen know that it should unhide and
	//	enable the continue link, so the user can start playing the game.
	//////////////////////////////////////////////////////////////////////////
	void LoadingComplete();

	//////////////////////////////////////////////////////////////////////////
	//	Function: UpdateF
	//	Parameters: 
	//		theFrac	- The number of updates this time slice represents.
	//
	//	Returns: none
	//
	//	Purpose: Like Update, but allows for smooth motion. See previous
	//	demos or the doc for a lengthy explanation.
	//////////////////////////////////////////////////////////////////////////
	void Update(void);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void StartAnimation();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void Pause(bool p);



	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void OnOptionsClosed(void);
	void OnLevelPaksClosed(std::string theSelectedPakName);

	void LoadPak(std::string theSelectedPakName);


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	bool MoveMainActor(Map::Dir theDirection);

	bool Wait();
	bool ShowActorStats(int actorNumber);

	void KeyChar(char theChar);
	void KeyDown(Sexy::KeyCode theKey);

	void EditWidgetText(int theId, const std::string& theString);
	bool AllowText(int theId, const std::string &theText);


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	void InitScore(void);
	void DeleteScore(void);
	void AddScore(int amount);
	void SetScore(void);
	void SetHighScore(int theHighScore);

	void RemoveBackgroundTile(int theTileNumber);
	void AddBackgroundTile(int theTileNumber, int theTileColor, int theTileSize);

	void CreateEditorButtons(void);
	void RemoveEditorButtons(void);

	void PlayMovie(enum MOVIE_ID);
	void StopMovie();
	void OnMovieEnd();

	void LoseGame();
	void ShowInstructionsDialog();

	void GameScreen::ResetArcadeBoard();
	void GameScreen::StartRandomGame();

	void GameScreen::ResetItemUncoveredPart();
	void GameScreen::SetItemUncoveredPart(float part);
	void GameScreen::SetItemUncoveredParticleLine(float part);

	void DialogButtonPress(int theDialogId, int theButtonId);

	void SetHighestLevelPakCompleteNumber();


};

} 

#endif //__TITLE_SCREEN_H__