#include "GameApp.h"
#include "GameScreen.h"
#include "GameBoard.h"
#include "OptionsDialog.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/ResourceManager.h"
#include "../SexyAppFramework/BassMusicInterface.h"
#include "../SexyAppFramework/Dialog.h"

// #include "../SexyAppFramework/SWTri.h"

#include "ParticleWidget.h"

#include "Level.h"

#include "Res.h"
#include "Globals.h"

#include <fstream>


// mem debugging
//#define SEXY_MEMTRACE
//#include "memmgr.h"

using namespace Sexy;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::GameApp()
{
	mProdName = "Epic";
	mProductVersion = "1.0.0";
	mTitle = mProdName;
	mRegKey = "Epic";

	// read player name
	mPlayer.name = "Player 1";
	RegistryReadString("CurrentPlayer",&mPlayer.name);

	// set to 1024x768
	mWidth = 1024;
	mHeight = 768;

	//gSexyAppBase->RegistryReadInteger("ScreenWidth", &mWidth );
	//gSexyAppBase->RegistryReadInteger("ScreenHeight", &mHeight );
	//if( mWidth < 480 ) mWidth = 480;
	//if( mHeight < 360 ) mHeight = 360;
	// TODO: Cap?

	gGraphicsScale = (float)mWidth / (float)SOURCE_GRAPHICS_WIDTH;
	gPauseLevel = 0;

	mGamePauseDialog = NULL;

	mDebugKeysEnabled = true;

	// By setting this to true, the framework will automatically check to see
	// if hardware acceleration can be turned on. This doesn't guarantee that it
	// WILL be turned on, however. Some cards just aren't compatible or have
	// known issues. Also, cards with less than 8MB of video RAM aren't supported.
	// There are ways to override the 3D enabled settings, which we will discuss
	// in a later demo. As a side note, if you want to see if you app is
	// running with 3D acceleration, first enable debug keys by pressing
	// CTRL-ALT-D and then press F9. To toggle 3D on/off, press F8. That is just
	// for testing purposes.
	mAutoEnable3D = true;

	// Enable smooth motion via UpdateF
	mVSyncUpdates = true;

	// init game specific members
	mGameBoard = NULL;
	mGameScreen = NULL;

	mParticleManager = NULL;
	mParticleWidget = NULL;

	mLevelPak = NULL;

	mMusicFadeInTimer = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
GameApp::~GameApp()
{	
	//if (mGameBoard != NULL)
	//{
	//	mWidgetManager->RemoveWidget(mGameBoard);
	//	delete mGameBoard;
	//}

	if( mParticleManager != NULL )
	{
		mParticleManager->KillAll();
		delete mParticleManager;
	}

	if( mParticleWidget != NULL )
	{
		mWidgetManager->RemoveWidget(mParticleWidget);
		delete mParticleWidget;
	}


	if (mGameScreen != NULL)
	{
		if( mLevelPak != NULL )
		{
			// save player number
			RegistryWriteString("CurrentPlayer",mPlayer.name);

			// save hi score
			SexyString highScoreKey;
			highScoreKey = mGameScreen->mLevelPakName;
			highScoreKey += ":HighScore";
			RegistryWriteInteger(highScoreKey,mGameScreen->mHighScore);

			// write highest to registry
			if( mGameScreen->mLevelPakName != "Arcade" &&
				mGameScreen->mLevelPakName != "Random Puzzles" )
			{
				// check for highest level
				std::string highestLevelPakComplete;
				std::string playerNumber;
				std::string regKey;

				playerNumber = "";
				playerNumber += mPlayer.name;


				regKey = playerNumber;
				regKey += ":CurrentLevelPak";
				RegistryWriteString(regKey,mLevelPak->mName);
				regKey = playerNumber;
				regKey += ":CurrentLevel";
				RegistryWriteInteger(regKey,mLevelPak->mNewCurrentLevel);



				regKey = playerNumber;
				regKey += ":HighestLevelPak";


				RegistryReadString(regKey, &highestLevelPakComplete);

				if( highestLevelPakComplete == "" )
				{
					RegistryWriteString(regKey,mLevelPak->mName);
				}
				else
				{
					std::vector<std::string> levelPakList;

					LevelPak::FillLevelPakList( levelPakList );

					// find first in list
					std::vector<std::string>::iterator aName = levelPakList.begin();

					while( aName != levelPakList.end() )
					{
						if( std::string(*aName) == mLevelPak->mName )
						{
							// don't write anything
							break;
						}
						else if( std::string(*aName) == highestLevelPakComplete )
						{
							RegistryWriteString(regKey,mLevelPak->mName);
						}

						aName++;
					}
				}
			}

		delete mLevelPak;
		}


		mWidgetManager->RemoveWidget(mGameScreen);

		delete mGameScreen;
	}


	mWidgetManager->RemoveWidget(&gTitleLogo);
	mWidgetManager->RemoveWidget(&gLoadingProgress);

	// Using "" deletes ALL resources
	mResourceManager->DeleteResources("");


	if( mLevelPak != NULL )
	{

	}


	if( mMonkeyFile.is_open() ) mMonkeyFile.close();

//	if( debugStream.is_open() ) debugStream.close();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::Init()
{

	SexyAppBase::Init();

	mDebugKeysEnabled = GetBoolean("DEBUG_KEYS_ENABLED",false);

	SWTri_AddAllDrawTriFuncs();

	//this->SwitchScreenMode(false);
	//this->EnableCustomCursors(true);

	gSexyAppBase->mVSyncUpdates = true;

	LoadResourceManifest();
	if (!mResourceManager->LoadResources("Init"))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	if (!ExtractInitResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	LoadProperties();

	// To use custom cursors, you have to specify an image resource
	// to use. The CURSOR_ defines indicate which cursor type you
	// want to assign the image to. 
	// CURSOR_POINTER is the normal Windows pointer.
	// CURSOR_HAND is used when mousing over widgets (if mDoFinger is true).
	// CURSOR_DRAGGING is used when a drag operation is happening.
	// CURSOR_TEXT is the icon used when entering text in an edit box.
	SetCursorImage(CURSOR_POINTER, IMAGE_CUSTOM_POINTER);
	SetCursorImage(CURSOR_HAND, IMAGE_CUSTOM_HAND);
	SetCursorImage(CURSOR_DRAGGING, IMAGE_CUSTOM_DRAGGING);
	SetCursorImage(CURSOR_TEXT, IMAGE_CUSTOM_TEXT);

	if (!mResourceManager->LoadResources("GameScreen"))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	if (!ExtractGameScreenResources(mResourceManager))
	{
		mLoadingFailed = true;
		ShowResourceError(true);
		return;
	}

	// prescale images
	MemoryImage* mi;
	Graphics* tempg;

	// must change all code below to add new images to list, sorry i'm in a rush
	Image* ppImagesToScale[] =
	{
		IMAGE_TITLE_LOGO,
		IMAGE_BRANDING
	};

	for( int i=0; i<2; i++ )
	{
		Image* pImage = ppImagesToScale[i];

		if( pImage == NULL ) break;

		mi = new MemoryImage();

		// use create, do not set mwidth/mheight directly:
		mi->Create(
			pImage->GetWidth() * gGraphicsScale,
			pImage->GetHeight() * gGraphicsScale);

		mi->mNumCols = mi->mNumRows = 1;

		tempg = new Graphics(mi);
		tempg->DrawImage(pImage, 0, 0, mi->GetWidth(), mi->GetHeight());

		// replace preloaded image
		// TODO: will this blow up resource loading/unloading?
		// doesn't appear to! Great!
		switch( i )
		{
		case 0:
			IMAGE_TITLE_LOGO = mi;
			break;
		case 1:
			IMAGE_BRANDING = mi;
			break;
		}

		delete tempg;
	}

	// Now let's create and add our title screen to the widget manager
	mGameScreen = new GameScreen(this);
	mGameScreen->Resize(0, 0, mWidth, mHeight);

	// Let's let the title screen initialize it's widgets and data
	// before adding it to the widget manager:
	mGameScreen->Init();

	mWidgetManager->AddWidget( mGameScreen );
	//mWidgetManager->SetFocus( mGameScreen );

	// title logo
	//gTitleLogo.mPosition.x = mWidth / 2 - (IMAGE_TITLE_LOGO->mWidth / 2 * gGraphicsScale);
	//gTitleLogo.mPosition.y = mHeight / 2 - (IMAGE_TITLE_LOGO->mHeight / 1.2f * gGraphicsScale);
	gTitleLogo.mPosition.x = 0;
	gTitleLogo.mPosition.y = 0;
	gTitleLogo.mX = gTitleLogo.mPosition.x;
	gTitleLogo.mY = gTitleLogo.mPosition.y;

	// fade in
	gTitleLogo.mAcceleration.z = 0.1f;
	gTitleLogo.mPosition.z = 0.1f;


	gTitleLogo.mImage = IMAGE_TITLE_LOGO;
	gTitleLogo.mWidth = IMAGE_TITLE_LOGO->mWidth;
	gTitleLogo.mHeight = IMAGE_TITLE_LOGO->mHeight;
	gTitleLogo.mWidthF = IMAGE_TITLE_LOGO->mWidth;
	gTitleLogo.mHeightF = IMAGE_TITLE_LOGO->mHeight;


	//// draw branding or buy now link
	//Graphics* g = IMAGE_TITLE_LOGO->GetGraphics();

	//if( GetBoolean("BRANDING",true)  )
	//{
	//	g->DrawImage(IMAGE_BRANDING, 24*gGraphicsScale, 662*gGraphicsScale);
	//}
	//else
	//{
	//	// show buy now link
	//}


	gTitleLogo.mIsActive = true;

	mWidgetManager->AddWidget(&gTitleLogo);
	mWidgetManager->BringToFront(&gTitleLogo);


	// loading progress

	// prescale loading progress and loading text

	// prescale loading progress images
	mi = new MemoryImage();

	// use create, do not set mwidth/mheight directly:
	mi->Create(
		IMAGE_LOADER_BAR->GetWidth() * gGraphicsScale,
		IMAGE_LOADER_BAR->GetHeight() * gGraphicsScale);

	mi->mNumCols = mi->mNumRows = 1;

	tempg = new Graphics(mi);
	tempg->DrawImage(IMAGE_LOADER_BAR, 0, 0, mi->GetWidth(), mi->GetHeight());

	// replace preloaded image
	// TODO: will this blow up resource loading/unloading?
	// doesn't appear to! Great!
	IMAGE_LOADER_BAR = mi;

	delete tempg;

	// loading text
	mi = new MemoryImage();

	// use create, do not set mwidth/mheight directly:
	mi->Create(
		IMAGE_LOADER_LOADINGTXT->GetWidth() * gGraphicsScale,
		IMAGE_LOADER_LOADINGTXT->GetHeight() * gGraphicsScale);

	mi->mNumCols = mi->mNumRows = 1;

	tempg = new Graphics(mi);
	tempg->DrawImage(IMAGE_LOADER_LOADINGTXT, 0, 0, mi->GetWidth(), mi->GetHeight());

	// replace preloaded image
	// TODO: will this blow up resource loading/unloading?
	// doesn't appear to! Great!
	IMAGE_LOADER_LOADINGTXT = mi;

	delete tempg;


	gLoadingProgress.mImage = IMAGE_LOADER_BAR;
	gLoadingProgress.mWidth = IMAGE_LOADER_BAR->mWidth;
	gLoadingProgress.mHeight = IMAGE_LOADER_BAR->mHeight + IMAGE_LOADER_LOADINGTXT->mHeight;
	//gLoadingProgress.mWidthF = IMAGE_LOADER_BAR->mWidth;
	//gLoadingProgress.mHeightF = IMAGE_LOADER_BAR->mHeight;

//	gLoadingProgress.mX = gLoadingProgress.mPosition.x;
//	gLoadingProgress.mY = gLoadingProgress.mPosition.y;

	gLoadingProgress.mIsActive = true;

	mWidgetManager->AddWidget(&gLoadingProgress);
	mWidgetManager->BringToFront(&gLoadingProgress);

	gLoadingProgress.mPosition.x = (512.0f*gGraphicsScale) - (IMAGE_LOADER_BAR->mWidth/2);
	gLoadingProgress.mPosition.y = 520.0f * gGraphicsScale;
	gLoadingProgress.mPosition.z = 0.01f;
	gLoadingProgress.mAcceleration.z = 0.002f;
	
//	mWidgetManager->BringToFront(mGameScreen);

	// This example just uses 1 standard OGG file instead of a tracker file.
	// Since we aren't cross fading at all, we'll only load it once.
	mMusicInterface->LoadMusic(0, "music/maintheme.mp3");
//	mMusicInterface->PlayMusic(0,0);
	mMusicInterface->FadeIn(0, 0, 0.0005, false);


	// add final loading tasks for resizing
	mNumLoadingThreadTasks = mResourceManager->GetNumResources("Game") + 53;


//	debugStream.open("debug.txt", std::ios_base::out);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadProc()
{
	// prescale images
	Graphics* tempg = NULL;
	MemoryImage* mi = NULL;


	mResourceManager->StartLoadResources("Game");

	while (mResourceManager->LoadNextResource())
	{
		mCompletedLoadingThreadTasks++;
		if (mShutdown)
			return;

		mGameScreen->MarkDirty();
	}

	if (mResourceManager->HadError() || !ExtractGameResources(mResourceManager))
	{		
		ShowResourceError(false);
		mLoadingFailed = true;
	
		return;
	}

	//mResourceManager->StartLoadResources("Hungarr");

	//while (mResourceManager->LoadNextResource())
	//{
	//	mCompletedLoadingThreadTasks++;
	//	if (mShutdown)
	//		return;

	//	mGameScreen->MarkDirty();
	//}

	if (mResourceManager->HadError() || !ExtractGameResources(mResourceManager))
	{		
		ShowResourceError(false);
		mLoadingFailed = true;

		return;
	}


	// prescale images
	// must change all code below to add new images to list, sorry i'm in a rush
	// must in above proc too
	Image* ppImagesToScale[] =
	{
		IMAGE_DIALOG_BUTTON,
		IMAGE_CHECKBOX,
		IMAGE_SLIDER_THUMB,
		IMAGE_SLIDER_TRACK,
		IMAGE_BACKGROUND_LAYER_3,
		IMAGE_DIALOG_LARGE,
		IMAGE_DIALOG_SMALL,
		IMAGE_YOU_LOSE,
		IMAGE_SLIDER_BUTTON,
		IMAGE_BOARD_BG,
		IMAGE_anchor,
		IMAGE_anchor_sm,
		IMAGE_compass,
		IMAGE_compass_sm,
		IMAGE_journal,
		IMAGE_journal_sm,
		IMAGE_jug,
		IMAGE_jug_sm,
		IMAGE_key,
		IMAGE_key_sm,
		IMAGE_lantern,
		IMAGE_lantern_sm,
		IMAGE_map,
		IMAGE_map_sm,
		IMAGE_navcompass,
		IMAGE_navcompass_sm,
		IMAGE_pipe,
		IMAGE_pipe_sm,
		IMAGE_spyglass,
		IMAGE_spyglass_sm,
		IMAGE_watch,
		IMAGE_watch_sm,
		IMAGE_wheel,
		IMAGE_wheel_sm,
		IMAGE_PAK_COMPLETE_HOW_TO_PLAY,
		IMAGE_PAK_COMPLETE_END_GAME,
		IMAGE_lock,
		IMAGE_small_lock,
		IMAGE_smile,
	  IMAGE_lantern_large,
	  IMAGE_spyglass_large,
	  IMAGE_watch_large,
	  IMAGE_anchor_large,
	  IMAGE_map_large,
	  IMAGE_pipe_large,
	  IMAGE_jug_large,
	  IMAGE_wheel_large,
	  IMAGE_journal_large,
	  IMAGE_compass_large,
	  IMAGE_navcompass_large,
	  IMAGE_key_large,
	  IMAGE_unknown_large,
	  IMAGE_PAK_TEXT
	};

	// SET COUNT HERE, AND AT END OF ABOVE FUNCTION!
	for( int i=0; i<53; i++ )
	{
		Image* pImage = ppImagesToScale[i];

		if( pImage == NULL ) break;

		mi = new MemoryImage();

		// use create, do not set mwidth/mheight directly:
		mi->Create(
			pImage->GetWidth() * gGraphicsScale,
			pImage->GetHeight() * gGraphicsScale);

		mi->mNumCols = mi->mNumRows = 1;

		tempg = new Graphics(mi);
		tempg->DrawImage(pImage, 0, 0, mi->GetWidth(), mi->GetHeight());

		// replace preloaded image
		// TODO: will this blow up resource loading/unloading?
		// doesn't appear to! Great!
		switch( i )
		{
		case 0:
			IMAGE_DIALOG_BUTTON = mi;
			break;
		case 1:
			IMAGE_CHECKBOX = mi;
			break;
		case 2:
			IMAGE_SLIDER_THUMB = mi;
			break;
		case 3:
			IMAGE_SLIDER_TRACK = mi;
			break;
		case 4:
			IMAGE_BACKGROUND_LAYER_3 = mi;
			break;
		case 5:
			IMAGE_DIALOG_LARGE = mi;
			break;
		case 6:
			IMAGE_DIALOG_SMALL = mi;
			break;
		case 7:
			IMAGE_YOU_LOSE = mi;
			break;

		case 8:
			IMAGE_SLIDER_BUTTON = mi;
			IMAGE_SLIDER_BUTTON->mNumCols = 4;
			IMAGE_SLIDER_BUTTON->mNumRows = 2;
			break;			
		case 9:
			IMAGE_BOARD_BG = mi;
			break;			

		case 10:	IMAGE_anchor = mi;		break;
		case 11:	IMAGE_anchor_sm = mi;		break;
		case 12:	IMAGE_compass = mi;		break;
		case 13:	IMAGE_compass_sm = mi;		break;
		case 14:	IMAGE_journal = mi;		break;
		case 15:	IMAGE_journal_sm = mi;		break;
		case 16:	IMAGE_jug = mi;		break;
		case 17:	IMAGE_jug_sm = mi;		break;
		case 18:	IMAGE_key = mi;		break;
		case 19:	IMAGE_key_sm = mi;		break;
		case 20:	IMAGE_lantern = mi;		break;
		case 21:	IMAGE_lantern_sm = mi;		break;
		case 22:	IMAGE_map = mi;		break;
		case 23:	IMAGE_map_sm = mi;		break;
		case 24:	IMAGE_navcompass = mi;		break;
		case 25:	IMAGE_navcompass_sm = mi;		break;
		case 26:	IMAGE_pipe = mi;		break;
		case 27:	IMAGE_pipe_sm = mi;		break;
		case 28:	IMAGE_spyglass = mi;		break;
		case 29:	IMAGE_spyglass_sm = mi;		break;
		case 30:	IMAGE_watch = mi;		break;
		case 31:	IMAGE_watch_sm = mi;		break;
		case 32:	IMAGE_wheel = mi;		break;
		case 33:	IMAGE_wheel_sm = mi;		break;


		case 34:	IMAGE_PAK_COMPLETE_HOW_TO_PLAY = mi;		break;
		case 35:	IMAGE_PAK_COMPLETE_END_GAME = mi;		break;

		case 36:	IMAGE_lock = mi;		break;
		case 37:	IMAGE_small_lock = mi;	break;

		case 38:	IMAGE_smile = mi;	break;


	  case 39:	IMAGE_lantern_large = mi;	break;
	  case 40:	IMAGE_spyglass_large = mi;	break;
	  case 41:	IMAGE_watch_large = mi;	break;
	  case 42:	IMAGE_anchor_large = mi;	break;
	  case 43:	IMAGE_map_large = mi;	break;
	  case 44:	IMAGE_pipe_large = mi;	break;
	  case 45:	IMAGE_jug_large = mi;	break;
	  case 46:	IMAGE_wheel_large = mi;	break;
	  case 47:	IMAGE_journal_large = mi;	break;
	  case 48:	IMAGE_compass_large = mi;	break;
	  case 49:	IMAGE_navcompass_large = mi;	break;
	  case 50:	IMAGE_key_large = mi;	break;
	  case 51:	IMAGE_unknown_large = mi;	break;

		  case 52:	IMAGE_PAK_TEXT = mi;
			  IMAGE_PAK_TEXT->mNumRows = 16;
			  break;

		}

		delete tempg;

		mCompletedLoadingThreadTasks++;
	}

	//// prescale dialog buttons
	//mi = new MemoryImage();

	//// use create, do not set mwidth/mheight directly:
	//mi->Create(
	//	IMAGE_DIALOG_BUTTON->GetWidth() * gGraphicsScale,
	//	IMAGE_DIALOG_BUTTON->GetHeight() * gGraphicsScale);

	//mi->mNumCols = mi->mNumRows = 1;

	//tempg = new Graphics(mi);
	//tempg->DrawImage(IMAGE_DIALOG_BUTTON, 0, 0, mi->GetWidth(), mi->GetHeight());

	//// replace preloaded image
	//// TODO: will this blow up resource loading/unloading?
	//// doesn't appear to! Great!
	//IMAGE_DIALOG_BUTTON = mi;

	//delete tempg;


	//// prescale dialog buttons
	//mi = new MemoryImage();

	//// use create, do not set mwidth/mheight directly:
	//mi->Create(
	//	IMAGE_CHECKBOX->GetWidth() * gGraphicsScale,
	//	IMAGE_CHECKBOX->GetHeight() * gGraphicsScale);

	//mi->mNumCols = mi->mNumRows = 1;

	//tempg = new Graphics(mi);
	//tempg->DrawImage(IMAGE_CHECKBOX, 0, 0, mi->GetWidth(), mi->GetHeight());

	//// replace preloaded image
	//// TODO: will this blow up resource loading/unloading?
	//// doesn't appear to! Great!
	//IMAGE_CHECKBOX = mi;

	//delete tempg;


	// prescale images
	//Sexy::ResourceManager::ResMap::iterator mImageItr;

	//for (mImageItr=mResourceManager->mImageMap.begin(); mImageItr != mResourceManager->mImageMap.end(); ++mImageItr)
	//{
	//	Sexy::ResourceManager::ImageRes *aRes = (Sexy::ResourceManager::ImageRes*)mImageItr->second;
	//	if ((DDImage*) aRes->mImage != NULL)
	//	{
	//		mi = new MemoryImage();

	//		// use create, do not set mwidth/mheight directly:
	//		mi->Create(
	//			aRes->mImage->GetWidth() * gGraphicsScale,
	//			aRes->mImage->GetHeight() * gGraphicsScale);

	//		mi->mNumCols = mi->mNumRows = 1;

	//		tempg = new Graphics(mi);
	//		tempg->DrawImage(aRes->mImage, 0, 0, mi->GetWidth(), mi->GetHeight());

	//		// replace preloaded image
	//		// TODO: will this blow up resource loading/unloading?
	//		// doesn't appear to! Great!
	//		aRes->mImage = mi;

	//		delete tempg;
	//	}
	//}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LoadingThreadCompleted()
{
	// Let the base app class also know that we have completed
	SexyAppBase::LoadingThreadCompleted();

	if (mLoadingFailed)
		return;

	//// check for monkey all levels
	//// TODO: change to isRecordingLevelData
	//isMonkeyingAllLevels = gSexyAppBase->GetBoolean("MONKEY_ALL_LEVELS", false );

	//if( isMonkeyingAllLevels )
	//{
	//	std::string monkeyLine;
	//	monkeyLine = "Pak Name, Level, Clicks To Win, Times Lost\r\n";

	//	mMonkeyFile.open("MonkeyFile.csv", std::ios_base::out );
	//	mMonkeyFile.write(monkeyLine.c_str(), monkeyLine.length());
	//	mMonkeyFile.close();
	//}

//	// create game board
//	mGameBoard = new GameBoard(this);
//
//	mGameBoard->mWidth = (int)((float)614.4 * gGraphicsScale);
//	mGameBoard->mHeight = (int)((float)614.4 * gGraphicsScale);
//
//	// store home position
//	mGameBoard->mHomeX = mWidth - ((float)(IMAGE_BOARD_BG->mWidth)) -
//		(mHeight - (float)(IMAGE_BOARD_BG->mHeight)) / 2;
//	mGameBoard->mX = mGameBoard->mHomeX;
//
//	mGameBoard->mHomeY = (mHeight / 2) - (IMAGE_BOARD_BG->mHeight / 2);
//	mGameBoard->mY = mGameBoard->mHomeY;
//
//	mGameBoard->mPosition.x = mGameBoard->mX;
//	mGameBoard->mPosition.y = mGameBoard->mY;
//
//	mGameBoard->mImage = IMAGE_BOARD_BG;
//	mGameBoard->mIsActive = true;
//
//	mGameBoard->mGameScreen = mGameScreen;
//
////	mWidgetManager->AddWidget(mGameBoard);
//
//
//	mGameBoard->MarkDirty();
//
//	mGameBoard->mParent = mGameScreen;

	// never want an empty board
	//mGameBoard->RandomizeBoard(true, 4);

	// create particle manager
	mParticleManager = new HGE::hgeParticleManager;

	mParticleWidget = new ParticleWidget();
	mParticleWidget->SetVisible(true);
	mParticleWidget->Resize(0,0,mGameScreen->Width(),mGameScreen->Height());
	mParticleWidget->mMouseVisible = false;
	mWidgetManager->AddWidget(mParticleWidget);
	mParticleWidget->MarkDirty();

//	mWidgetManager->SetFocus(mGameBoard);

	// set up level paks
	mLevelPak = new LevelPak();

	// in case default levelpak can't be loaded
	mLevelPak->mLevelCount = 1;

	//for( int i=0; i<MAX_LEVEL_COUNT; i++ )
	//{
	//	//mGameBoard->RandomizeBoard(true, 4);
	//	mGameBoard->SaveLevel(mLevelPak->mLevels[i]);
	//}

	std::string	levelPakPath = ".\\levelpaks\\";
	std::string currentLevelPak;

	std::string regKey;
	regKey = mPlayer.name;
	regKey += ":CurrentLevelPak";


	gSexyAppBase->RegistryReadString(regKey, &currentLevelPak );

	// TODO: VALIDATE PAK NAME!

//	// TODO: load item from file or set random or arcade item
//	mGameScreen->mPuzzlePakItem = new PuzzlePakItem();
//	mWidgetManager->AddWidget(mGameScreen->mPuzzlePakItem);
//	mWidgetManager->BringToFront(mGameScreen->mPuzzlePakItem);
//	mGameScreen->mPuzzlePakItem->mIsActive = true;
////	mGameScreen->mPuzzlePakItem->mPosition.z = 1.0f;
//
//	if(currentLevelPak == gSexyAppBase->GetString("RANDOM_MODE_TEXT", "Random Puzzles"))
//	{
//		mGameScreen->LoadPak("<RANDOM_MODE>");
//
//		// select item
//		mGameScreen->mPuzzlePakItem->mName = "Unknown";
//		mGameScreen->mPuzzlePakItem->SetImage();
//	}
//	else
//	{
//		if(currentLevelPak == "") currentLevelPak = gSexyAppBase->GetString("DEFAULT_PUZZLE_PAK","How To Play");
//
//		levelPakPath += currentLevelPak;
//		mLevelPak->LoadFromFile(levelPakPath);
//
//		// select item
//		mGameScreen->mPuzzlePakItem->mName = mLevelPak->mItemName;
//		mGameScreen->mPuzzlePakItem->SetImage();
//
//	}
//
//
//	mGameScreen->mLevelPakName = currentLevelPak;
//	mGameScreen->mNewLevelPakName = mGameScreen->mLevelPakName;
//	
//	regKey;
//	regKey = mPlayer.name;
//	regKey += ":CurrentLevel";
//	gSexyAppBase->RegistryReadInteger(regKey, &(mLevelPak->mCurrentLevel) );
//	if( mLevelPak->mCurrentLevel > mLevelPak->mLevelCount ) mLevelPak->mCurrentLevel = 1;
//
//	mLevelPak->mNewCurrentLevel = mLevelPak->mCurrentLevel;
//
//
//
//	mGameBoard->LoadLevel(
//		mLevelPak->mLevels[mLevelPak->mCurrentLevel]);
//	mGameBoard->ChangeToNewTiles();
//
//	// set up score
//	mGameScreen->mScore = 0;
//	mGameScreen->mHighScore = 0;
//
//	SexyString highScoreKey;
//	highScoreKey = mLevelPak->mName;
//	highScoreKey += ":HighScore";
//
//	gSexyAppBase->RegistryReadInteger(highScoreKey, &(mGameScreen->mHighScore) );
//	mGameScreen->mCurrentScore = 0;
//
//	mGameScreen->InitScore();
//	mGameScreen->SetScore();

	mGameScreen->LoadingComplete();
	mGameScreen->MarkDirty();

	mWidgetManager->BringToFront(&gTitleLogo);
	mWidgetManager->BringToFront(&gLoadingProgress);

	//if( mGameScreen != NULL )
	//{
	//	if( mGameScreen->mNSPGamesLink != NULL )
	//	{
	//		mWidgetManager->BringToFront(mGameScreen->mNSPGamesLink);
	//	}
	//}

	//// testing
	//if( isMonkeyingAllLevels )
	//{
	//	for( __int64 i = 0; i < 10000000000L; i++ )
	//	{
	//		mWidgetManager->UpdateFrame();
	//		mWidgetManager->UpdateFrameF(1.0f);
	//	}
	//}
	//// end testing


}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::GameScreenIsFinished()
{
	mGameScreen = NULL;
	mResourceManager->DeleteResources("GameScreen");

//	mGameBoard->Resize(10, 10, mWidth-10, mHeight-10);
//	mWidgetManager->SetFocus(mGameBoard);

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Dialog* GameApp::NewDialog(int theDialogId, bool isModal, const std::string& theDialogHeader, 
						   const std::string& theDialogLines, const std::string& theDialogFooter, int theButtonMode)
{
	// Rather than dupliate a lengthy explanation, check out the top of DemoDialog.cpp for a complete description
	// of what all the parameters and functions are, in Demo5.
	Dialog* d = new Dialog(NULL, IMAGE_DIALOG_BUTTON, 1.0f, theDialogId, isModal,
		theDialogHeader, theDialogLines, theDialogFooter, theButtonMode);

	d->SetButtonFont(FONT_DEFAULT);
	d->SetLinesFont(FONT_DEFAULT);
	d->SetHeaderFont(FONT_DEFAULT);

	d->SetColor(Dialog::COLOR_HEADER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_A",255)
		));

	d->SetColor(Dialog::COLOR_LINES, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
		));

	d->SetColor(Dialog::COLOR_FOOTER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_A",255)
		));

	d->SetColor(Dialog::COLOR_BUTTON_TEXT, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_A",255)
		));

	d->SetColor(Dialog::COLOR_BUTTON_TEXT_HILITE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_A",255)
		));
	
	d->SetColor(Dialog::COLOR_BKG, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_A",255)
		));

	d->SetColor(Dialog::COLOR_OUTLINE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_A",255)
		));

	d->mContentInsets = Insets(48*gGraphicsScale, 20*gGraphicsScale, 48*gGraphicsScale, 32*gGraphicsScale);
	d->mSpaceAfterHeader = 24*gGraphicsScale;

	d->mComponentImage = IMAGE_DIALOG_SMALL;
	d->Resize(0, 0, IMAGE_DIALOG_SMALL->mWidth, IMAGE_DIALOG_SMALL->mHeight);

	return d;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::LostFocus()
{
	SexyAppBase::LostFocus();

	//if (mGameBoard)
	//	mGameBoard->Pause(true);
	if (mGameScreen)
	{
		if( (mGameScreen->mState == GameScreen::STATE_PLAYING_GAME) && (gPauseLevel <= 0) )
		{
			mGameScreen->Pause(true);

			mGamePauseDialog = gSexyAppBase->DoDialog(GameScreen::DialogIDs::CONFIRM_PAUSE_ID, true, "Game Paused", 
			"Your game has been paused.",
			"Return to Game", Dialog::BUTTONS_FOOTER);

			mGamePauseDialog->Move( mGameScreen->Left() + (mGameScreen->Width()/2) - (mGamePauseDialog->Width()/2),
				mGameScreen->Top() + (mGameScreen->Height()/2) - (mGamePauseDialog->Height()/2) );

			mGamePauseDialog->mDialogListener = mGameScreen;
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::GotFocus()
{
	SexyAppBase::GotFocus();

	//if (mGameBoard)
	//	mGameBoard->Pause(false);
	if (mGameScreen)
	{
		// only un pause if not playing game
		if( mGameScreen->mState != GameScreen::STATE_PLAYING_GAME )
		{
			mGameScreen->Pause(false);
		}
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::ButtonPress(int theId)
{
	if (theId == OptionsDialog::MESSAGE_BOX_ID + 2000)
		KillDialog(theId - 2000);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameApp::PlaySample(int theSoundNum)
{
	SexyAppBase::PlaySample(theSoundNum);
}


void GameApp::GetDefaultName(std::string& userName)
{
// 32767 is highest but we should just fail
// if it's more than 256
// #define INFO_BUFFER_SIZE 32767
#define INFO_BUFFER_SIZE 256
	std::string tempString;

	DWORD i;
	TCHAR  infoBuf[INFO_BUFFER_SIZE];
	DWORD  bufCharCount = INFO_BUFFER_SIZE;

	// assumes ascii
	bufCharCount = INFO_BUFFER_SIZE;
	if( !GetUserName( infoBuf, &bufCharCount ) )
	{
		userName.assign("Roman");
	}
	else
	{
		userName.assign(infoBuf);
	}
}