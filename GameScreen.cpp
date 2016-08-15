#include "GameScreen.h"
#include "GameApp.h"
#include "OptionsDialog.h"
#include "OptionsDialogHost.h"
#include "Res.h"
#include "../SexyAppFramework/Font.h"
#include "../SexyAppFramework/ImageFont.h"
#include "../SexyAppFramework/Graphics.h"
#include "../SexyAppFramework/Image.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/Rect.h"
#include "../SexyAppFramework/HyperlinkWidget.h"
#include "../SexyAppFramework/MusicInterface.h"

#include <iostream>     
#include <sstream>     
#include <iomanip>

#include "strsafe.h"

#include "Globals.h"
#include "gameboard.h"

#include "PauseBGTint.h"

#include "FancyTextWidget.h"

#include "action.h"
#include "mapview.h"


// mem debugging
//#define SEXY_MEMTRACE
//#include "memmgr.h"


using namespace Sexy;



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameScreen::GameScreen(GameApp *pApp)
{
	((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);

	mMapView = NULL;
	mGamePanel = NULL;

	mMouseVisible = false;

	mApp = pApp;		
	mStartGameBtn = NULL;
	mOptionsBtn = NULL;
	mLevelPaksBtn = NULL;
	mEditorBtn = NULL;
	mGameModeBtn = NULL;
	mExitBtn = NULL;
	mSaveFilenameEdit = NULL;
	mLoadFilenameEdit = NULL;

	mMovie = new Movie(this);

	mLevelPakName = "";
	mNewLevelPakName = "";

	mDoneLoading = false;
	mAnimateCurrentTime = 0;
	mAnimateStartTime = 0;
	mTitleAlpha = 0;
	mLoadingAlpha = 0;
	gPauseLevel = 0;

	mBackgroundTileCount = 8;

	mState = STATE_LOADING;

	// board tile
	mEditTile.mPosition.x = 0;
	mEditTile.mPosition.y = 0;

	mEditTile.mImage = IMAGE_BLOCKS;

	mEditTile.mColor = 0;
	mEditTile.mIsActive = false;

	for( int i=0; i<8; i++ )
	{
		mFillTiles[i] = NULL;
	}


	mGameMode = GameMode::Classic;

	mArcadeLevel = 1;
	mArcadeNextLevelGap = gSexyAppBase->GetInteger("ARCADE_NEXT_LEVEL_GAP", 5000);

	mBGTileCenterX = 0;
	mBGTileCenterY = 0;

	mMaxBGTileHeight = gSexyAppBase->GetDouble("BOARD_TILE_WIDTH", 51.2f) * gGraphicsScale;

	mPauseBGTint = NULL;

	mItemNames.push_back("compass");
	mItemNames.push_back("lantern");
	mItemNames.push_back("spyglass");
	mItemNames.push_back("watch");
	mItemNames.push_back("anchor");
	mItemNames.push_back("map");
	mItemNames.push_back("pipe");
	mItemNames.push_back("jug");
	mItemNames.push_back("wheel");
	mItemNames.push_back("journal");
	mItemNames.push_back("navcompass");
	mItemNames.push_back("key");

//	mCursorForce.mMass = 0.3f;
//	mCursorForce.mSource.z = 1;
//	mEditTile.mForce = &mCursorForce;

	mPuzzlePakItem = NULL;

	ps = NULL;
	ps2 = NULL;
	ps3 = NULL;

	SetHighestLevelPakCompleteNumber();

	mHappyLength = 0;

	mIsItemUncoverEnabled = true;

	mIsEndGame = false;
	mIsShowingEndGame = false;

	mTempImage = NULL;

	mNSPGamesLink = NULL;


	if( gSexyAppBase->GetBoolean("ALLOW_5_MIN_DEMO",true) )
	{
		//OutputDebugString("Checking for 5 min demo switch.\r\n");

		if( IsArmadilloDefaultTrial() || IsArmadillo5MinRepeatingTrial() )
		{
			mIsCheckingFor5MinDemoSwitch = true;
			//OutputDebugString("Enabling 5 min demo switch.\r\n");
		}
		else
		{
			mIsCheckingFor5MinDemoSwitch = false;
		}
	}
	else
	{
		mIsCheckingFor5MinDemoSwitch = false;
	}

	mRandomReset = false;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameScreen::~GameScreen()
{
	if( mStartGameBtn )
		delete mStartGameBtn;
	if( mOptionsBtn )
		delete mOptionsBtn;
	if( mLevelPaksBtn )
		delete mLevelPaksBtn;
	if( mEditorBtn )
		delete mEditorBtn;
	if( mGameModeBtn )
		delete mGameModeBtn;
	if( mExitBtn )
		delete mExitBtn;
	if( mSaveFilenameEdit )
		delete mSaveFilenameEdit;
	if( mLoadFilenameEdit )
		delete mLoadFilenameEdit;

	if( mPauseBGTint )
		delete mPauseBGTint;

	if( mMovie->mParent != NULL )
	{
		RemoveWidget(mMovie);
		delete mMovie;
	}

	if( mEditTile.mParent != NULL )
		RemoveWidget(&mEditTile);

	if( mPuzzlePakItem )
		delete mPuzzlePakItem;

	if( mTempImage != NULL )
	{
		delete mTempImage;
	}

	if( mNSPGamesLink != NULL )
	{
		delete mNSPGamesLink;
	}

	if( mMapView != NULL )
	{
		delete mMapView;
	}

	if( mGamePanel != NULL )
	{
		delete mGamePanel;
	}

	if( gCurrentMap != NULL )
	{
		delete gCurrentMap;
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::Init(void)
{
	mPauseBGTint = new PauseBGTint();

	mStartGameBtn = new DialogButton(IMAGE_DIALOG_BUTTON, STARTGAME_BUTTON_ID, this);
	mStartGameBtn->SetFont(FONT_DEFAULT);
	mStartGameBtn->mLabel = gSexyAppBase->GetString("START_GAME_TEXT", "New Game");

	mOptionsBtn = new DialogButton(IMAGE_DIALOG_BUTTON, OPTIONS_BUTTON_ID, this);
	mOptionsBtn->SetFont(FONT_DEFAULT);
	mOptionsBtn->mLabel = gSexyAppBase->GetString("OPTIONS_BUTTON_TEXT", "Options");

	mLevelPaksBtn = new DialogButton(IMAGE_DIALOG_BUTTON, LEVELPAKS_BUTTON_ID, this);
	mLevelPaksBtn->SetFont(FONT_DEFAULT);
	mLevelPaksBtn->mLabel = gSexyAppBase->GetString("PUZZLEPAKS_BUTTON_TEXT", "Game Menu");

	mExitBtn = new DialogButton(IMAGE_DIALOG_BUTTON, EXIT_BUTTON_ID, this);
	mExitBtn->SetFont(FONT_DEFAULT);
	mExitBtn->mLabel = gSexyAppBase->GetString("EXIT_BUTTON_TEXT", "Quit");

	mGameModeBtn = new DialogButton(IMAGE_DIALOG_BUTTON, GAMEMODE_BUTTON_ID, this);
	mGameModeBtn->SetFont(FONT_DEFAULT);
	mGameModeBtn->mLabel = gSexyAppBase->GetString("GAMEMODE_BUTTON_TEXT", "Classic");

	mEditorBtn = new DialogButton(IMAGE_DIALOG_BUTTON, EDITOR_BUTTON_ID, this);
	mEditorBtn->SetFont(FONT_DEFAULT);
	mEditorBtn->mLabel = gSexyAppBase->GetString("EDITOR_BUTTON_TEXT", "Edit");

	mBackgroundColor = Sexy::Color::Black;
	mSourceBackgroundColor = Color(
		gSexyAppBase->GetInteger("GameScreen_BACKGROUND_COLOR_R", 160/5),
		gSexyAppBase->GetInteger("GameScreen_BACKGROUND_COLOR_G", 160/2),
		gSexyAppBase->GetInteger("GameScreen_BACKGROUND_COLOR_B", 160));

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);	

	int labelWidth;
	int labelHeight;
	int top;
	int left;


	// set pause bg tint
	mPauseBGTint->Resize(0,0,Width(),Height());
	mPauseBGTint->SetColor(0,Color(0,0,0,0));

	// set buttons
	labelWidth = gSexyAppBase->GetInteger("DEFAULT_BUTTON_WIDTH", 274);
	labelHeight = gSexyAppBase->GetInteger("DEFAULT_BUTTON_HEIGHT", 40);

	// exit button
	mExitBtn->SetVisible(false);
	mExitBtn->SetDisabled(true);

	//top = gGraphicsScale * gSexyAppBase->GetInteger("EXIT_BUTTON_TOP");
	//left = gGraphicsScale * gSexyAppBase->GetInteger("EXIT_BUTTON_LEFT");

	top = 720;
	left = 772;

	mExitBtn->Resize( left,
		top,
		labelWidth,
		labelHeight );

	mExitBtn->mY = top;
	mExitBtn->mX = left;

	mExitBtn->SetFont(FONT_DEFAULT);
	mExitBtn->mDoFinger = true;
	mExitBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mExitBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	
	mExitBtn->mFrameNoDraw = true;
	mExitBtn->mButtonImage = IMAGE_DIALOG_BUTTON;
	

	// game  button
	mGameModeBtn->SetVisible(false);
	mGameModeBtn->SetDisabled(true);

	top = gGraphicsScale * gSexyAppBase->GetInteger("GAMEMODE_BUTTON_TOP");
	left = gGraphicsScale * gSexyAppBase->GetInteger("GAMEMODE_BUTTON_LEFT");

	mGameModeBtn->Resize( left,
		top,
		gGraphicsScale * gSexyAppBase->GetInteger("GAMEMODE_BUTTON_WIDTH"),
		gGraphicsScale * gSexyAppBase->GetInteger("GAMEMODE_BUTTON_HEIGHT") );

	mGameModeBtn->mY = top;
	mGameModeBtn->mX = left;

	mGameModeBtn->SetFont(FONT_DEFAULT);
	mGameModeBtn->mDoFinger = true;
	mGameModeBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mGameModeBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	
	mGameModeBtn->mFrameNoDraw = true;
	mGameModeBtn->mButtonImage = IMAGE_DIALOG_BUTTON;



	// start game
	mStartGameBtn->SetVisible(false);
	mStartGameBtn->SetDisabled(true);

	top = gGraphicsScale * gSexyAppBase->GetInteger("STARTGAME_BUTTON_TOP", 540 );
	left = (Width()/2) - (labelWidth/2);

	mStartGameBtn->Resize( left, top, labelWidth, labelHeight );

	mStartGameBtn->mY = top;
	mStartGameBtn->mX = left;

	mStartGameBtn->SetFont(FONT_DEFAULT);
	mStartGameBtn->mDoFinger = true;
	mStartGameBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mStartGameBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	
	mStartGameBtn->mFrameNoDraw = true;
	mStartGameBtn->mButtonImage = IMAGE_DIALOG_BUTTON;






	// options button
	mOptionsBtn->SetVisible(false);
	mOptionsBtn->SetDisabled(true);

	// top = gGraphicsScale * gSexyAppBase->GetInteger("OPTIONS_BUTTON_TOP");
	// left = gGraphicsScale * gSexyAppBase->GetInteger("OPTIONS_BUTTON_LEFT");

	top = 680;
	left = 772;

	mOptionsBtn->Resize( left, top, labelWidth, labelHeight	);

	mOptionsBtn->mY = top;
	mOptionsBtn->mX = left;

	mOptionsBtn->SetFont(FONT_DEFAULT);
	mOptionsBtn->mDoFinger = true;
	mOptionsBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mOptionsBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	
	mOptionsBtn->mFrameNoDraw = true;
	mOptionsBtn->mButtonImage = IMAGE_DIALOG_BUTTON;




	// level paks button
	mLevelPaksBtn->SetVisible(false);
	mLevelPaksBtn->SetDisabled(true);

	top = gGraphicsScale * gSexyAppBase->GetInteger("LEVELPAKS_BUTTON_TOP");
	left = gGraphicsScale * gSexyAppBase->GetInteger("LEVELPAKS_BUTTON_LEFT");

	mLevelPaksBtn->Resize( left, top, labelWidth, labelHeight	);

	mLevelPaksBtn->mY = top;
	mLevelPaksBtn->mX = left;

	mLevelPaksBtn->SetFont(FONT_DEFAULT);
	mLevelPaksBtn->mDoFinger = true;
	mLevelPaksBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mLevelPaksBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	
	mLevelPaksBtn->mFrameNoDraw = true;
	mLevelPaksBtn->mButtonImage = IMAGE_DIALOG_BUTTON;



	// editor button
	mEditorBtn->SetColor(ButtonWidget::COLOR_LABEL, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
		192));
	mEditorBtn->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
		gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
		255));	

	mEditorBtn->mFrameNoDraw = true;
	mEditorBtn->mButtonImage = IMAGE_DIALOG_BUTTON;

	mEditorBtn->SetVisible(false);
	mEditorBtn->SetDisabled(true);

	top = gGraphicsScale * gSexyAppBase->GetInteger("EDITOR_BUTTON_TOP");
	left = gGraphicsScale * gSexyAppBase->GetInteger("EDITOR_BUTTON_LEFT");

	int editorbtnwidth = gGraphicsScale * gSexyAppBase->GetInteger("EDITOR_BUTTON_WIDTH");
	int editorbtnheight = gGraphicsScale * gSexyAppBase->GetInteger("EDITOR_BUTTON_HEIGHT");

	mEditorBtn->Resize( left, top, editorbtnwidth, editorbtnheight );

	//mEditorBtn->mY = top;
	//mEditorBtn->mX = left;

	//mEditorBtn->mOverImage = IMAGE_BUTTON_OVER;
	//mEditorBtn->mDownImage = IMAGE_BUTTON_DOWN;
	//mEditorBtn->mButtonImage = IMAGE_BUTTON_NORMAL;

	mEditorBtn->mDoFinger = true;

	// save as filename edit
	mSaveFilenameEdit = new EditWidget(20, this);
	mSaveFilenameEdit->Resize( Width() / 4,
		(Height()/2)-(FONT_DEFAULT->GetHeight()/2),
		Width()/2,
		FONT_DEFAULT->GetHeight() );
	mSaveFilenameEdit->mMaxChars = 32;

	// load filename edit
	mLoadFilenameEdit = new EditWidget(21, this);
	mLoadFilenameEdit->Resize( Width() / 4,
		(Height()/2)-(FONT_DEFAULT->GetHeight()/2),
		Width()/2,
		FONT_DEFAULT->GetHeight() );
	mLoadFilenameEdit->mMaxChars = 32;


	// add widgets to manager
	theWidgetManager->AddWidget(mStartGameBtn);
	theWidgetManager->AddWidget(mOptionsBtn);
	//theWidgetManager->AddWidget(mEditorBtn);
	theWidgetManager->AddWidget(mExitBtn);
	//theWidgetManager->AddWidget(mGameModeBtn);
	//theWidgetManager->AddWidget(mLevelPaksBtn);

	if( gSexyAppBase->GetBoolean("DRAW_INTERFACE_BUTTONS",true) == false)
	{
		mStartGameBtn->SetVisible(false);
		mOptionsBtn->SetVisible(false);
		mEditorBtn->SetVisible(false);
		mStartGameBtn->Move(Width(), Height());
		mOptionsBtn->Move(Width(), Height());
		mEditorBtn->Move(Width(), Height());
	}

	for( int i=SAVELEVEL; i<=BLOCK; i++ )
	{
		mEditorButtons[i-SAVELEVEL] = new ButtonWidget(i,this);
	}

	// match force
	int centerx, centery;
	centerx = gSexyAppBase->GetInteger("BACKGROUND_TILES_LEFT") +
		(gSexyAppBase->GetInteger("BACKGROUND_TILES_WIDTH") / 2);
	centery = gSexyAppBase->GetInteger("BACKGROUND_TILES_TOP") +
		(gSexyAppBase->GetInteger("BACKGROUND_TILES_HEIGHT") / 2);

	mBGTileCenterX = centerx * gGraphicsScale;
	mBGTileCenterY = centery * gGraphicsScale;

	// bg tiles
	if( gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", false) )
	{
		for( int i = 0; i < mBackgroundTileCount; i++ )
		{
			mTiles[i] = new Tile();
			mBackgroundTileTimer[i] = (gSexyAppBase->GetDouble("BACKGROUND_TILES_ANIM_LENGTH")
				/ mBackgroundTileCount) * (float)i;

			int centerx, centery;
			centerx = gSexyAppBase->GetInteger("BACKGROUND_TILES_LEFT") +
				(gSexyAppBase->GetInteger("BACKGROUND_TILES_WIDTH") / 2);
			centery = gSexyAppBase->GetInteger("BACKGROUND_TILES_TOP") +
				(gSexyAppBase->GetInteger("BACKGROUND_TILES_HEIGHT") / 2);

			mBGTileCenterX = centerx * gGraphicsScale;
			mBGTileCenterY = centery * gGraphicsScale;

			mTiles[i]->mPosition.x = centerx;
			mTiles[i]->mPosition.x *= gGraphicsScale;

			mTiles[i]->mPosition.y = centery;
			mTiles[i]->mPosition.y *= gGraphicsScale;

			/*
			mTiles[i]->mPosition.x = gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_X",128) +
				Rand() % gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_MAX_DISTANCE",32) -
				(gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_MAX_DISTANCE",32) / 2);
			mTiles[i]->mPosition.x *= gGraphicsScale;
			mTiles[i]->mPosition.y = gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_Y",512) +
				Rand() % gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_MAX_DISTANCE",32) -
				(gSexyAppBase->GetInteger("BACKGROUND_TILES_FORCE_MAX_DISTANCE",32) / 2);
			mTiles[i]->mPosition.y *= gGraphicsScale;

			mTiles[i]->mVelocity.x = 0;
			mTiles[i]->mVelocity.y = 0;

			int angle = Rand() % 360;
			float angleInRadians = (angle * PI) / 180.0f;

			mTiles[i]->mVelocity.x = 0.2f * (sin( (float)angleInRadians ));
			mTiles[i]->mVelocity.y = 0.2f * (cos( (float)angleInRadians ));
			*/

			mTiles[i]->mColor = i;

			theWidgetManager->AddWidget(mTiles[i]);
			mTiles[i]->mIsActive = false;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::RemovedFromManager(WidgetManager *theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	theWidgetManager->RemoveWidget(mPauseBGTint);

	theWidgetManager->RemoveWidget(mStartGameBtn);
	theWidgetManager->RemoveWidget(mOptionsBtn);
	theWidgetManager->RemoveWidget(mEditorBtn);
	theWidgetManager->RemoveWidget(mExitBtn);
	theWidgetManager->RemoveWidget(mGameModeBtn);
	theWidgetManager->RemoveWidget(mLevelPaksBtn);

	theWidgetManager->RemoveWidget(mMovie);


	if( mMapView != NULL )
	{
		RemoveWidget(mMapView);
	}

	if( mGamePanel != NULL )
	{
		RemoveWidget(mGamePanel);
	}

	if( mNSPGamesLink != NULL )
	{
		if( mNSPGamesLink->mWidgetManager != NULL )
			theWidgetManager->RemoveWidget(mNSPGamesLink);
	}

	if( mSaveFilenameEdit != NULL )
	{
		if( mSaveFilenameEdit->mWidgetManager != NULL )
			theWidgetManager->RemoveWidget(mSaveFilenameEdit);
	}

	if( mLoadFilenameEdit != NULL )
	{
		if( mLoadFilenameEdit->mWidgetManager != NULL )
			theWidgetManager->RemoveWidget(mLoadFilenameEdit);
	}

	if( gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", false) )
	{
		for( int i = 0; i < mBackgroundTileCount; i++ )
		{
			theWidgetManager->RemoveWidget(mTiles[i]);
			delete mTiles[i];
		}
	}

	RemoveEditorButtons();

	if( mDoneLoading )
	{
		DeleteScore();
	}

	theWidgetManager->RemoveWidget(mPuzzlePakItem);

	//if( mPuzzlePakItem != NULL )
	//{
	//	if( mPuzzlePakItem->mWidgetManager != NULL )
	//		theWidgetManager->RemoveWidget(mPuzzlePakItem);
	//}


}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::Draw(Graphics *g)
{
	std::string s;
	int strWidth;
	int x, y;


//	g->SetColor(mBackgroundColor);
//	g->FillRect(0,0,mWidth,mHeight);

	g->SetColorizeImages(true);

	if( mDoneLoading )
	{


		//// Draw Tiles
		//if( gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", false) )
		//{
		//	//g->SetColor(Color(255,255,255,mTitleAlpha / 10));
		//	g->SetColor(Color(255,255,255,mTitleAlpha));

		//	Tile *aTile;

		//	for( int i = 0; i<mBackgroundTileCount; i++ )
		//	{
		//		aTile = mTiles[i];

		//		if( aTile->mColor >= 0 )
		//		{
		//			Rect destRect;
		//			//destRect.mX = (int)aTile->mPosition.x + (52-aTile->mWidth);
		//			//destRect.mY = (int)aTile->mPosition.y + (52-aTile->mHeight);
		//			destRect.mX = (int)aTile->mPosition.x - (aTile->Width()>>1);
		//			destRect.mY = (int)aTile->mPosition.y - (aTile->Height()>>1);
		//			destRect.mWidth = aTile->mWidth;
		//			destRect.mHeight = aTile->mHeight;

		//			g->DrawImageCel(IMAGE_BLOCKS,
		//				destRect,
		//				aTile->mColor,
		//				0);
		//		}
		//	}
		//}

		//if( gSexyAppBase->GetBoolean("DRAW_BACKGROUND_LAYER_3", false) )
		//{
			//g->SetColor(Color(255,255,255,mTitleAlpha));
			//g->DrawImage(IMAGE_BACKGROUND_LAYER_3,
			//	0,
			//	0,
			//	IMAGE_BACKGROUND_LAYER_3->mWidth,
			//	IMAGE_BACKGROUND_LAYER_3->mHeight);


			//// draw smile if happy!
			//if( mHappyLength > 0 )
			//{
			//	switch( Width() )
			//	{
			//	default:
			//	case 1024:
			//		g->DrawImage(IMAGE_smile,
			//			114, 520);
			//		break;
			//	case 800:
			//		g->DrawImage(IMAGE_smile,
			//			90, 406);
			//		break;
			//	case 640:
			//		g->DrawImage(IMAGE_smile,
			//			72, 325);
			//		break;
			//	}
			//}

		//}

		//if( gSexyAppBase->GetBoolean("DRAW_BACKGROUND_LAYER_2", false) )
		//{
		//	g->SetColor(Color(255,255,255,mTitleAlpha));
		//	g->DrawImage(IMAGE_BACKGROUND_LAYER_2,
		//		0,
		//		0,
		//		IMAGE_BACKGROUND_LAYER_2->mWidth,
		//		IMAGE_BACKGROUND_LAYER_2->mHeight);
		//}

		//if( gSexyAppBase->GetBoolean("DRAW_BACKGROUND_LAYER_1", false) )
		//{
		//	g->SetColor(Color(255,255,255,mTitleAlpha));
		//	g->DrawImage(IMAGE_BACKGROUND_LAYER_1,
		//		0,
		//		0,
		//		IMAGE_BACKGROUND_LAYER_1->mWidth,
		//		IMAGE_BACKGROUND_LAYER_1->mHeight);
		//}

		//if( gSexyAppBase->GetBoolean("DRAW_INTERFACE_TEXT",true) )
		//{
			// draw high score
			//g->SetFont(FONT_DEFAULT);

			//// high score
			//s = StrFormat("%d", mHighScore);
			//strWidth = FONT_DEFAULT->StringWidth(s);

			//x = gSexyAppBase->GetInteger("HISCORE_TEXT_LEFT");
			//x += gSexyAppBase->GetInteger("HISCORE_TEXT_WIDTH") / 2;
			//x *= gGraphicsScale;
			//x -= (strWidth/2);

			//y = gSexyAppBase->GetInteger("HISCORE_TEXT_TOP");
			//y += gSexyAppBase->GetInteger("HISCORE_TEXT_HEIGHT") -
			//	(gSexyAppBase->GetInteger("HISCORE_TEXT_HEIGHT") / 2);
			//y *= gGraphicsScale;
			//y += FONT_DEFAULT->GetAscent() / 2;
			////y -= FONT_DEFAULT->GetHeight() / 2;

			////g->SetColor(Color(0, 0, 0, mTitleAlpha/2));
			////g->DrawString(s,x+1,y);
			////g->DrawString(s,x-1,y);
			////g->DrawString(s,x,y+1);
			////g->DrawString(s,x,y-1);

			//g->SetColor(Color(255, 255, 0, mTitleAlpha));
			//g->DrawString(s,x,y);



			//if( mApp->mLevelPak != NULL )
			//{
			//	// level pak name
			//	s = mLevelPakName;

			//	strWidth = FONT_DEFAULT->StringWidth(s);

			//	x = gSexyAppBase->GetInteger("LEVELPAK_TEXT_LEFT");
			//	x += gSexyAppBase->GetInteger("LEVELPAK_TEXT_WIDTH") / 2;
			//	x *= gGraphicsScale;
			//	x -= (strWidth/2);

			//	y = gSexyAppBase->GetInteger("LEVELPAK_TEXT_TOP");
			//	y += gSexyAppBase->GetInteger("LEVELPAK_TEXT_HEIGHT") -
			//		(gSexyAppBase->GetInteger("LEVELPAK_TEXT_HEIGHT") / 2);
			//	y *= gGraphicsScale;
			//	y += FONT_DEFAULT->GetAscent() / 2;
			//	//y -= FONT_DEFAULT->GetHeight() / 2;

			//	//g->SetColor(Color(0, 0, 0, mTitleAlpha/2));
			//	//g->DrawString(s,x+1,y);
			//	//g->DrawString(s,x-1,y);
			//	//g->DrawString(s,x,y+1);
			//	//g->DrawString(s,x,y-1);

			//	g->SetColor(Color(255, 255, 0, mTitleAlpha));
			//	g->DrawString(s,x,y);

			//	// current level
			//	if( mGameMode == Arcade )
			//	{
			//		s = StrFormat("Level %d", mArcadeLevel);
			//	}
			//	else
			//	{
			//		s = StrFormat("Level %d", mApp->mLevelPak->mCurrentLevel);
			//	}

			//	strWidth = FONT_DEFAULT->StringWidth(s);

			//	x = gSexyAppBase->GetInteger("CURRENT_LEVEL_TEXT_LEFT");
			//	x += gSexyAppBase->GetInteger("CURRENT_LEVEL_TEXT_WIDTH") / 2;
			//	x *= gGraphicsScale;
			//	x -= (strWidth/2);

			//	y = gSexyAppBase->GetInteger("CURRENT_LEVEL_TEXT_TOP");
			//	y += gSexyAppBase->GetInteger("CURRENT_LEVEL_TEXT_HEIGHT") -
			//		(gSexyAppBase->GetInteger("CURRENT_LEVEL_TEXT_HEIGHT") / 2);
			//	y *= gGraphicsScale;
			//	y += FONT_DEFAULT->GetAscent() / 2;

			//	//g->SetColor(Color(0, 0, 0, mTitleAlpha/2));
			//	//g->DrawString(s,x+1,y);
			//	//g->DrawString(s,x-1,y);
			//	//g->DrawString(s,x,y+1);
			//	//g->DrawString(s,x,y-1);

			//	g->SetColor(Color(255, 255, 0, mTitleAlpha));
			//	g->DrawString(s,x,y);
			//}

		//// draw small items and locks
		//g->SetColor(Color(255, 255, 255, mTitleAlpha));

		//// item 1
		//if( mHighestLevelPakCompleteNumber < 1 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 60*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_journal_sm, 60*gGraphicsScale, 569*gGraphicsScale);
		//}

		//// item 2
		//if( mHighestLevelPakCompleteNumber < 2 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 95*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_map_sm, 95*gGraphicsScale, 569*gGraphicsScale);
		//}

		//// item 3
		//if( mHighestLevelPakCompleteNumber < 3 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 129*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_compass_sm, 129*gGraphicsScale, 569*gGraphicsScale);
		//}

		//// item 4
		//if( mHighestLevelPakCompleteNumber < 4 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 163*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_navcompass_sm, 163*gGraphicsScale, 569*gGraphicsScale);
		//}

		//// item 5
		//if( mHighestLevelPakCompleteNumber < 5 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 197*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_wheel_sm, 197*gGraphicsScale, 569*gGraphicsScale);

		//}

		//// item 6
		//if( mHighestLevelPakCompleteNumber < 6 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 231*gGraphicsScale, 569*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_anchor_sm, 231*gGraphicsScale, 569*gGraphicsScale);
		//}

		//// item 7
		//if( mHighestLevelPakCompleteNumber < 7 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 60*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_spyglass_sm, 60*gGraphicsScale, 598*gGraphicsScale);
		//}

		//// item 8
		//if( mHighestLevelPakCompleteNumber < 8 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 95*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_lantern_sm, 95*gGraphicsScale, 598*gGraphicsScale);
		//}

		//// item 9
		//if( mHighestLevelPakCompleteNumber < 9 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 129*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_watch_sm, 129*gGraphicsScale, 598*gGraphicsScale);
		//}

		//// item 10
		//if( mHighestLevelPakCompleteNumber < 10 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 163*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_pipe_sm, 163*gGraphicsScale, 598*gGraphicsScale);
		//}

		//// item 11
		//if( mHighestLevelPakCompleteNumber < 11 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 197*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_jug_sm, 197*gGraphicsScale, 598*gGraphicsScale);
		//}

		//// item 12
		//if( mHighestLevelPakCompleteNumber < 12 )
		//{
		//	g->DrawImage(IMAGE_small_lock, 231*gGraphicsScale, 598*gGraphicsScale);
		//}
		//else
		//{
		//	// draw item
		//	g->DrawImage(IMAGE_key_sm, 231*gGraphicsScale, 598*gGraphicsScale);
		//}


	} // end if done loading


	// version string
	// TODO: remove for release
	//s = "v";
	//s += mApp->mProductVersion;
	//g->SetColor(Color(255, 255, 0, 120));
	//g->DrawString(s,10*gGraphicsScale,mHeight - (10*gGraphicsScale));



	g->SetColorizeImages(false);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::LoadingComplete()
{
	mDoneLoading = true;
	mState = STATE_OPEN_MENU;

	//StartAnimation();

	// kill loading progress
	gLoadingProgress.Stop();
	if( gLoadingProgress.mPosition.z > 1.0f )
		gLoadingProgress.mPosition.z = 1.0f;

//	gLoadingProgress.mAcceleration.y = 0.01f;
	gLoadingProgress.mAcceleration.z = -0.002f;


	//// go ahead and brand end game screen
	//Graphics* g = IMAGE_PAK_COMPLETE_END_GAME->GetGraphics();

	//g->DrawImage(IMAGE_BRANDING, 24*gGraphicsScale, 624*gGraphicsScale);

	//if( !mApp->GetBoolean("BRANDING",false)  )
	//{
	//	// add nspgames.com link
	//	mNSPGamesLink = new ImageLinkWidget();

	//	mNSPGamesLink->mLinkTarget = "http://nspgames.com";

	//	mNSPGamesLink->mImage = IMAGE_BRANDING;
	//	mNSPGamesLink->mHoverImage = IMAGE_BRANDING;

	//	mNSPGamesLink->Move(24*gGraphicsScale, 662*gGraphicsScale);

	//	mNSPGamesLink->mMouseVisible = true;
	//	mNSPGamesLink->mDoFinger = true;

	//	mWidgetManager->AddWidget(mNSPGamesLink);
	//	mWidgetManager->BringToFront(mNSPGamesLink);

	//	// check and do random reset if not licensed and no armadillo wrapper found
	//	//if( !IsArmadilloWrapperInstalled() )
	//	//{
	//	//	mRandomReset = true;
	//	//}
	//}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::ButtonDepress(int theId)
{
	LevelPak* levelPak;
	LevelPakDialog* ld;
	OptionsDialog* od;
	Dialog* d;

//	int gs; // for gridsize precalc

	switch( theId )
	{
	case InterfaceButtonIDs::STARTGAME_BUTTON_ID:
		mWidgetManager->BringToBack(mStartGameBtn);
		StartAnimation();
		break;

	case InterfaceButtonIDs::OPTIONS_BUTTON_ID:
		// TODO: Play a sound whenever the options button is depressed/sad.
		Pause(true);
		od = new OptionsDialog(this);
		od->Resize((150*gGraphicsScale),
			(60*gGraphicsScale),
			mWidth - (300*gGraphicsScale),
			mHeight - (120*gGraphicsScale));
		mApp->AddDialog(OptionsDialog::DIALOG_ID, od);
		break;
//
//
//	case InterfaceButtonIDs::EDITOR_BUTTON_ID:
//		if( mApp->mGameBoard->mState == GameBoard::State::WaitingForUser )
//		{
//			CreateEditorButtons();
//			mApp->mGameBoard->mDoFinger = true;
//			mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);
////
//			//mApp->mGameBoard->ChangeToNewTiles();
//			mApp->mGameBoard->mState = GameBoard::State::EditingLevel;
//			mApp->mGameBoard->mStoredState = mApp->mGameBoard->mState;
//
//			mEditTile.mIsActive = true;
//
//			for( int i=0; i<MAX_GRID_SIZE*MAX_GRID_SIZE; i++ )
//			{
//				mApp->mGameBoard->mBoardTiles[i].mDoFinger = true;
//			}
//
//			gSexyAppBase->mWidgetManager->SetFocus(mApp->mGameBoard);
//			mEditorBtn->mLabel = gSexyAppBase->GetString("EDITOR_EXIT_BUTTON_TEXT", "Play");
//
//		}
//		else if( mApp->mGameBoard->mState == GameBoard::State::EditingLevel )
//		{
//			RemoveEditorButtons();
//			mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//			memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
//				&mApp->mGameBoard->myLevel, sizeof(Level));
//
//			for( int i=0; i<MAX_GRID_SIZE*MAX_GRID_SIZE; i++ )
//			{
//				mApp->mGameBoard->mBoardTiles[i].mDoFinger = false;
//			}
//
//			mApp->mGameBoard->mDoFinger = false;
//
//			mApp->mGameBoard->mState = GameBoard::State::WaitingForUser;
//			mApp->mGameBoard->mStoredState = mApp->mGameBoard->mState;
//			mEditTile.mIsActive = false;
//			gSexyAppBase->mWidgetManager->SetFocus(mApp->mGameBoard);
//
//			mEditorBtn->mLabel = gSexyAppBase->GetString("EDITOR_BUTTON_TEXT", "Edit");
//		}
//
//		// Our hyperlink widget was pressed. We want to remove ourselves
//		// and the hyperlink widget, and tell the app to display the
//		// main board and get the game started. 
//		// You might be thinking, "If I delete the title screen and
//		// hyperlink, won't I crash the program?" Yes, you will. That's
//		// why we aren't going to delete them using "delete". We're going
//		// to use SexyAppBase's SafeDeleteWidget method. This places the
//		// widget in a queue that is processed after all widgets and data
//		// get processed, and is then deleted at a time that is safe
//		// and appropriate. We still have to remove ourself and the
//		// hyperlink widget from the WidgetManager. We can easily access
//		// the WidgetManager, as it is a public variable in our game app.
//		//mApp->mWidgetManager->RemoveWidget(this);
//		//mApp->SafeDeleteWidget(this);
//		//mContinueLink = NULL;
//		//mApp->PlaySample(SOUND_CONTINUE);
//
//		//// Now let's tell the game app that it's ok to add the board widget:
//		//mApp->GameScreenIsFinished();
//		break;
//
//
//
//	case InterfaceButtonIDs::LEVELPAKS_BUTTON_ID:
//		Pause(true);
//		ld = new LevelPakDialog(this);
//		ld->Resize((150*gGraphicsScale),
//			(60*gGraphicsScale),
//			mWidth - (300*gGraphicsScale),
//			mHeight - (120*gGraphicsScale));
//		mApp->AddDialog(LevelPakDialog::DIALOG_ID, ld);
//		mWidgetManager->SetFocus(ld);
//		break;
//
	case InterfaceButtonIDs::EXIT_BUTTON_ID:

		Pause(true);

		d = gSexyAppBase->DoDialog(DialogIDs::CONFIRM_QUIT_ID, true, "Quit Playing?", 
			"Do you really want to quit the game? Your progress will be saved.",
			"", Dialog::BUTTONS_YES_NO);

		d->Move( Widget::Left() + (Widget::Width()/2) - (d->Width()/2),
			Widget::Top() + (Widget::Height()/2) - (d->Height()/2) );

		d->mDialogListener = this;  

		//gSexyAppBase->Shutdown();
		break;
//
//	case InterfaceButtonIDs::ADDLEVEL:
//		mApp->mGameBoard->RandomizeBoard(true);
//		break;
//
//	case InterfaceButtonIDs::BLOCK:
//		switch( mEditTile.mIsBlock )
//		{
//		case true:
//			mEditTile.mIsBlock = false;
//			break;
//		case false:
//			mEditTile.mIsBlock = true;
//			break;
//		}
//		break;
//
//	case InterfaceButtonIDs::CLEARLEVEL:
//		mApp->mGameBoard->ClearTiles();
//		break;
//
//	case InterfaceButtonIDs::SPLITLEVELS:
//		mApp->mLevelPak->SaveLevelsToFolder();
//		break;
//
//	case InterfaceButtonIDs::UNSPLITLEVELS:
//		mApp->mLevelPak->LoadLevelsFromFolder();
//
//		levelPak = mApp->mLevelPak;
//		mApp->mGameBoard->LoadLevel(
//			levelPak->mLevels[levelPak->mCurrentLevel]);
//
//		mApp->mGameBoard->ChangeToNewTiles();
//
//		break;
//
//	case InterfaceButtonIDs::NEWLEVELPAK:
//		mSaveFilenameEdit->mEditListener = this;
//		mWidgetManager->AddWidget(mSaveFilenameEdit);
//		mSaveFilenameEdit->SetText("New Level Pak");
//		mWidgetManager->SetFocus(mSaveFilenameEdit);
//		Pause(true);
//		break;
//
//	case InterfaceButtonIDs::PREVLEVEL:
//		mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
//			&mApp->mGameBoard->myLevel, sizeof(Level));
//
//
//		levelPak = mApp->mLevelPak;
//
//		if( levelPak->mCurrentLevel > 1 )
//		{
//			levelPak->mNewCurrentLevel = levelPak->mCurrentLevel - 1;
//			mApp->mGameBoard->LoadLevel(
//				levelPak->mLevels[levelPak->mNewCurrentLevel]);
//			mApp->mGameBoard->ChangeToNewTiles();
//
//			mApp->mGameBoard->MarkDirty();
//		}
//		break;
//
//	case InterfaceButtonIDs::NEXTLEVEL:
//		mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
//			&mApp->mGameBoard->myLevel, sizeof(Level));
//
//		levelPak = mApp->mLevelPak;
//
//		if( levelPak->mCurrentLevel < levelPak->mLevelCount )
//		{
//			levelPak->mNewCurrentLevel = levelPak->mCurrentLevel + 1;
//			mApp->mGameBoard->LoadLevel(
//				levelPak->mLevels[levelPak->mNewCurrentLevel]);
//			mApp->mGameBoard->ChangeToNewTiles();
//		}
//		else
//		{
//			if( levelPak->mCurrentLevel < MAX_LEVEL_COUNT-1 )
//			{
//				levelPak->mLevelCount++;
//				levelPak->mCurrentLevel++;
//				levelPak->mNewCurrentLevel = levelPak->mCurrentLevel;
//				mApp->mGameBoard->LoadLevel(
//					levelPak->mLevels[levelPak->mCurrentLevel]);
//
//				mApp->mGameBoard->ChangeToNewTiles();
//
//			}
//		}
//		mApp->mGameBoard->ChangeToNewTiles();
//
//		break;
//
//	case InterfaceButtonIDs::SAVELEVEL:
//		mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
//			&mApp->mGameBoard->myLevel, sizeof(Level));
//
//		mApp->mLevelPak->mFilename = "LevelPaks\\";
//		mApp->mLevelPak->mFilename += mLevelPakName;
//
//		mApp->mLevelPak->SaveToFile(mApp->mLevelPak->mFilename);
//
//		mApp->mGameBoard->FadeInTiles();
//		break;
//
//	case InterfaceButtonIDs::TILE4:
//		switch( mEditTile.mMatchLevel )
//		{
//		case 3:
//		case 5:
//			mEditTile.mMatchLevel = 4;
//			mEditTile.mNumberOverlay->mDigit = 4;
//			break;
//		case 4:
//			mEditTile.mMatchLevel = 3;
//			mEditTile.mNumberOverlay->mDigit = -1;
//			break;
//		}
//		
//		break;
//
//	case InterfaceButtonIDs::TILE5:
//		switch( mEditTile.mMatchLevel )
//		{
//		case 3:
//		case 4:
//			mEditTile.mMatchLevel = 5;
//			mEditTile.mNumberOverlay->mDigit = 5;
//			break;
//		case 5:
//			mEditTile.mMatchLevel = 3;
//			mEditTile.mNumberOverlay->mDigit = -1;
//			break;
//		}
//		break;
//
	}


}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::StartAnimation(void)
{
//	mMouseVisible = false;

	mAnimateStartTime = 0;
	mAnimateCurrentTime = 0;

	if( mNSPGamesLink != NULL )
	{
		mWidgetManager->RemoveWidget(mNSPGamesLink);
	}

	// clear any floating tiles (was seeing random digits at start)
	for( int i=0; i<100; i++ )
	{
		if( floatingnumber[i].mWidgetManager != NULL )
		{
			floatingnumber[i].mIsActive = false;
		}
	}

	// options button
	mOptionsBtn->mButtonImage = IMAGE_DIALOG_BUTTON;
	mOptionsBtn->SetVisible(true);
	mOptionsBtn->SetDisabled(false);

	mLevelPaksBtn->mButtonImage = IMAGE_DIALOG_BUTTON;
	mLevelPaksBtn->SetVisible(true);
	mLevelPaksBtn->SetDisabled(false);

	// disable editor button
	// mEditorBtn->SetVisible(true);
	// mEditorBtn->SetDisabled(false);

	mExitBtn->mButtonImage = IMAGE_DIALOG_BUTTON;
	mExitBtn->SetVisible(true);
	mExitBtn->SetDisabled(false);


	mState = STATE_PLAYING_GAME;



	//// show item cover f/x
	//ps = mApp->mParticleManager->SpawnPS(
	//	"particles\\item_line_cover_blue.psi",
	//	NULL,
	//	mPuzzlePakItem->Left() + (mPuzzlePakItem->Width() / 2),
	//	mPuzzlePakItem->Top() + (mPuzzlePakItem->Height() / 2)
	//	);

	//if( ps == NULL ) throw( std::exception("Failed to spawn particle system.") );

	//ps2 = mApp->mParticleManager->SpawnPS(
	//	"particles\\item_line_cover_red.psi",
	//	NULL,
	//	mPuzzlePakItem->Left() + (mPuzzlePakItem->Width() / 2),
	//	mPuzzlePakItem->Top() + (mPuzzlePakItem->Height() / 2)
	//	);

	//if( ps2 == NULL ) throw( std::exception("Failed to spawn particle system.") );

	//ps3 = mApp->mParticleManager->SpawnPS(
	//	"particles\\item_line_cover_green.psi",
	//	NULL,
	//	mPuzzlePakItem->Left() + (mPuzzlePakItem->Width() / 2),
	//	mPuzzlePakItem->Top() + (mPuzzlePakItem->Height() / 2)
	//	);

	//if( ps3 == NULL ) throw( std::exception("Failed to spawn particle system.") );


	//// set up animation
	//float x, y;
	//float x1, x2, x3;

	//y = mPuzzlePakItem->Top();

	//ps->mWayPoints.clear();
	//ps2->mWayPoints.clear();
	//ps3->mWayPoints.clear();

	//for( int frame = 0; frame < 200; frame++ )
	//{
	//	x = mPuzzlePakItem->Left() + (mPuzzlePakItem->Width() / 2);
	//	x1 = x + (sin((float)frame / (100.0f / PI))) * (mPuzzlePakItem->Width() / 2);
	//	x2 = x + (sin((float)(frame+66) / (100.0f / PI))) * (mPuzzlePakItem->Width() / 2);
	//	x3 = x + (sin((float)(frame+133) / (100.0f / PI))) * (mPuzzlePakItem->Width() / 2);

	//	ps->mWayPoints.push_back(Point(x1,y));
	//	ps2->mWayPoints.push_back(Point(x2,y));
	//	ps3->mWayPoints.push_back(Point(x3,y));
	//}

	//ps->mPlayTime = 3;
	//ps2->mPlayTime = 3;
	//ps3->mPlayTime = 3;

	//ps->Play();
	//ps2->Play();
	//ps3->Play();


	//SetItemUncoveredPart(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));
	//SetItemUncoveredParticleLine(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));

	//// TODO: reenable hilite?
	////mApp->mParticleManager->SpawnPS(
	////	"particles\\item_cover_hilite.psi",
	////	NULL,
	////	mPuzzlePakItem->Left() + (mPuzzlePakItem->Width() / 2),
	////	mPuzzlePakItem->Top() + (mPuzzlePakItem->Height() / 2)
	////	);





	//// fade in item
	//mPuzzlePakItem->mAcceleration.z = 0.001f;
	//mPuzzlePakItem->mPosition.z = 0.0f;

	//// load all small items
	//mPuzzlePakItems.clear();

	
	gTileManager.Init();
	gActorManager.Init();

	gCurrentMap = new Map();
	gCurrentMap->Load("Shady's Cove");
//	gCurrentMap->Create(256,256);
//	gCurrentMap->mActors[(128*256)+128] = &gActorManager.mActors[0];

	mMapView = new MapView();
	mMapView->mMap = gCurrentMap;
	mMapView->mCenterX = 128;
	mMapView->mCenterY = 128;

	AddWidget(mMapView);

	mGamePanel = new GamePanel();

	AddWidget(mGamePanel);

	mWidgetManager->SetFocus(this);

	//mApp->GetDefaultName(gActorManager.mActors[0].mName);
	gActorManager.mActors[0].mName = "Ethan";


	gTitleLogo.Stop();
	gTitleLogo.mPosition.z = 1.0f;
	gTitleLogo.mAcceleration.y = 1.0f;
	gTitleLogo.mAcceleration.z = -0.01f;




	// DISABLED ARCADE MODE!
//	mGameModeBtn->SetVisible(true);
//	mGameModeBtn->SetDisabled(false);


//	if( gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", true) )
//	{
//		for( int i = 0; i < mBackgroundTileCount; i++ )
//		{
//			mTiles[i]->mIsActive = true;
//		}
//	}

	// PlayMovie(GameScreen::MOVIE_ID::INSTRUCTIONS_MOVIE_ID);

	// ShowInstructionsDialog();
}

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
void GameScreen::Update(void)
{
	static bool gIsFirstTimeDroppingScore = true;
	static bool gIsFirstTimeRaisingScore = true;


	Widget::Update();

	// TODO: change type to optimize
	float theFrac = 1.25;

	mAnimateCurrentTime += theFrac;

	if( mDoneLoading )
	{

		if( mState == STATE_OPEN_MENU )
		{
			mStartGameBtn->mButtonImage = IMAGE_DIALOG_BUTTON;
			mStartGameBtn->SetVisible(true);
			mStartGameBtn->SetDisabled(false);

			mWidgetManager->BringToFront(mStartGameBtn);
		}
		else
		{
			// TODO: bad place
			if( mApp->mLevelPak->mCurrentLevel == mApp->mLevelPak->mLevelCount )
			{
				mEditorButtons[3]->mLabel = "New";
			}
			else
			{
				mEditorButtons[3]->mLabel = "Next";
			}


			if( mCurrentScore < mScore )
			{
				gIsFirstTimeDroppingScore = true;

				if( gIsFirstTimeRaisingScore )
				{
					gIsFirstTimeRaisingScore = false;
					mScoreRaiseStartTime = mAnimateCurrentTime;
				}

				//mCurrentScore += 4.0f * theFrac;
				mCurrentScore += (
					((mAnimateCurrentTime - mScoreRaiseStartTime) * (mAnimateCurrentTime - mScoreRaiseStartTime)))
					* theFrac;

				if( mCurrentScore >= mScore )
				{
					gIsFirstTimeRaisingScore = true;
					mCurrentScore = mScore;
				}

				SetScore();
			}

			if( mCurrentScore > mScore )
			{
				gIsFirstTimeRaisingScore = true;

				if( gIsFirstTimeDroppingScore )
				{
					gIsFirstTimeDroppingScore = false;
					mScoreDropStartTime = mAnimateCurrentTime;
				}

				mCurrentScore -= (
					((mAnimateCurrentTime - mScoreDropStartTime) * (mAnimateCurrentTime - mScoreDropStartTime))
					+ 10) * theFrac;

				if( mCurrentScore < mScore ) mCurrentScore = mScore;
				SetScore();
			}

			if( mTitleAlpha < 255 )
				mTitleAlpha = ((int)(mAnimateCurrentTime)%129)*2;
			if( mTitleAlpha > 255 )
				mTitleAlpha = 255;

			if( mLoadingAlpha > 0 )
				mLoadingAlpha -= theFrac * 4;
			if( mLoadingAlpha < 0 )
				mLoadingAlpha = 0;


			// check for particle move
			if( mPuzzlePakItem != NULL &&
				(mPuzzlePakItem->mCurrentPartUncovered < mPuzzlePakItem->mPartUncovered || 
				mPuzzlePakItem->mCurrentPartUncovered > mPuzzlePakItem->mPartUncovered + 0.001f ) )
			{
				SetItemUncoveredParticleLine( mPuzzlePakItem->mCurrentPartUncovered );
			}

			// update particles
			if( mApp->mParticleManager != NULL )
			{
				mApp->mParticleManager->Update(0.01f);
			}

			if( mHappyLength > 0 )
			{
				mHappyLength -= 10 * theFrac;
				if( mHappyLength < 0 )
					mHappyLength = 0;
			}

		} // end if past opening menu

	} // end if animating (loading complete)
	else
	{
		if( mLoadingAlpha < 255)
			mLoadingAlpha = ((int)(mAnimateCurrentTime)%66)*4;
		if( mLoadingAlpha > 255)
			mLoadingAlpha = 255;
	}

	// update music
	if( mApp->mMusicFadeInTimer > 0.0f )
	{
		mApp->mMusicFadeInTimer -= theFrac;

		if( mApp->mMusicFadeInTimer <= 0.0f )
		{
			mApp->mMusicInterface->FadeIn(0);
		}
	}

	this->MarkAllDirty();

	if( mRandomReset && (((int)mAnimateCurrentTime) % 100) == 0 )
	{
		if( mAnimateCurrentTime > 30000 && (Rand(300) == 0) )
		{
			gSexyAppBase->Popup("This appears to be an unregistered copy of MatchBlox 2 with trial functionality removed. If you believe this to be an error, "
				"please contact NSP Games by email at support@nspgames.com or by visiting http://nspgames.com/contact.php.\r\n\r\n"
				"You can always download an official copy of MatchBlox 2 at http://nspgames.com.");

			gSexyAppBase->Shutdown();
		}
	}


	if( mIsCheckingFor5MinDemoSwitch && (((int)mAnimateCurrentTime) % 100) == 0 )
	{
		char timeleftnow[256]="";
		char timelefttotal[256]="";
		SexyString hours, mins, secs;

		UpdateArmadilloEnvironment();

		// check total time
		if( GetEnvironmentVariable("TIMELEFTTOTAL",timelefttotal, 255) )
		{
			SexyString timeleftotalstring;
			timeleftotalstring = timelefttotal;

			//OutputDebugString("Time left: ");
			//OutputDebugString(timeleftnow);
			//OutputDebugString("\r\n");

			if( timeleftotalstring.length() == 7 )
			{
				hours = timeleftotalstring.substr(0, 1);
				mins = timeleftotalstring.substr(2,2);
				secs = timeleftotalstring.substr(5,2);

				if( hours == "0" && mins == "04" )
				{
						SetArmadilloDefaultKey();
						InstallArmadilloCode("5MINDEMO","0M0MFZ-QCEX74-5J3VY5-WW2KJV-KH178P");
						UpdateArmadilloEnvironment();
						mIsCheckingFor5MinDemoSwitch = false;
						gSexyAppBase->Shutdown();

						//OutputDebugString("Shutdown.\r\n");
				}
			}
			else if( timeleftotalstring.length() == 8 )
			{
				hours = timeleftotalstring.substr(0, 2);
				mins = timeleftotalstring.substr(3,2);
				secs = timeleftotalstring.substr(6,2);

				if( hours == "00" && mins == "04" )
				{
						SetArmadilloDefaultKey();
						InstallArmadilloCode("5MINDEMO","0M0MFZ-QCEX74-5J3VY5-WW2KJV-KH178P");
						UpdateArmadilloEnvironment();
						mIsCheckingFor5MinDemoSwitch = false;
						gSexyAppBase->Shutdown();

						//OutputDebugString("Shutdown.\r\n");
				}
			}
		}


		// check time now
		if( GetEnvironmentVariable("TIMELEFTNOW",timeleftnow, 255) )
		{
			SexyString timeleft;
			timeleft = timeleftnow;

			//OutputDebugString("Time left: ");
			//OutputDebugString(timeleftnow);
			//OutputDebugString("\r\n");

			if( timeleft.length() == 7 )
			{
				hours = timeleft.substr(0, 1);
				mins = timeleft.substr(2,2);
				secs = timeleft.substr(5,2);

				if( hours == "0" && mins == "00" )
				{
					if( secs=="00" )
					{
						SetArmadilloDefaultKey();
						InstallArmadilloCode("5MINDEMO","0M0MFZ-QCEX74-5J3VY5-WW2KJV-KH178P");
						UpdateArmadilloEnvironment();
						mIsCheckingFor5MinDemoSwitch = false;
						gSexyAppBase->Shutdown();

						//OutputDebugString("Shutdown.\r\n");
					}
				}
			}
			else if( timeleft.length() == 8 )
			{
				hours = timeleft.substr(0, 2);
				mins = timeleft.substr(3,2);
				secs = timeleft.substr(6,2);

				if( hours == "00" && mins == "00" )
				{
					if( secs=="00" )
					{
						SetArmadilloDefaultKey();
						InstallArmadilloCode("5MINDEMO","0M0MFZ-QCEX74-5J3VY5-WW2KJV-KH178P");
						UpdateArmadilloEnvironment();
						mIsCheckingFor5MinDemoSwitch = false;
						gSexyAppBase->Shutdown();

						//OutputDebugString("Shutdown.\r\n");
					}
				}
			}
			else
			{
				// do nothing invalid format
				//mIsCheckingFor5MinDemoSwitch = false;

				//OutputDebugString("Invalid Format.");
				//OutputDebugString("\r\n");

			}

		}
		else
		{
			mIsCheckingFor5MinDemoSwitch = false;
		}

	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::Pause(bool p)
{
	if (p)
	{
		// disable controls
		mOptionsBtn->MarkDirty();
		
		// Since when we're paused we don't update each frame, call
		// MarkDirty here so that we ensure the "PAUSED" overlay appears
		
		MarkDirty();
		++gPauseLevel;

		// add PAUSED overlay
		if( gPauseLevel == 1 )
		{
			if( mPauseBGTint )
			{
				mPauseBGTint->mVisible = true;
				mWidgetManager->AddWidget(mPauseBGTint);

				mPauseBGTint->Stop();
				mPauseBGTint->mPosition.z = 0.001f;
				mPauseBGTint->mAcceleration.z = 0.01f;
			}

			if( mApp->mParticleWidget )
			{
				mApp->mParticleWidget->SetVisible(false);
			}
		}
	}
	else
	{
		// enable controls
		// mOptionsBtn->MarkDirty();

		--gPauseLevel;
		if( gPauseLevel < 0 ) gPauseLevel = 0;


		if( gPauseLevel == 0 )
		{
			if( mPauseBGTint )
			{
				//mPauseBGTint->mVisible = true;
				//mWidgetManager->AddWidget(mPauseBGTint);
				mPauseBGTint->Stop();
				mPauseBGTint->mPosition.z = 1.0f;
				mPauseBGTint->mAcceleration.z = -0.01f;
			}

			if( mApp->mParticleWidget )
			{
				mApp->mParticleWidget->SetVisible(true);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////


void GameScreen::LoadPak(std::string theSelectedPakName)
{
	// stop all tiles
	for( int i=0; i<mApp->mGameBoard->mGridSize * mApp->mGameBoard->mGridSize; i++ )
	{
		Tile* aTile = &mApp->mGameBoard->mBoardTiles[i];
		aTile->Stop();
		aTile->mIsSliding = false;
		aTile->mIsActive = false;
		aTile->mPosition.x = aTile->mHomeX;
		aTile->mPosition.y = aTile->mHomeY;
	}

	mPuzzlePakItem->mPosition.z = 1.0f;

	if( theSelectedPakName == "<RANDOM_MODE>")
	{
		StartRandomGame();

		SetItemUncoveredPart(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));
		SetItemUncoveredParticleLine(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));

	}
	else if( theSelectedPakName != "" )
	{
		// reset current level if not the same level pak
		if( mLevelPakName != theSelectedPakName )
			mApp->mLevelPak->mNewCurrentLevel = 1;

		mNewLevelPakName = theSelectedPakName;
		mApp->mLevelPak->mFilename = ".\\levelpaks\\";
		mApp->mLevelPak->mFilename += theSelectedPakName;
		mApp->mLevelPak->LoadFromFile( mApp->mLevelPak->mFilename );

		SetItemUncoveredPart(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));
		SetItemUncoveredParticleLine(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));


		mApp->mGameBoard->LoadLevel(
			mApp->mLevelPak->mLevels[1]);

		//mApp->mGameBoard->FadeOutTiles();
		//mApp->mGameBoard->ChangeToNewTiles();
	}

//	SetItemUncoveredPart( 0.0f );
//	SetItemUncoveredParticleLine( 0.0f );

	// reset current
//	mPuzzlePakItem->mCurrentPartUncovered = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
void GameScreen::OnOptionsClosed()
{
	Pause(false);
}

void GameScreen::OnLevelPaksClosed(std::string theSelectedPakName)
{
	//SexyString highScoreKey;
	//		std::string highestLevelPakComplete;
	//		std::string playerNumber;
	//		std::string regKey;

	//Pause(false);

	//if( theSelectedPakName == "" )
	//{
	//	// do nothing
	//}
	//else if( theSelectedPakName.substr(0, 15) == "<CHANGE_PLAYER>" )
	//{
	//	std::string s;
	//	s = theSelectedPakName.substr(15, theSelectedPakName.length() - 15);


	//	// reset score
	//	mScore = 0;

	//	// save high score
	//	highScoreKey = mLevelPakName;
	//	highScoreKey += ":HighScore";
	//	mApp->RegistryWriteInteger(highScoreKey,mHighScore);

	//		// check for highest level


	//	// save highest level pak completed
	//	// write highest to registry
	//	if( mApp->mLevelPak->mName != "Arcade" &&
	//		mApp->mLevelPak->mName != "Random Puzzles" )
	//	{


	//		playerNumber = "";
	//		playerNumber += mApp->mPlayer.name;


	//		regKey = playerNumber;
	//		regKey += ":CurrentLevelPak";
	//		mApp->RegistryWriteString(regKey,mApp->mLevelPak->mName);
	//		regKey = playerNumber;
	//		regKey += ":CurrentLevel";
	//		mApp->RegistryWriteInteger(regKey,mApp->mLevelPak->mCurrentLevel);



	//		regKey = playerNumber;
	//		regKey += ":HighestLevelPak";


	//		mApp->RegistryReadString(regKey, &highestLevelPakComplete);

	//		if( highestLevelPakComplete == "" )
	//		{
	//			mApp->RegistryWriteString(regKey,mApp->mLevelPak->mName);
	//		}
	//		else
	//		{
	//			std::vector<std::string> levelPakList;

	//			LevelPak::FillLevelPakList( levelPakList );

	//			// find first in list
	//			std::vector<std::string>::iterator aName = levelPakList.begin();

	//			while( aName != levelPakList.end() )
	//			{
	//				if( std::string(*aName) == mApp->mLevelPak->mName )
	//				{
	//					// don't write anything
	//					break;
	//				}
	//				else if( std::string(*aName) == highestLevelPakComplete )
	//				{
	//					mApp->RegistryWriteString(regKey,mApp->mLevelPak->mName);
	//				}

	//				aName++;
	//			}
	//		}

	//	} // and if not arcade or random

	//		std::string currentLevelPak;
	//		int currentLevel;

	//regKey = s;
	//regKey += ":CurrentLevelPak";


	//if( !gSexyAppBase->RegistryReadString(regKey, &currentLevelPak ) )
	//{
	//	currentLevelPak = gSexyAppBase->GetString("DEFAULT_PUZZLE_PAK","How To Play");
	//}


	//regKey = s;
	//regKey += ":CurrentLevel";
	//if( !gSexyAppBase->RegistryReadInteger(regKey, &currentLevel ) )
	//{
	//	currentLevel = 1;
	//}




	//	// load hi score
	//	mHighScore = 0;
	//	highScoreKey = currentLevelPak;
	//	highScoreKey += ":HighScore";
	//	gSexyAppBase->RegistryReadInteger(highScoreKey, &mHighScore );


	//	// stop all tiles
	//	for( int i=0; i<mApp->mGameBoard->mGridSize * mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		Tile* aTile = &mApp->mGameBoard->mBoardTiles[i];
	//		aTile->Stop();
	//		aTile->mIsSliding = false;
	//		aTile->mIsActive = false;
	//		aTile->mPosition.x = aTile->mHomeX;
	//		aTile->mPosition.y = aTile->mHomeY;
	//	}

	//	//// deactivate all sliders
	//	//for( int i=0; i<mApp->mGameBoard->mGridSize*mApp->mGameBoard->mGridSize; i++ )
	//	//{
	//	//	mApp->mGameBoard->mSliderButtons[i].mIsAttractor = false;
	//	//}

	//	LoadPak(currentLevelPak);

	//	// select item
	//	mPuzzlePakItem->mName = mApp->mLevelPak->mItemName;
	//	mPuzzlePakItem->SetImage();

	//	mApp->mLevelPak->mCurrentLevel = 1;
	//	mApp->mLevelPak->mNewCurrentLevel = currentLevel;

	//	mGameMode = GameMode::Classic;
	//	//mLevelPakName = mApp->mLevelPak->mName;

	//	//for( int i=0; i<mApp->mGameBoard->mGridSize; i++ )
	//	//{
	//	//	mFillTiles[i]->mIsActive = false;
	//	//	mFillTiles[i]->mPosition.z = 1.0f;
	//	//}

	//	//mApp->mGameBoard->LoadLevel(
	//	//	mApp->mLevelPak->mLevels[mApp->mLevelPak->mNewCurrentLevel] );
	//	//
	//	//mApp->mGameBoard->FadeOutTiles();
	//	//mApp->mGameBoard->ChangeToNewTiles();
	//	//mGameModeBtn->mLabel = gSexyAppBase->GetString("CLASSIC_GAMEMODE_TEXT", "Classic");


	//	mIsItemUncoverEnabled = true;
	//	ResetItemUncoveredPart();

	//	mApp->mPlayer.name = s;

	//	SetHighestLevelPakCompleteNumber();

	//	SetItemUncoveredPart(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mNewCurrentLevel));


	//}
	//else if( theSelectedPakName == "<ARCADE_MODE>" )
	//{
	//	// reset score
	//	mScore = 0;

	//	// save high score
	//	highScoreKey = mLevelPakName;
	//	highScoreKey += ":HighScore";
	//	mApp->RegistryWriteInteger(highScoreKey,mHighScore);

	//	// load hi score
	//	mHighScore = 0;
	//	highScoreKey = "Arcade";
	//	highScoreKey += ":HighScore";
	//	gSexyAppBase->RegistryReadInteger(highScoreKey, &mHighScore );


	//	// save current level
	//	if( mApp->mLevelPak->mName != "Arcade" &&
	//		mApp->mLevelPak->mName != "Random Puzzles" )
	//	{
	//		playerNumber = "";
	//		playerNumber += mApp->mPlayer.name;


	//		regKey = playerNumber;
	//		regKey += ":CurrentLevelPak";
	//		mApp->RegistryWriteString(regKey,mApp->mLevelPak->mName);
	//		regKey = playerNumber;
	//		regKey += ":CurrentLevel";
	//		mApp->RegistryWriteInteger(regKey,mApp->mLevelPak->mNewCurrentLevel);
	//	}


	//	// stop all tiles
	//	for( int i=0; i<mApp->mGameBoard->mGridSize * mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		Tile* aTile = &mApp->mGameBoard->mBoardTiles[i];
	//		aTile->Stop();
	//		aTile->mIsSliding = false;
	//		aTile->mIsActive = false;
	//		aTile->mPosition.x = aTile->mHomeX;
	//		aTile->mPosition.y = aTile->mHomeY;
	//	}

	//	//// deactivate all sliders
	//	//for( int i=0; i<mApp->mGameBoard->mGridSize*mApp->mGameBoard->mGridSize; i++ )
	//	//{
	//	//	mApp->mGameBoard->mSliderButtons[i].mIsAttractor = false;
	//	//}

	//	mGameMode = GameMode::Arcade;
	//	
	//	mNewLevelPakName = gSexyAppBase->GetString("ARCADE_EASY_MODE_TEXT", "Arcade");
	//	mApp->mLevelPak->mNewCurrentLevel = 1;
	//	mApp->mLevelPak->mCurrentLevel = 1;


	//	for( int i=0; i<mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		mFillTiles[i]->mIsActive = true;
	//		mFillTiles[i]->mPosition.z = 0.5f;
	//	}

	//	mApp->mGameBoard->myLevel.ClearTiles();
	//	mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);

	//	mApp->mGameBoard->SetArcadeSlideInDelay(1);
	//	mArcadeLevel = 1;

	//	mApp->mGameBoard->RandomizeBoard(false);
	//	mApp->mGameBoard->RandomizeFillTiles();

	//	// clear bottom two rows
	//	int gs = mApp->mGameBoard->mGridSize;

	//	for( int i=(gs*gs)-(gs*2); i<(gs*gs); i++ )
	//	{
	//		mApp->mGameBoard->mBoardTiles[i].mIsActive = false;
	//		mApp->mGameBoard->mBoardTiles[i].mColor = -1;
	//	}

	//	mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);

	//	mApp->mGameBoard->mNextArcadeSlideInTime =
	//		mApp->mGameBoard->mTimer;

	//	mApp->mGameBoard->ClearTiles();
	//	mApp->mGameBoard->FadeOutTiles();

	//	// set up item
	//	mPuzzlePakItem->mName = "Unknown";
	//	mPuzzlePakItem->SetImage();

	//	ResetItemUncoveredPart();

	//	SetItemUncoveredPart(-1.0f);
	//	SetItemUncoveredParticleLine(-1.0f);
	//	mIsItemUncoverEnabled = false;

	//}
	//else if( theSelectedPakName == "<RANDOM_MODE>" )
	//{
	//	// reset score
	//	mScore = 0;

	//	// save high score
	//	highScoreKey = mLevelPakName;
	//	highScoreKey += ":HighScore";
	//	mApp->RegistryWriteInteger(highScoreKey,mHighScore);

	//	// load hi score
	//	mHighScore = 0;
	//	highScoreKey = "Random Puzzles";
	//	highScoreKey += ":HighScore";
	//	gSexyAppBase->RegistryReadInteger(highScoreKey, &mHighScore );

	//	// save current level
	//	if( mApp->mLevelPak->mName != "Arcade" &&
	//		mApp->mLevelPak->mName != "Random Puzzles" )
	//	{
	//		playerNumber = "";
	//		playerNumber += mApp->mPlayer.name;


	//		regKey = playerNumber;
	//		regKey += ":CurrentLevelPak";
	//		mApp->RegistryWriteString(regKey,mApp->mLevelPak->mName);
	//		regKey = playerNumber;
	//		regKey += ":CurrentLevel";
	//		mApp->RegistryWriteInteger(regKey,mApp->mLevelPak->mNewCurrentLevel);
	//	}


	//	// stop all tiles
	//	for( int i=0; i<mApp->mGameBoard->mGridSize * mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		Tile* aTile = &mApp->mGameBoard->mBoardTiles[i];
	//		aTile->Stop();
	//		aTile->mIsSliding = false;
	//		aTile->mIsActive = false;
	//		aTile->mPosition.x = aTile->mHomeX;
	//		aTile->mPosition.y = aTile->mHomeY;
	//	}

	//	//// deactivate all sliders
	//	//for( int i=0; i<mApp->mGameBoard->mGridSize*mApp->mGameBoard->mGridSize; i++ )
	//	//{
	//	//	mApp->mGameBoard->mSliderButtons[i].mIsAttractor = false;
	//	//}

	//	StartRandomGame();

	//	ResetItemUncoveredPart();

	//	SetItemUncoveredPart(-1.0f);
	//	SetItemUncoveredParticleLine(-1.0f);
	//	mIsItemUncoverEnabled = false;

	//}
	//else // new classic level pak
	//{
	//	// reset score
	//	mScore = 0;

	//	// save high score
	//	highScoreKey = mLevelPakName;
	//	highScoreKey += ":HighScore";
	//	mApp->RegistryWriteInteger(highScoreKey,mHighScore);

	//	// load hi score
	//	mHighScore = 0;
	//	highScoreKey = theSelectedPakName;
	//	highScoreKey += ":HighScore";
	//	gSexyAppBase->RegistryReadInteger(highScoreKey, &mHighScore );


	//	// stop all tiles
	//	for( int i=0; i<mApp->mGameBoard->mGridSize * mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		Tile* aTile = &mApp->mGameBoard->mBoardTiles[i];
	//		aTile->Stop();
	//		aTile->mIsSliding = false;
	//		aTile->mIsActive = false;
	//		aTile->mPosition.x = aTile->mHomeX;
	//		aTile->mPosition.y = aTile->mHomeY;
	//	}

	//	//// deactivate all sliders
	//	//for( int i=0; i<mApp->mGameBoard->mGridSize*mApp->mGameBoard->mGridSize; i++ )
	//	//{
	//	//	mApp->mGameBoard->mSliderButtons[i].mIsAttractor = false;
	//	//}

	//	LoadPak(theSelectedPakName);

	//	// select item
	//	mPuzzlePakItem->mName = mApp->mLevelPak->mItemName;
	//	mPuzzlePakItem->SetImage();

	//	mApp->mLevelPak->mNewCurrentLevel = 1;

	//	// reset current level if this is the last pak played
	//	std::string lastLevelPak = "";

	//		playerNumber = "";
	//		playerNumber += mApp->mPlayer.name;

	//		regKey = playerNumber;
	//		regKey += ":CurrentLevelPak";
	//		mApp->RegistryReadString(regKey,&lastLevelPak);

	//	if( theSelectedPakName == lastLevelPak )
	//	{
	//		regKey = mApp->mPlayer.name;
	//		regKey += ":CurrentLevel";
	//		if( !mApp->RegistryReadInteger(regKey,&mApp->mLevelPak->mNewCurrentLevel) )
	//		{
	//			mApp->mLevelPak->mNewCurrentLevel = 1;
	//		}
	//	}

	//	mGameMode = GameMode::Classic;
	//	//mLevelPakName = mApp->mLevelPak->mName;

	//	for( int i=0; i<mApp->mGameBoard->mGridSize; i++ )
	//	{
	//		mFillTiles[i]->mIsActive = false;
	//		mFillTiles[i]->mPosition.z = 1.0f;
	//	}

	//	mApp->mGameBoard->LoadLevel(
	//		mApp->mLevelPak->mLevels[mApp->mLevelPak->mNewCurrentLevel] );
	//	
	//	mApp->mGameBoard->FadeOutTiles();
	//	//mApp->mGameBoard->ChangeToNewTiles();
	//	//mGameModeBtn->mLabel = gSexyAppBase->GetString("CLASSIC_GAMEMODE_TEXT", "Classic");


	//	mIsItemUncoverEnabled = true;
	//	ResetItemUncoveredPart();

	//}

}


void GameScreen::StartRandomGame()
{
#define NUM_OF_RANDOM_LEVELS	100

		//mApp->mLevelPak->mName = gSexyAppBase->GetString("RANDOM_MODE_TEXT", "Random Puzzles");
		//// this->mLevelPakName = mApp->mLevelPak->mName;
		//this->mNewLevelPakName = mApp->mLevelPak->mName;
		//mApp->mLevelPak->mLevelCount = NUM_OF_RANDOM_LEVELS;

		//for( int level=0; level<=NUM_OF_RANDOM_LEVELS; level++ )
		//{
		//	int numofcolors = 3 + (level/2);
		//	if( numofcolors > 8 ) numofcolors = 8;

		//	bool doesContainBlocks = false;
		//	if( level % 2 == 0 ) doesContainBlocks = true;

		//	mApp->mGameBoard->RandomizeBoard(doesContainBlocks, numofcolors);
		//	mApp->mGameBoard->SaveLevel(mApp->mLevelPak->mLevels[level]);
		//}

		//mApp->mLevelPak->mCurrentLevel = 1;
		//mApp->mLevelPak->mNewCurrentLevel = 1;


		//mGameMode = GameMode::Classic;
		//mLevelPakName = mApp->mLevelPak->mName;

		//for( int i=0; i<mApp->mGameBoard->mGridSize; i++ )
		//{
		//	mFillTiles[i]->mIsActive = false;
		//	mFillTiles[i]->mPosition.z = 1.0f;
		//}

		//mApp->mGameBoard->LoadLevel(mApp->mLevelPak->mLevels[1]);
		//mApp->mGameBoard->ClearTiles();
		////mApp->mGameBoard->ChangeToNewTiles();
		//mApp->mGameBoard->FadeOutTiles();


		//// set up item
		//mPuzzlePakItem->mName = "Unknown";
		//mPuzzlePakItem->SetImage();

		//SetItemUncoveredPart( 0.0f );

		//if( ps != NULL )
		//{
		//	SetItemUncoveredParticleLine( 0.0f );
		//}
		//mPuzzlePakItem->mCurrentPartUncovered = 0.0f;

}

bool GameScreen::ShowActorStats(int actorNumber)
{
	Event anEvent;
	std::stringstream textStream;

	textStream << gActorManager.mActors[actorNumber].mName << " is feeling well.\n";
	textStream << "Life: " << gActorManager.mActors[actorNumber].mLife << "\n";
	textStream << "Strength: " << gActorManager.mActors[actorNumber].mStrength << "\n";
	textStream << "Balance: " << gActorManager.mActors[actorNumber].mBalance << "\n";
	textStream << "Intellect: " << gActorManager.mActors[actorNumber].mIntellect << "\n";
	textStream << "Faith: " << gActorManager.mActors[actorNumber].mFaith << "\n";
	textStream << "Focus: " << gActorManager.mActors[actorNumber].mFocus << "\n";

	anEvent.mDescription = textStream.str();

	gEventManager.AddEventToQueue(anEvent);

	return true;
}


bool GameScreen::Wait()
{
	gGameProperties.Advance(1);

	Event anEvent;
	std::stringstream textStream;
	switch (rand() % 4)
	{
	default:
	case 0:
		textStream << gActorManager.mActors[0].mName << " waited.";
		break;
	case 1:
		textStream << gActorManager.mActors[0].mName << " paused and pondered something profound.";
		break;
	case 2:
		textStream << gActorManager.mActors[0].mName << " kicked at the ground for awhile.";
		break;
	case 3:
		textStream << gActorManager.mActors[0].mName << " couldn't think of anything to do for a bit.";
		break;
	}
	
	anEvent.mDescription = textStream.str();

	gEventManager.AddEventToQueue(anEvent);


	return true;
}

bool GameScreen::MoveMainActor(Map::Dir theDirection)
{
	bool wasSuccessful = false;

	if( gCurrentMap->MoveActor(mMapView->mCenterX, mMapView->mCenterY, theDirection) )
	{
		wasSuccessful = true;

		switch( theDirection )
		{
		case Map::Dir::North:
			mMapView->mCenterY--;
			break;
		case Map::Dir::South:
			mMapView->mCenterY++;
			break;
		case Map::Dir::West:
			mMapView->mCenterX--;
			break;
		case Map::Dir::East:
			mMapView->mCenterX++;
			break;
		case Map::Dir::Northeast:
			mMapView->mCenterY--;
			mMapView->mCenterX++;
			break;
		case Map::Dir::Southeast:
			mMapView->mCenterY++;
			mMapView->mCenterX++;
			break;
		case Map::Dir::Northwest:
			mMapView->mCenterY--;
			mMapView->mCenterX--;
			break;
		case Map::Dir::Southwest:
			mMapView->mCenterY++;
			mMapView->mCenterX--;
			break;
		}

		// cancel pending action
		mMapView->SetPendingAction(Action::None);

		gGameProperties.Advance(1);

		mApp->PlaySample(SOUND_footstep1);
	}

	return wasSuccessful;
}

void GameScreen::KeyChar(char theChar)
{
	switch( theChar )
	{
	case 'w':
	case '8':
		MoveMainActor(Map::Dir::North);
		break;
	case 's':
	case '2':
		MoveMainActor(Map::Dir::South);
		break;
	case 'a':
	case '4':
		MoveMainActor(Map::Dir::West);
		break;
	case 'd':
	case '6':
		MoveMainActor(Map::Dir::East);
		break;
	case 'q':
	case '7':
		MoveMainActor(Map::Dir::Northwest);
		break;
	case 'e':
	case '9':
		MoveMainActor(Map::Dir::Northeast);
		break;
	case 'c':
	case '3':
		MoveMainActor(Map::Dir::Southeast);
		break;
	case 'z':
	case '1':
		MoveMainActor(Map::Dir::Southwest);
		break;
	case 'r':
		gGameProperties.Advance(3600*4);
		break;
	case 't':
		switch( mMapView->mPendingAction.mActionType )
		{
		default:
			break;
		case Action::None:
			mMapView->SetPendingAction( Action::Talk );
			break;
		case Action::Talk:
			mMapView->SetPendingAction( Action::None );
			break;
		}

		break;
	case 'f':
		switch( mMapView->mPendingAction.mActionType )
		{
		default:
			break;
		case Action::None:
			mMapView->SetPendingAction( Action::Attack );
			break;
		case Action::Attack:
			mMapView->SetPendingAction( Action::None );
			break;
		}

		break;

	case 'E':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mState = STATE_PLAYING_GAME;
			mMapView->mIsEditting = false;
			mMapView->mEditTileType = MapView::EDIT_TILE_TYPE_GROUND;
			break;
		case STATE_PLAYING_GAME:
			mState = STATE_EDIT_MAP;
			mMapView->mIsEditting = true;
			break;
		}
		break;

	case 'G':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mEditTileType = MapView::EDIT_TILE_TYPE_GROUND;
			break;
		}
		break;

	case 'W':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mEditTileType = MapView::EDIT_TILE_TYPE_WALL;
			break;
		}
		break;

	case ' ':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mMap->mWallTiles[mMapView->mSelectedTileNumber] = NULL;
			break;

		case STATE_PLAYING_GAME:
			Wait();
			break;
		}
		break;

	case 'v':
		ShowActorStats(0);
		break;



	//case 'Q':
	//	switch( mState )
	//	{
	//	case STATE_EDIT_MAP:
	//		if( mMapView->mEditorGroundTileNumber == 0 )
	//		{
	//			mMapView->mEditorGroundTileNumber = gTileManager.mGroundTileCount - 1;
	//		}
	//		else
	//		{
	//			mMapView->mEditorGroundTileNumber--;
	//		}
	//		break;
	//	}
	//	break;

	//case 'W':
	//	switch( mState )
	//	{
	//	case STATE_EDIT_MAP:
	//		if( mMapView->mEditorGroundTileNumber == gTileManager.mGroundTileCount-1 )
	//		{
	//			mMapView->mEditorGroundTileNumber = 0;
	//		}
	//		else
	//		{
	//			mMapView->mEditorGroundTileNumber++;
	//		}
	//		break;
	//	}
	//	break;

	case 'S':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mMap->Save();
			break;
		}
		break;

	case 'L':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mMap->Load("New Map");
			break;
		}
		break;

	case '+':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mMap->mID++;
			break;
		}
		break;

	case '-':
		switch( mState )
		{
		case STATE_EDIT_MAP:
			mMapView->mMap->mID--;
			break;
		}
		break;

	}

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameScreen::KeyDown(Sexy::KeyCode theKey)
{
	switch( theKey )
	{
	case KEYCODE_UP:
		MoveMainActor(Map::Dir::North);
		break;
	case KEYCODE_DOWN:
		MoveMainActor(Map::Dir::South);
		break;
	case KEYCODE_LEFT:
		MoveMainActor(Map::Dir::West);
		break;
	case KEYCODE_RIGHT:
		MoveMainActor(Map::Dir::East);
		break;
	case KEYCODE_ESCAPE:
		mMapView->SetPendingAction( Action::None );
		break;
	}
}

void GameScreen::InitScore(void)
{
	//for( int i=0; i<10; i++ )
	//{
	//	mScoreDigits[i] = NULL;
	//	mScoreDigits[i] = new Digit();
	//	mScoreDigits[i]->mImage = IMAGE_NUMBERS;
	//	gSexyAppBase->mWidgetManager->AddWidget(mScoreDigits[i]);
	//	mScoreDigits[i]->mIsActive = true;
	//}
}

void GameScreen::DeleteScore(void)
{
	//for( int i=0; i<10; i++ )
	//{
	//	gSexyAppBase->mWidgetManager->RemoveWidget(mScoreDigits[i]);
	//	delete mScoreDigits[i];
	//}
}

void GameScreen::SetScore(void)
{
	bool isAtEnd = false;
	short int scoreLength;
	char szScore[11];
	
	itoa(mCurrentScore,szScore,10);
	scoreLength = strlen(szScore);
	
	for( int i=0; i<10; i++ )
	{
		mScoreDigits[i]->mDigit = szScore[i] - '0';
		if( szScore[i] == 0 )
		{
			mScoreDigits[i]->mIsActive = true;

			if( i > scoreLength )
			{
				isAtEnd = true;
			}
		}

		if( isAtEnd )
		{
			mScoreDigits[i]->mIsActive = false;
			mScoreDigits[i]->mDigit = -1;
		}
	}

	short int x = this->Width() -
		(int)(( (76.8f*gGraphicsScale) +
		(IMAGE_BOARD_BG->mWidth/2) + 
		((float)mScoreDigits[0]->Width() * ((float)scoreLength/2.0f))));

	short int y = ((gSexyAppBase->GetDouble("BOARD_TILE_WIDTH",51.2f)*gGraphicsScale * 1.5f) / 2.0f) -
		(mScoreDigits[0]->Height() / 2);

	for( int i=0; i<scoreLength; i++ )
	{
		mScoreDigits[i]->mPosition.x = x;
		mScoreDigits[i]->mPosition.y = y;

		x += mScoreDigits[i]->Width();
	}
}


void GameScreen::SetHighScore(int theHighScore)
{
	mHighScore = theHighScore;

	//bool isAtEnd = false;
	//short int scoreLength;
	//char szScore[11];
	//
	//itoa(mCurrentScore,szScore,10);
	//scoreLength = strlen(szScore);
	//
	//for( int i=0; i<10; i++ )
	//{
	//	mScoreDigits[i]->mDigit = szScore[i] - '0';
	//	if( szScore[i] == 0 )
	//	{
	//		mScoreDigits[i]->mIsActive = true;

	//		if( i > scoreLength )
	//		{
	//			isAtEnd = true;
	//		}
	//	}

	//	if( isAtEnd )
	//	{
	//		mScoreDigits[i]->mIsActive = false;
	//		mScoreDigits[i]->mDigit = -1;
	//	}
	//}

	//short int x = this->Width() -
	//	(int)(( (60*gGraphicsScale) +
	//	(IMAGE_BOARD_BG->mWidth*gGraphicsScale/2) + 
	//	((float)mScoreDigits[0]->Width() * ((float)scoreLength/2.0f))));

	//short int y = ((gSexyAppBase->GetInteger("BOARD_TILE_WIDTH",52)*gGraphicsScale * 1.5f) / 2.0f) -
	//	(mScoreDigits[0]->Height() / 2);

	//for( int i=0; i<scoreLength; i++ )
	//{
	//	mScoreDigits[i]->mPosition.x = x;
	//	mScoreDigits[i]->mPosition.y = y;

	//	x += mScoreDigits[i]->Width();
	//}
}

void GameScreen::AddScore(int amount)
{
	mScore += amount;
	if( mHighScore < mScore ) mHighScore = mScore;

	if( mGameMode == GameMode::Arcade )
	{
		if( (mScore/mArcadeNextLevelGap)+1 > mArcadeLevel )
		{
			mArcadeLevel = (mScore/mArcadeNextLevelGap)+1;
			mApp->mGameBoard->SetArcadeSlideInDelay(mArcadeLevel);

			if( (mArcadeLevel > 0) && ((mArcadeLevel%10) == 0))
			{
				// play happy sound and start over
				mApp->mMusicInterface->StopAllMusic();
				mApp->mMusicInterface->FadeIn(0,0,0.0008,false);
				mApp->PlaySample(SOUND_LEVEL_UP);

				mApp->mGameBoard->AddNewFillColor();

				ResetArcadeBoard();
			}
			else
			{
				// mApp->mGameBoard->RandomizeFillTiles();
			}
		}
	}

	mHappyLength += amount * 20;

//	mApp->debugStream << "Score: " << mScore << std::endl;

}


void GameScreen::RemoveBackgroundTile(int theTileNumber)
{
	if( mTiles[theTileNumber] != NULL && gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", false) == true)
	{
		mTiles[theTileNumber]->mColor = -1;
	}
}

void GameScreen::AddBackgroundTile(int theTileNumber, int theTileColor, int theTileSize)
{
	if( mTiles[theTileNumber] != NULL && gSexyAppBase->GetBoolean("SHOW_BACKGROUND_TILES", false) == true)
	{
		mTiles[theTileNumber]->mColor = theTileColor;
		mTiles[theTileNumber]->mWidth = theTileSize;
		mTiles[theTileNumber]->mHeight = theTileSize;
	}

}

void GameScreen::CreateEditorButtons(void)
{
	mWidgetManager->AddWidget(&mEditTile);

	for( int i=SAVELEVEL; i<=BLOCK; i++ )
	{
		switch( i)
		{
		case InterfaceButtonIDs::ADDLEVEL:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Random";
			break;

		case InterfaceButtonIDs::BLOCK:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Block";
			break;

		case InterfaceButtonIDs::CLEARLEVEL:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Clear";
			break;

		case InterfaceButtonIDs::SPLITLEVELS:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Split";
			mEditorButtons[i-SAVELEVEL]->SetDisabled(true);
			break;

		case InterfaceButtonIDs::UNSPLITLEVELS:
			mEditorButtons[i-SAVELEVEL]->mLabel = "UnSplit";
			mEditorButtons[i-SAVELEVEL]->SetDisabled(true);
			break;

		case InterfaceButtonIDs::NEWLEVELPAK:
			mEditorButtons[i-SAVELEVEL]->mLabel = "NewPak";
			break;

		case InterfaceButtonIDs::SAVELEVEL:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Save";
			break;

		case InterfaceButtonIDs::NEXTLEVEL:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Next";
			break;

		case InterfaceButtonIDs::PREVLEVEL:
			mEditorButtons[i-SAVELEVEL]->mLabel = "Prev";
			break;

		case InterfaceButtonIDs::TILE4:
			mEditorButtons[i-SAVELEVEL]->mLabel = "4";
			break;

		case InterfaceButtonIDs::TILE5:
			mEditorButtons[i-SAVELEVEL]->mLabel = "5";
			break;
		}

		mEditorButtons[i-SAVELEVEL]->SetColor(ButtonWidget::COLOR_LABEL, Color(64,64,64,255));
		mEditorButtons[i-SAVELEVEL]->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color::Black);	
		//mEditorButtons[i-SAVELEVEL]->SetFont(FONT_DEFAULT);

		mEditorButtons[i-SAVELEVEL]->SetVisible(true);
		mEditorButtons[i-SAVELEVEL]->SetDisabled(false);

		mEditorButtons[i-SAVELEVEL]->Resize(	0,
			0,
			64 * gGraphicsScale,
			48 * gGraphicsScale );

		//mEditorButtons[i-SAVELEVEL]->mOverImage = IMAGE_DIALOG_BUTTON;
		//mEditorButtons[i-SAVELEVEL]->mDownImage = IMAGE_DIALOG_BUTTON;
		//mEditorButtons[i-SAVELEVEL]->mButtonImage = IMAGE_DIALOG_BUTTON;

		mEditorButtons[i-SAVELEVEL]->mDoFinger = true;
		mEditorButtons[i-SAVELEVEL]->mY = (mHeight/2)
			- (mEditorButtons[i-SAVELEVEL]->Height() * 6)
			+ (mEditorButtons[i-SAVELEVEL]->Height() * (i-SAVELEVEL));
		mEditorButtons[i-SAVELEVEL]->mX = Width() - (mEditorButtons[i-SAVELEVEL]->Width() * 1.5f);

		mWidgetManager->AddWidget(mEditorButtons[i-SAVELEVEL]);
	}

	// enable edit selection tiles
	for( int i=0; i<8; i++ )
	{
		mFillTiles[i]->mIsActive = true;
		mFillTiles[i]->mColor = i;
		mFillTiles[i]->mDoFinger = true;
	}

}

void GameScreen::RemoveEditorButtons(void)
{
	mWidgetManager->RemoveWidget(&mEditTile);

	for( int i=0; i<11; i++ )
	{
		mWidgetManager->RemoveWidget(mEditorButtons[i]);
	}

	// disable edit selection tiles
	for( int i=0; i<8; i++ )
	{
		if( mFillTiles[i] != NULL )
			mFillTiles[i]->mIsActive = false;
	}

}

void GameScreen::EditWidgetText(int theId, const std::string& theString)
{
	mWidgetManager->RemoveWidget(&mEditTile);

	switch( theId )
	{
	case 20:
		mNewLevelPakName = theString;

		mApp->mLevelPak->mName = theString;
	
		mLevelPakName = theString;
		mApp->mLevelPak->mFilename = ".\\levelpaks\\";
		mApp->mLevelPak->mFilename += theString;

		mWidgetManager->RemoveWidget(mSaveFilenameEdit);

		mApp->mLevelPak->mLevelCount = 1;
		mApp->mLevelPak->mCurrentLevel = 1;

		for( int i=0; i<MAX_LEVEL_COUNT; i++ )
		{
			mApp->mLevelPak->mLevels[i].mGridSize = 10;
			mApp->mLevelPak->mLevels[i].ClearTiles();
		}


		Pause(false);

		mApp->mGameBoard->myLevel.ClearTiles();
		mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);

		mApp->mGameBoard->ChangeToNewTiles();
		break;
	}
}

bool GameScreen::AllowText(int theId, const std::string &theText)
{
	for( int i=0; i<(int)theText.size(); i++ )
	{
		if( (theText[i] >= 'a' && theText[i] <= 'z') ||
			(theText[i] >= 'A' && theText[i] <= 'Z') ||
			(theText[i] >= '0' && theText[i] <= '9') ||
			(theText[i] == ' ') )
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void GameScreen::PlayMovie(MOVIE_ID id)
{
	Graphics* g = NULL;

	// delete old temp image
	if( mTempImage != NULL )
	{
		delete(mTempImage);
		mTempImage = NULL;
	}

	switch( id )
	{
	case MOVIE_ID::INSTRUCTIONS_MOVIE_ID:
		mMovie->mType = Movie::MovieType::TEXT;
		mMovie->mText = "Slide blox by clicking the arrows at the edge of the board.";
		mMovie->mLength = 500;

//		mMovie->mText = "Click on the controls at the edge of the board to slide all the blox in "
//			"that row to the edge.\nMatch 3 blox of the same color to remove them from the board.\n"
//			"Clear the entire board to win!";
		break;

	case MOVIE_ID::LOSE_MOVIE_ID:
		mMovie->mType = Movie::MovieType::FLOATINGIMAGE;
		mMovie->mImage = IMAGE_YOU_LOSE;

		mMovie->mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_MAIN_TEXT_FORCE_MASS",0.0001f);

		if( mApp->isMonkeyingAllLevels )
			mMovie->mLength = 0;
		else
			mMovie->mLength = 640;
		break;

	case MOVIE_ID::WIN_MOVIE_ID:
		
		mIsEndGame = true;

		mMovie->mType = Movie::MovieType::FLOATINGIMAGE;

		mTempImage = new MemoryImage((MemoryImage&)(*IMAGE_PAK_COMPLETE_END_GAME));
		mMovie->mImage = mTempImage;

		// add level pak text
		g = new Graphics(mMovie->mImage);

		g->DrawImage(IMAGE_PAK_COMPLETE_END_GAME_TEXT,
			172*gGraphicsScale,
			193*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_TEXT->mWidth*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_TEXT->mHeight*gGraphicsScale);

		
		mMovie->mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_WIN_MOVIE_FORCE_MASS",0.0001f);

		mMovie->mHasContinueButton = true;

		mMovie->mContinueButtonX = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_X",400) * gGraphicsScale;
		mMovie->mContinueButtonY = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_Y",678) * gGraphicsScale;


		if( mApp->isMonkeyingAllLevels )
			mMovie->mLength = 0;
		else
			mMovie->mLength = 640;

		// reset to 1 now so if player closes bad current level isn't saved
		mApp->mLevelPak->mNewCurrentLevel = 1;

		break;


	case MOVIE_ID::WIN_2_MOVIE_ID:
		
		mIsEndGame = true;

		mMovie->mType = Movie::MovieType::FLOATINGIMAGE;

		mTempImage = new MemoryImage((MemoryImage&)(*IMAGE_PAK_COMPLETE_END_GAME));
		mMovie->mImage = mTempImage;

		// add level pak text
		g = new Graphics(mMovie->mImage);

		// add puzzle pak item
		g->DrawImage(IMAGE_PAK_COMPLETE_END_GAME_2_TEXT,
			172*gGraphicsScale,
			193*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_2_TEXT->mWidth*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_2_TEXT->mHeight*gGraphicsScale);


		

		mMovie->mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_WIN_MOVIE_FORCE_MASS",0.0001f);

		mMovie->mHasContinueButton = true;

		mMovie->mContinueButtonX = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_X",400) * gGraphicsScale;
		mMovie->mContinueButtonY = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_Y",678) * gGraphicsScale;


		if( mApp->isMonkeyingAllLevels )
			mMovie->mLength = 0;
		else
			mMovie->mLength = 640;

		// reset to 1 now so if player closes bad current level isn't saved
		mApp->mLevelPak->mNewCurrentLevel = 1;

		break;


	case MOVIE_ID::WIN_3_MOVIE_ID:
		
		mIsEndGame = true;

		mMovie->mType = Movie::MovieType::FLOATINGIMAGE;

		mTempImage = new MemoryImage((MemoryImage&)(*IMAGE_PAK_COMPLETE_END_GAME));
		mMovie->mImage = mTempImage;

		// add level pak text
		g = new Graphics(mMovie->mImage);

		// add puzzle pak item
		g->DrawImage(IMAGE_PAK_COMPLETE_END_GAME_3_TEXT,
			172*gGraphicsScale,
			193*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_3_TEXT->mWidth*gGraphicsScale,
			IMAGE_PAK_COMPLETE_END_GAME_3_TEXT->mHeight*gGraphicsScale);




		mMovie->mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_WIN_MOVIE_FORCE_MASS",0.0001f);

		mMovie->mHasContinueButton = true;

		mMovie->mContinueButtonX = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_X",400) * gGraphicsScale;
		mMovie->mContinueButtonY = gSexyAppBase->GetInteger("CONTINUE_BUTTON_ENDGAME_Y",678) * gGraphicsScale;


		if( mApp->isMonkeyingAllLevels )
			mMovie->mLength = 0;
		else
			mMovie->mLength = 640;

		// reset to 1 now so if player closes bad current level isn't saved
		mApp->mLevelPak->mNewCurrentLevel = 1;


		break;



	case MOVIE_ID::WIN_PAK_MOVIE_ID:
		mMovie->mType = Movie::MovieType::FLOATINGIMAGE;

		// show correct picture
		std::vector<std::string> theList;

		LevelPak::FillLevelPakList(theList);
		std::vector<std::string>::iterator aName = theList.begin();

		int levelPakNumber = 1;

		while( aName != theList.end() )
		{
			if( *aName == mLevelPakName )
				break;
					
			aName++;
			levelPakNumber++;
		}

		mTempImage = new MemoryImage((MemoryImage&)(*IMAGE_PAK_COMPLETE_HOW_TO_PLAY));
		mMovie->mImage = mTempImage;

		// add level pak text
		Graphics* g = new Graphics(mMovie->mImage);

		// cel split isn't right if scaling!
		// do it manually :>
		Rect srcRect(0,
			((float)(levelPakNumber-1)) * ((float)IMAGE_PAK_TEXT->mHeight / 16.0f),
			IMAGE_PAK_TEXT->mWidth,
			(float)IMAGE_PAK_TEXT->mHeight / 16.0f);

		g->DrawImage(IMAGE_PAK_TEXT, 100*gGraphicsScale, 585*gGraphicsScale,
			srcRect);
		

		// add puzzle pak item
		g->DrawImage(mPuzzlePakItem->mLargeImage,
			252*gGraphicsScale,
			280*gGraphicsScale);


		// set properties
		mMovie->mHasContinueButton = true;
		mMovie->mContinueButtonX = gSexyAppBase->GetInteger("CONTINUE_BUTTON_X",396) * gGraphicsScale;
		mMovie->mContinueButtonY = gSexyAppBase->GetInteger("CONTINUE_BUTTON_Y",686) * gGraphicsScale;


		mMovie->mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_WIN_MOVIE_FORCE_MASS",0.0001f);

		if( mApp->isMonkeyingAllLevels )
			mMovie->mLength = 0;
		else
			mMovie->mLength = 640;


		// reset to 1 now so if player closes bad current level isn't saved
		mApp->mLevelPak->mNewCurrentLevel = 1;

		break;
	}

	mMovie->Resize(0,0,Width(),Height());

	// TODO: load movie from XML based on MOVIE_ID

	mMovie->mMovieTimer = 0;

	mApp->mWidgetManager->AddWidget(mMovie);
}

void GameScreen::StopMovie()
{
	mApp->mWidgetManager->RemoveWidget(mMovie);
}

void GameScreen::OnMovieEnd()
{
	StopMovie();

//	switch( mGameMode )
//	{
//	case Classic:
//		//mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);
//
//		// restart on auto try again (hokey check for image pointer)
//		if( mMovie->mImage == IMAGE_YOU_LOSE )
//		{
//			mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);
//
//			mApp->mGameBoard->FadeOutTiles();
//
//			SetItemUncoveredPart(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));
////			SetItemUncoveredParticleLine(mApp->mLevelPak->GetPartCompleteToLevel(mApp->mLevelPak->mCurrentLevel));
//
//			if( !mApp->mGameBoard->mIsAutoFadeIn )
//			{
//				mApp->mGameBoard->ChangeToNewTiles();
//				mApp->mGameBoard->mIsAutoFadeIn = true;
//			}
//
//		}
//		else if( mLevelPakName == "Lost Treasures" && mIsShowingEndGame )
//		{
//			// show special end game screen before MatchBlox bonus levels
//			PlayMovie(GameScreen::MOVIE_ID::WIN_MOVIE_ID);
//			mIsShowingEndGame = false;
//
//			mScore = 0;
//		}
//		else
//		{
//			mApp->mGameBoard->LoadLevel(mApp->mGameBoard->myLevel);
//
//			// select item
//			mPuzzlePakItem->mName = mApp->mLevelPak->mItemName;
//			mPuzzlePakItem->SetImage();
//
//			SetItemUncoveredPart(0.0f);
//			SetItemUncoveredParticleLine(0.0f);
//
//			if( !mApp->mGameBoard->mIsAutoFadeIn )
//			{
//				mApp->mGameBoard->ChangeToNewTiles();
//				mApp->mGameBoard->mIsAutoFadeIn = true;
//			}
//
//			mScore = 0;
//
//
//		}
//
//		//if( !mApp->mGameBoard->mIsAutoFadeIn )
//		//{
//		//	mApp->mGameBoard->ChangeToNewTiles();
//		//	mApp->mGameBoard->mIsAutoFadeIn = true;
//		//}
//		break;
//
//	case Arcade:
//
//		// restart on auto try again (hokey check for image pointer)
//		if( mMovie->mImage == IMAGE_YOU_LOSE )
//		{
//			// reset arcade properties
//			mArcadeLevel = 1;
//			mApp->mGameBoard->SetArcadeSlideInDelay(1);
//
//			ResetArcadeBoard();
//
//			// save high score
//			if( mScore > mHighScore )
//			{
//				SetHighScore(mScore);
//			}
//			mScore = 0;
//		
//		}
//		break;
//	}
}

void GameScreen::LoseGame()
{
	mHappyLength = 0;


	mApp->mMusicInterface->FadeOut(0,false,0.02f);
	mApp->mMusicFadeInTimer = 400.0f;

	mApp->PlaySample(SOUND_GAME_OVER_RESTART);

	PlayMovie(GameScreen::MOVIE_ID::LOSE_MOVIE_ID);
}


void GameScreen::ShowInstructionsDialog()
{
	// instructions dialog
	std::string text =
		"Move the colored blox by activating the arrows at the edges of the board. "
		"Match three or more blox of the same color to remove them from the board. "
		"Clear the board of all colored blox to advance to the next puzzle.\n\n"
		"Stone blox cannot be moved and never match other blox. "
		"A block with a number must be matched with at least that many tiles to be removed.\n\n"
		"Want some fast paced action? Click on the CLASSIC button to switch to "
		"Arcade mode. You can also try other puzzles by selecting LEVEL PAKS.";
		//"Here the blox push onto the board and come increasingly faster. Again use "
		//"the arrow keys at the edges of the board to move the blox around and make matches. Be "
		//"careful! Once a full column of blox reach the top of the board, its game over!";

	bool isShowingInstructions = true;
	gSexyAppBase->RegistryReadBoolean("", &isShowingInstructions);

	if( mApp->isMonkeyingAllLevels ) isShowingInstructions = false;

	if( isShowingInstructions )
	{
		Dialog *aDialog = gSexyAppBase->DoDialog(INSTRUCTIONS_DIALOG_ID,
			true,
			gSexyAppBase->GetString("INSTRUCTIONS_DIALOG_TITLE_TEXT", "Instructions"),
			text,
			gSexyAppBase->GetString("CLOSE_DIALOG_TEXT", "Close"),
			Dialog::BUTTONS_FOOTER);

		// set fonts
		aDialog->SetHeaderFont(FONT_DEFAULT);
		aDialog->SetLinesFont(FONT_DEFAULT);
		aDialog->SetButtonFont(FONT_DEFAULT);

		aDialog->SetColor(Dialog::COLOR_HEADER, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_A",255)
			));

		aDialog->SetColor(Dialog::COLOR_LINES, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
			gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
			));

		aDialog->SetColor(Dialog::COLOR_FOOTER, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_A",255)
			));

		aDialog->SetColor(Dialog::COLOR_BUTTON_TEXT, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_A",255)
			));

		aDialog->SetColor(Dialog::COLOR_BUTTON_TEXT_HILITE, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_A",255)
			));
		
		aDialog->SetColor(Dialog::COLOR_BKG, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_A",255)
			));

		aDialog->SetColor(Dialog::COLOR_OUTLINE, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_A",255)
			));


		int x, y, width, height;
		width = Width() * 0.9f;
		height = Height() * 0.8f;
		x = Left() + (Width() >> 1)	- (width >> 1);
		y = Top() + (Height() >> 1)	- (height >> 1);

		aDialog->Resize(x,y,width,height);

		aDialog->WaitForResult();
	}

}



void GameScreen::ResetArcadeBoard()
{
//	GameApp* mApp;
//	mApp = (GameApp*)gSexyAppBase;
//
//	Level tempLevel;
//	mApp->mGameBoard->SaveLevel(tempLevel);
//
//	int numColors = ((mArcadeLevel)/10) + 3;
//	if( numColors > 8 ) numColors = 8;
//
////	mApp->debugStream << "ResetArcadeBoard: numColors" << numColors << std::endl;
//
//	mApp->mGameBoard->RandomizeBoard(false, numColors);
//
//	mApp->mGameBoard->RandomizeFillTiles();
//
//	// clear bottom two rows
//	int gs = mApp->mGameBoard->mGridSize;
//
//	for( int i=(gs*gs)-(gs*2); i<(gs*gs); i++ )
//	{
//		mApp->mGameBoard->mBoardTiles[i].mIsActive = false;
//		mApp->mGameBoard->mBoardTiles[i].mColor = -1;
//	}
//
//	mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//
//
//	mApp->mGameBoard->mNextArcadeSlideInTime =
//		mApp->mGameBoard->mTimer;
//
//	mApp->mGameBoard->SaveLevel(mApp->mGameBoard->myLevel);
//
//
//	// reset level from templevel
//	int gridsizesquared = tempLevel.mGridSize * tempLevel.mGridSize;
//
//	for( int tileNo = 0; tileNo < gridsizesquared; tileNo++ )
//	{
//		mApp->mGameBoard->mBoardTiles[tileNo].mColor = tempLevel.mTiles[tileNo];
//		mApp->mGameBoard->mBoardTiles[tileNo].mMatchLevel = tempLevel.mMatchLevel[tileNo];
//		mApp->mGameBoard->mBoardTiles[tileNo].mIsBlock = tempLevel.mBlock[tileNo];
//		if( mApp->mGameBoard->mBoardTiles[tileNo].mMatchLevel == 3 )
//		{
//			mApp->mGameBoard->mBoardTiles[tileNo].mNumberOverlay->mDigit = -1;
//			mApp->mGameBoard->mBoardTiles[tileNo].mNumberOverlay->mIsActive = false;
//		}
//		else
//		{
//			mApp->mGameBoard->mBoardTiles[tileNo].mNumberOverlay->mDigit = 
//				mApp->mGameBoard->mBoardTiles[tileNo].mMatchLevel;
//
//			mApp->mGameBoard->mBoardTiles[tileNo].mNumberOverlay->mIsActive = true;
//		}
//		
//
//		if( mApp->mGameBoard->mBoardTiles[tileNo].mColor >= 8 ) throw std::exception("BAD TILE COLOR");
//
//		if( mApp->mGameBoard->mBoardTiles[tileNo].mColor != -1 )
//		{
//			mApp->mGameBoard->mBoardTiles[tileNo].mIsActive = true;
//		}
//		else
//		{
//			mApp->mGameBoard->mBoardTiles[tileNo].mIsActive = false;
//		}
//	}
//
//	mApp->mGameBoard->FadeOutTiles();
//
//	SetItemUncoveredPart(0.0f);
//	SetItemUncoveredParticleLine( 0.0f );
}



void GameScreen::ResetItemUncoveredPart()
{
	mPuzzlePakItem->mPartUncovered = 0;
	mPuzzlePakItem->mCurrentPartUncovered = 0;
	SetItemUncoveredParticleLine(0);
}


void GameScreen::SetItemUncoveredPart(float part)
{
	if( mIsItemUncoverEnabled )
	{
		mPuzzlePakItem->mPartUncovered = part;
	}
}


void GameScreen::SetItemUncoveredParticleLine(float part)
{
	if( mIsItemUncoverEnabled )
	{
		// ensure valid particle systems
		if( ps != NULL &&
			ps2 != NULL &&
			ps3 != NULL )
		{
			float y;

			if( part > 0.99f || part < -0.01f )
			{
				// hide offscreen
				y = -200 - (float)mPuzzlePakItem->Height();
			}
			else
			{
				y = ((float)mPuzzlePakItem->Height() * part) +
					mPuzzlePakItem->Top();
			}

			std::vector<Sexy::Point>::iterator i;

			for( i = ps->mWayPoints.begin(); i < ps->mWayPoints.end(); i++ )
			{
				i->mY = y;
			}

			for( i = ps2->mWayPoints.begin(); i < ps2->mWayPoints.end(); i++ )
			{
				i->mY = y;
			}

			for( i = ps3->mWayPoints.begin(); i < ps3->mWayPoints.end(); i++ )
			{
				i->mY = y;
			}

		}
	}

}

void GameScreen::DialogButtonPress(int theDialogId, int theButtonId)
{
	switch( theDialogId )
	{
	case DialogIDs::CONFIRM_QUIT_ID:
		switch ( theButtonId )
		{
		case Dialog::ID_YES:
			gSexyAppBase->Shutdown();
			break;

		case Dialog::ID_NO:
			gSexyAppBase->KillDialog(DialogIDs::CONFIRM_QUIT_ID);
			Pause(false);
			break;
		}

		break;

	case DialogIDs::CONFIRM_PAUSE_ID:
		gSexyAppBase->KillDialog(DialogIDs::CONFIRM_PAUSE_ID);
		Pause(false);
		break;
	}
}


void GameScreen::SetHighestLevelPakCompleteNumber()
{
	// start at -1 (how to play doesn't count)
	mHighestLevelPakCompleteNumber = -1;

	// load order
	std::vector<std::string> levelPakList;

	LevelPak::FillLevelPakList( levelPakList );

	// show list
	std::vector<std::string>::iterator aName = levelPakList.begin();

	// only show unlocked paks
	std::string highestLevelPak;
	std::string regKey;
	regKey = "";
	regKey += mApp->mPlayer.name;
	regKey += ":HighestLevelPak";
	gSexyAppBase->RegistryReadString(regKey, &highestLevelPak);

	bool areLevelsLocked = false;

	while( aName != levelPakList.end() )
	{
		if( std::string(*aName) == highestLevelPak ||
			highestLevelPak == "" )
		{
			break;
		}

		mHighestLevelPakCompleteNumber++;

		aName++;
	}
}

