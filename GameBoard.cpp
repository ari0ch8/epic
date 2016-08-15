
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/SoundInstance.h"
#include "../SexyAppFramework/SoundManager.h"
#include "../SexyAppFramework/MusicInterface.h"

#include "GameBoard.h"
#include "Res.h"
#include "SliderButton.h"
#include "GameScreen.h"

#include "Globals.h"

#include <sstream>

#include <iostream>


// mem debugging
//#define SEXY_MEMTRACE
//#include "memmgr.h"


using namespace Sexy;



GameBoard::GameBoard(GameApp* theApp)
{
	mApp = theApp;
	mDemo = true;

	if( mApp->isMonkeyingAllLevels )
		mIsMonkey = true;
	else
		mIsMonkey = false;

	mIsAddingNewTiles = false;

	mEditTileSelected = NULL;

	mMaxAlpha = 255;

	nextfloatnumber = 0;

	// editor
	mCurrentEditColor = 0;

	// stats
	mNumOfClicks = 0;
	mNumOfTimesLost = 0;


//	mParent = NULL;
	mGameScreen = NULL;

	mState = State::WaitingForUser;
	mStoredState = State::WaitingForUser;

	mGridSize = gSexyAppBase->GetInteger("BOARD_GRID_SIZE", 10);

	// set focused slider button to invalid
	mFocusedSliderNumber = MAX_GRID_SIZE * 4;

	mTileTransitionEndTime = 0.0f;

	CreateBoardObjects();

	// disable
	mLimitToSliderNumber = -1;

	mFadeTimer = 0.0f;
	// TODO: change to false!
	mIsAutoFadeIn = true;

	mCanScore = true;

}

GameBoard::~GameBoard(void)
{
	for( int i = 0; i < MAX_GRID_SIZE*4; i++ )
	{
		RemoveWidget( &mSliderButtons[i] );
	}

	for( int i = 0; i < MAX_BOARD_TILES; i++ )
	{
		mBoardTiles[i].mIsActive = false;
		mBoardTiles[i].SetVisible(false);
		RemoveWidget(&mBoardTiles[i]);
	}

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::Update(void)
{
	// TODO: change type to optimize
	float theFrac = 1.25f;


	int	boardspace = 0;
	
	int levelupframe = 0;
	float fullsize = 0.0f;

	float tempz = 0.0f;

	// Widget::Update();

	Floater::Update();



	if( mIsMonkey )
	{
		if( mState == State::WaitingForUser )
		{
			if( mGameScreen->mMovie->mWidgetManager == NULL )
				mSliderButtons[Rand()%(mGridSize*4)].MouseDown(0,0,1);
		}
	}

	if( mState == State::EditingLevel )
	{
		SetBackgroundTiles();
	}

	mTimer += theFrac;

	switch( mGameScreen->mGameMode )
	{
	case GameScreen::GameMode::Arcade:
		if( mGameScreen->mMovie->mWidgetManager == NULL &&
			mTimer >= mNextArcadeSlideInTime &&
			gPauseLevel == 0 )
		{
			switch( mState )
			{
			case State::WaitingForUser:
				// set next
				while( mNextArcadeSlideInTime <= mTimer )
					mNextArcadeSlideInTime += mArcadeSlideInDelay;

				mState = State::SlidingTiles;
				StartTileShiftByOne(Direction::Up);
				mIsAddingNewTiles = true;
				break;
			}
		}
		break;
	}

	if( mAlpha < mMaxAlpha ) mAlpha += theFrac;
	else mAlpha = mMaxAlpha;

	if( mAlpha > mGameScreen->mTitleAlpha )
		mAlpha = mGameScreen->mTitleAlpha;

	switch( mState )
	{
	case State::SlidingTiles:
		ShiftTiles(theFrac);
		break;

	case State::WaitingForUser:
		break;

	case State::TileDisappear:
		if( mTimer >= mTileTransitionEndTime )
		{
			for( int i=0; i<MAX_BOARD_TILES; i++ )
			{
				mBoardTiles[i].mPosition.z = 0.0f;
			}

			mState = State::FadeOut;
			mFadeTimer = 0.0f;
			mFadeLength = 100.0f;

			Stop();
			mPosition.z = 1.0f;
			mAcceleration.z = -0.005f;
			mAcceleration.y = -0.1f;

			// TODO: Change to win level effect
			//for( int i=0; i<mGridSize*mGridSize; i++ )
			//{
			//	int x = mBoardTiles[i].Left() + (mBoardTiles[i].Width()/2)
			//		+ Left();
			//	int y = mBoardTiles[i].Top() + (mBoardTiles[i].Height()/2)
			//		+ Top();

			//	HGE::hgeParticleSystem *ps = NULL;
			//	ps = mApp->mParticleManager->SpawnPS("particles\\blue_stars.psi", NULL,
			//		x, y);

			//}

			// TODO: check for level up effect (only on WIN!)

			// level up effect
			for( int i=0;i<mGridSize*mGridSize;i++ )
			{
				mBoardTiles[i].mColor = -1;
				mBoardTiles[i].mIsActive = true;
				mBoardTiles[i].mIsBlock = false;
				mBoardTiles[i].mMatchLevel = 3;
				mBoardTiles[i].mPosition.z = 1.0f;
				mBoardTiles[i].mNumberOverlay->mDigit = -1;
			}
		}
		break;

	case State::FadeOut:
		mFadeTimer+=theFrac;

		if( mIsAutoFadeIn && mFadeTimer >= mFadeLength )
		{
			ChangeToNewTiles();
		}
		break;

	case State::FadeIn:
		mFadeTimer+=theFrac;

		// check for done
		if( mFadeTimer >= mFadeLength )
		{
			this->mWidth = IMAGE_BOARD_BG->mWidth;
			this->mHeight = IMAGE_BOARD_BG->mHeight;

			mPosition.z = 1.0f;
			mPosition.y = mHomeY;
			Stop();

			mState = State::TileAppear; // was waiting for user
		}
		break;

	case State::LoadingLevel:
		// check for playing movie, don't load until done
		// TODO: win animation here?
//		if( mGameScreen->mMovie->mWidgetManager == NULL )
//		{
			mApp->mLevelPak->mCurrentLevel = mApp->mLevelPak->mNewCurrentLevel;
			FadeOutTiles();
			//ChangeToNewTiles();
//		}
		break;

	case State::TileAppear:
		if( mTimer >= mTileTransitionEndTime )
		{
			mState = mStoredState;

			for( int i=0; i<MAX_BOARD_TILES; i++ )
			{
				mBoardTiles[i].mPosition.z = 1.0f;
			}

			mState = State::WaitingForUser;
		}

		break;
	}

	if (gPauseLevel > 0)
		return;

	MarkDirty();

	//if (mFilling)
	//{
	//	// If the beams have been released, update the filling of the grid
	//	float amt = mFillSpeed * theFrac;

	//	if (mFillDirection == FILL_RIGHT)
	//		FillRight(amt);		
	//	else if (mFillDirection == FILL_LEFT)
	//		FillLeft(amt);
	//	else if (mFillDirection == FILL_UP)
	//		FillUp(amt);
	//	else if (mFillDirection == FILL_DOWN)
	//		FillDown(amt);

	//	// Check what % full the filled regions are if it's done filling
	//	if (!mFilling)
	//		UpdatePercentComplete();
	//	
	//}

	//// Make the bonus text float upwards and fade it out over time.
	//std::vector<BonusText>::iterator it = mBonusText.begin(); 
	//while (it != mBonusText.end()) 
	//{ 
	//	BonusText* bt = &*it; 
	//	bt->mY -= 1.00f * theFrac; 
	//	bt->mHue = (bt->mHue + 5) % 0xFF; 

	//	if (--bt->mAlpha <= 0) 
	//	{ 
	//		//Totally faded out, remove it 
	//		it = mBonusText.erase(it); 
	//	} 
	//	else
	//		++it; 
	//} 


	//// Move the starfield. If a start gets beyond the screen,
	//// randomly place it offscreen again
	//int i;
	//for (i = 0; i < MAX_STARS; i++)
	//{
	//	Star* s = &mStarField[i];
	//	s->mX += s->mSpeed;
	//	if (s->mX > mWidth)
	//	{
	//		s->mX = -5;
	//		s->mY = Rand() % mHeight;
	//	}
	//}

	//if ((!mMovingLine1.mDone || !mMovingLine2.mDone) && !mGameOverEffect->IsActive())
	//	MoveLines(theFrac);


	//// If we're allowed to show the planets and the game isn't paused and the game
	//// over effect isn't playing, then we can move the planets around
	//if ((!mFadeEffect->HidePlanets() || (gPauseLevel > 0)) && !mGameOverEffect->IsActive())
	//{
	//	// Move the planets
	//	int w = IMAGE_PLANETS->GetCelWidth();
	//	int h = IMAGE_PLANETS->GetCelHeight();

	//	// Instead of playing the explosion sound every time a planet gets destroyed, we'll
	//	// only play it once. That way, if you destroy more than 1 planet in one go, you won't
	//	// hear the same sound played multipled times at once, which would result in this loud,
	//	// hideous, flanging sound.
	//	bool playSound = false;
	//	for (int i = 0; i < mPlanets.size(); i++)
	//	{
	//		Planet* p = &mPlanets[i];

	//		// Again, the timer is used solely for incrementing the animation frames
	//		++p->mTimer;

	//		if (!p->mExploding)
	//		{
	//			if (MovePlanet(p, theFrac))
	//				playSound = true;			// Returns true if the planet is to explode
	//		}
	//		else
	//		{
	//			if ((p->mTimer % p->mExplodeSpeed) == 0)
	//			{
	//				if (++p->mExplodeFrame >= IMAGE_BOMB_RADIAL_DEATH->mNumCols)
	//				{
	//					mPlanets.erase(mPlanets.begin() + i);
	//					--i;
	//				}
	//			}
	//		}
	//	}

	//	if (playSound)
	//		mApp->PlaySample(SOUND_PLANET);
	//}

	//// update and move the particles. When they have reached
	//// their last frame, remove them.
	//for (i = 0; i < mParticles.size(); i++)
	//{
	//	Particle* p = &mParticles[i];
	//	++p->mTimer;

	//	p->mX += p->mVX * theFrac;
	//	p->mY += p->mVY * theFrac;
	//	p->mVY += 0.1f;
	//	if (p->mTimer % 6 == 0)
	//	{
	//		if (++p->mFrame >= IMAGE_PARTICLE_LIGHTNING->mNumCols)
	//		{
	//			mParticles.erase(mParticles.begin() + i);
	//			--i;
	//		}
	//	}
	//}

	//if (mFadeEffect->IsActive())
	//{
	//	mFadeEffect->Update(theFrac);

	//	// If the proper state is reached in the level up effect, then we can begin
	//	// setting up the next level.
	//	if (mFadeEffect->StartNextLevel())
	//	{
	//		// Just finished, start the next level
	//		mOptionsBtn->SetVisible(true);
	//		InitLevel(mLevel + 1);
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::Draw(Graphics* g)
{
	Floater::Draw(g);

	DrawGrid(g);

	if( mEditTileSelected != NULL && mState == State::EditingLevel )
	{
		int oldColor = mEditTileSelected->mColor;
		mEditTileSelected->mColor = mCurrentEditColor;
		mEditTileSelected->mColor = oldColor;
	}

	// first intro level special case
	if( mLimitToSliderNumber != -1 )
	{
		// draw arrow
		Rect	arrowrect;
		arrowrect.mX = mSliderButtons[mLimitToSliderNumber].mX + (mSliderButtons[mLimitToSliderNumber].Width() * 0.75f);
		arrowrect.mY = mSliderButtons[mLimitToSliderNumber].mY - (mSliderButtons[mLimitToSliderNumber].Height() * 0.75f);
		arrowrect.mWidth = mSliderButtons[mLimitToSliderNumber].Width();
		arrowrect.mHeight = mSliderButtons[mLimitToSliderNumber].Height();

		// animate arrow image
		g->SetColorizeImages(true);
		int alpha = abs(((int)(mGameScreen->mAnimateCurrentTime) % 128) - 64) * 4;
		if( alpha > 255 ) alpha = 255;
		g->SetColor(Color(255,255,255,alpha));
		g->DrawImage(IMAGE_ARROW,arrowrect.mX,arrowrect.mY,arrowrect.mWidth,arrowrect.mHeight);
		g->SetColorizeImages(false);
	}

	// bring fx to front
	// TODO: move elsewhere to optimize?
	mApp->mWidgetManager->BringToFront(mApp->mParticleWidget);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::DrawGrid(Graphics* g)
{
	//if( gPauseLevel ) // || mGameScreen->mMovie->mWidgetManager != NULL )
	//	g->SetColor(Color(128,128,128,(int)mAlpha));
	//else
		g->SetColor(Color(255,255,255,(int)mAlpha));

	g->SetColorizeImages(true);

//	g->DrawImage(IMAGE_BOARD_BG,
//		0, 0, mWidth, mHeight );

	for( int i = 0; i < mGridSize * 4; i++ )
	{
		SliderButton* aButton = &mSliderButtons[i];

		if( aButton->mHasFocus )
		{
			aButton->DrawBoardHover(g);
		}
	}


	g->SetColorizeImages(false);

}

//void GameBoard::DrawTiles(Graphics* g)
//{
//	if( gPauseLevel )
//		g->SetColor(Color(128,128,128,(int)mAlpha));
//	else
//		g->SetColor(Color(255,255,255,(int)mAlpha));
//
//	g->SetColorizeImages(true);
//
//	//Tile *aTile;
//
//	//for( int i = 0; i < MAX_BOARD_TILES; i++ )
//	//{
//	//	aTile = &(mBoardTiles[i]);
//
//	//	if( mBoardTiles[i].mIsActive )
//	//	{
//	//		g->DrawImageCel(IMAGE_BLOCKS,
//	//			(int)aTile->mPosition.x,
//	//			(int)aTile->mPosition.y,
//	//			aTile->mColor);
//	//	}
//	//}
//
//	g->SetColorizeImages(false);
//
//}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::DrawUI(Graphics* g)
{
	//int ascent = FONT_DEFAULT->GetAscent();
	//int height = FONT_DEFAULT->GetHeight();

	//g->SetFont(FONT_DEFAULT);
	//std::string s;
	//int rightX = FONT_DEFAULT->StringWidth("POPULATION CONSUMED: ") + 5;
	//
	//int strWidth;
	//s = "WORLDS DEVOURED: ";
	//strWidth = FONT_DEFAULT->StringWidth(s);
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, rightX - strWidth, ascent);
	//g->SetColor(Color(255, 0, 0, 200));
	//g->DrawString(StrFormat("%d", mNumPlanetsEaten), rightX - 5, ascent);

	//s = "POPULATION CONSUMED: ";
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, 5, height * 2);
	//g->SetColor(Color(255, 0, 0, 200));
	//g->DrawString(CommaSeperate(mPopulationEaten), rightX - 5, height * 2);	

	//s = "SCORE: ";
	//strWidth = FONT_DEFAULT->StringWidth(s);
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, rightX - strWidth, height * 3);
	//g->SetColor(Color(255, 255, 0, 200));
	//g->DrawString(StrFormat("%s", CommaSeperate(mScore).c_str()), rightX - 5, height * 3);



	//int x = 380;
	//s = "SYSTEMS SUBJUGATED: ";
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, x, ascent);
	//g->SetColor(Color(255, 0, 0, 200));
	//g->DrawString(StrFormat("%d%%", mPartUncovered), x + FONT_DEFAULT->StringWidth(s), ascent);

	//s = "LIVES: ";
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, x, height * 2);
	//strWidth = FONT_DEFAULT->StringWidth(s);
	//g->DrawImage(IMAGE_HUNGARR_SMALL, strWidth + x, ascent);
	//g->SetColor(Color(255, 0, 0, 200));
	//g->DrawString(StrFormat("x%d", mLives), x + 10 + strWidth + IMAGE_HUNGARR_SMALL->GetWidth(), height * 2);	

	//s = "LEVEL: ";
	//g->SetColor(Color(255, 255, 255, 128));
	//g->DrawString(s, x, height * 3);
	//g->SetColor(Color(255, 255, 0, 200));
	//g->DrawString(StrFormat("%d", mLevel), x + FONT_DEFAULT->StringWidth(s), height * 3);

	//for (int i = 0; i < mBonusText.size(); i++)
	//{
	//	BonusText* bt = &mBonusText[i];
	//	g->SetColor( (mApp->HSLToRGB(bt->mHue, 255, 128) & 0xFFFFFF) | (bt->mAlpha << 24) );
	//	g->DrawString(bt->mText, bt->mX, bt->mY);
	//}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::AddedToManager(WidgetManager* theWidgetManager)
{	
	Widget::AddedToManager(theWidgetManager);

	gPauseLevel = 0;
	mAnimating = true;
	mTimer = 0;
	mNextArcadeSlideInTime = mTimer + mArcadeSlideInDelay;
	SetArcadeSlideInDelay(1);
	mAlpha = 0;

	this->mHasTransparencies = true;

	for( int i=0; i<mGridSize; i++ )
	{
		mGameScreen->mFillTiles[i] = new Tile();
		Tile* aTile = mGameScreen->mFillTiles[i];

		aTile->mIsEditTile = true;

		aTile->mPosition.x = this->Left() + aTile->Width() + (aTile->Width() * i);
		aTile->mPosition.y = this->Bottom();

		aTile->mColor = i%8;
		aTile->mIsActive = false;
		aTile->mImage = IMAGE_BLOCKS;
		theWidgetManager->AddWidget(aTile);
	}

	for( int i=0; i<100; i++ )
	{
		floatingtile[i] = new Tile();
		floatingtile[i]->mImage = IMAGE_BLOCKS;
	}

	mFloatingTileLogoForce.mActive = true;
	mFloatingTileLogoForce.mMass = 0.0003f;
	//mFloatingTileLogoForce.mSource.x = this->Left() / 2;
	mFloatingTileLogoForce.mSource.x = mGameScreen->mBGTileCenterX;
	mFloatingTileLogoForce.mSource.y = mGameScreen->mBGTileCenterY - (80*gGraphicsScale);
	mFloatingTileLogoForce.mSource.z = 0.15f;


	mGameScreen->mText.Resize(0,0,mGameScreen->Width(),mGameScreen->Height());
	mGameScreen->mText.SetText("",0);
	theWidgetManager->AddWidget(&mGameScreen->mText);

	FadeInTiles();

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	if( mGameScreen->mText.mWidgetManager != NULL )
		theWidgetManager->RemoveWidget( &mGameScreen->mText );


	for( int i=0; i<100; i++ )
	{
		if( floatingnumber[i].mWidgetManager != NULL )
			theWidgetManager->RemoveWidget( &floatingnumber[i] );
	}

	for( int i=0; i<100; i++ )
	{
		if( floatingtile[i]->mWidgetManager != NULL )
		{
			theWidgetManager->RemoveWidget( floatingtile[i] );
		}

		delete floatingtile[i];
	}


	for( int i=0; i<mGridSize; i++ )
	{
		if( mGameScreen->mFillTiles[i]->mWidgetManager != NULL )
			theWidgetManager->RemoveWidget( mGameScreen->mFillTiles[i] );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::ButtonDepress(int theId)
{
	//// Play a sound whenever the options button is depressed/sad.
	//if (theId == mOptionsBtn->mId)
	//{
	//	// Stop the shorting sound if it's playing, otherwise it's annoying
	//	mShortSound->Stop();

	//	mApp->PlaySample(SOUND_BUTTON);
	//	Pause(true);
	//	OptionsDialog* od = new OptionsDialog(this);
	//	od->Resize(mWidth / 2 - 200, mHeight / 2 - 175, 400, 350);
	//	mApp->AddDialog(OptionsDialog::DIALOG_ID, od);
	//}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::MouseMove(int x, int y)
{
	int tileNo;
	int mEditTileSelectedX;
	int mEditTileSelectedY;

	float fullsize = gSexyAppBase->GetDouble("BOARD_TILE_WIDTH", 51.2f) * gGraphicsScale;

	mEditTileSelectedX = ((x / (mGridSize+2))/fullsize)-1;
	mEditTileSelectedY = ((y / (mGridSize+2))/fullsize)-1;

	if( mEditTileSelectedX>=0 && mEditTileSelectedX<mGridSize && 
		mEditTileSelectedY>=0 && mEditTileSelectedY<mGridSize )
	{
		tileNo = mEditTileSelectedX + (mEditTileSelectedY*mGridSize);
		mEditTileSelected = &mBoardTiles[tileNo];
	}
	else
	{
		mEditTileSelected = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::MouseDrag(int x, int y)
{
}


void GameBoard::MouseEnter()
{
}

void GameBoard::MouseLeave()
{
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::MouseDown(int x, int y, int theClickCount)
{		

	if( mState == State::EditingLevel && mEditTileSelected != NULL )
	{
		mEditTileSelected->mColor = mGameScreen->mEditTile.mColor;
	}

	//// if the level up effect is displaying stats, or the game over effect is too,
	//// and the user clicked, then start the next phase
	//if (mFadeEffect->ShowingStats())
	//	mFadeEffect->DoneViewingStats();

	//if (mGameOverEffect->CanStartNewGame())
	//	mGameOverEffect->DoneViewingStats();

	//// ignore mouse clicks when paused or an effect is on screen and the user has no reason to click
	//if (mFadeEffect->IsActive() || (gPauseLevel > 0) || (mGameOverEffect->IsActive() && !mGameOverEffect->CanStartNewGame()))
	//	return;

	//// On a right click, if the click was within the grid bounds, switch hungarr's orientation
	//if ((theClickCount < 0) && XCoordInBounds(x) && YCoordInBounds(y, mHungarrIsVertical))
	//	mHungarrIsVertical = !mHungarrIsVertical;
	//else if ((theClickCount > 0) && mMovingLine1.mDone && mMovingLine2.mDone && !mFilling)
	//{
	//	//left click, and there's no lines moving: drop two new lines 

	//	// Make sure the user didn't click on a planet which would instantly kill them
	//	FRect hungarrRect = FRect(mHungarrX, mHungarrY, IMAGE_HUNGARR_HORIZ->mWidth, IMAGE_HUNGARR_HORIZ->mHeight);
	//	for (int i = 0; i < mPlanets.size(); i++)
	//	{
	//		Planet* p = &mPlanets[i];
	//		FRect planetRect = FRect(p->mX, p->mY, IMAGE_PLANETS->GetCelWidth(), IMAGE_PLANETS->GetCelHeight());
	//		if (planetRect.Intersects(hungarrRect))
	//			return;
	//	}

	//	mApp->PlaySample(SOUND_MAGZAP);

	//	// start the electrical shorting sound
	//	mShortSound->Play(true, false);

	//	mMovingLine1.mDone = mMovingLine2.mDone = false;
	//	mMovingLine1.mBroken = mMovingLine2.mBroken = false;
	//	int midX = IMAGE_HUNGARR_HORIZ->GetWidth() / 2;
	//	int midY = IMAGE_HUNGARR_HORIZ->GetHeight() / 2;

	//	//Align the XYs of the lines to the grid, and set the target coordinates to the
	//	//closest normal state tile.
	//	if (mHungarrIsVertical)
	//	{
	//		mMovingLine1.mIsVertical = mMovingLine2.mIsVertical = true;			
	//		mMovingLine1.mX = mMovingLine2.mX = GetAlignedX(mHungarrX + midX);
	//		mMovingLine1.mY = mMovingLine2.mY = GetAlignedY(mHungarrY + midY);
	//		mMovingLine1.mHeight = 1;
	//		mMovingLine2.mHeight = 13;
	//		mMovingLine1.mWidth = mMovingLine2.mWidth = GRID_PIX_SIZE;
	//		mMovingLine1.mTargetY = mMovingLine2.mTargetY = mMovingLine1.mY;
	//		mMovingLine1.mTargetX = mMovingLine2.mTargetX = mMovingLine1.mX;

	//		// Make sure the target coords end at a tile that's normal. If not, keep moving them
	//		int row = GetRow(mMovingLine1.mTargetY);
	//		int col = GetCol(mMovingLine1.mTargetX);
	//		
	//		// Tile immediately below is not valid
	//		if (mGridState[row][col].mFillState != GRID_NORMAL)
	//			return;

	//		while ((row >= 0) && (mGridState[row][col].mFillState == GRID_NORMAL))
	//		{
	//			mMovingLine1.mTargetY -= GRID_PIX_SIZE;
	//			--row;
	//		}

	//		// Make it end on the last valid tile. The loop above makes it leave
	//		// on an invalid tile
	//		mMovingLine1.mTargetY += GRID_PIX_SIZE;

	//		row = GetRow(mMovingLine2.mTargetY);
	//		col = GetCol(mMovingLine2.mTargetX);

	//		while ((row < GRID_HEIGHT) && (mGridState[row][col].mFillState == GRID_NORMAL))
	//		{
	//			mMovingLine2.mTargetY += GRID_PIX_SIZE;
	//			++row;
	//		}

	//		if (mMovingLine1.mTargetY > mMovingLine2.mTargetY)
	//			mMovingLine1.mDone = mMovingLine2.mDone = true;
	//	}
	//	else
	//	{
	//		mMovingLine1.mIsVertical = mMovingLine2.mIsVertical = false;
	//		mMovingLine1.mX = mMovingLine2.mX = GetAlignedX(mHungarrX + midX);
	//		mMovingLine1.mY = mMovingLine2.mY = GetAlignedY(mHungarrY + midY);
	//		mMovingLine1.mWidth = 1;
	//		mMovingLine2.mWidth = 13;
	//		mMovingLine1.mHeight = mMovingLine2.mHeight = GRID_PIX_SIZE;
	//		mMovingLine1.mTargetX = mMovingLine2.mTargetX = mMovingLine1.mX;
	//		mMovingLine1.mTargetY = mMovingLine2.mTargetY = mMovingLine1.mY;

	//		// Make sure the target coords end at a tile that's normal. If not, keep moving them
	//		int row = GetRow(mMovingLine1.mTargetY);
	//		int col = GetCol(mMovingLine1.mTargetX);

	//		// Tile immediately below is not valid...?
	//		if (mGridState[row][col].mFillState != GRID_NORMAL)
	//			return;

	//		while ((col >= 0) && (mGridState[row][col].mFillState == GRID_NORMAL))
	//		{
	//			mMovingLine1.mTargetX -= GRID_PIX_SIZE;
	//			--col;
	//		}

	//		// Make it end on the last valid tile. The loop above makes it leave
	//		// on an invalid tile
	//		mMovingLine1.mTargetX += GRID_PIX_SIZE;


	//		row = GetRow(mMovingLine2.mTargetY);
	//		col = GetCol(mMovingLine2.mTargetX);
	//		
	//		while ((col < GRID_WIDTH) && (mGridState[row][col].mFillState == GRID_NORMAL))
	//		{
	//			mMovingLine2.mTargetX += GRID_PIX_SIZE;
	//			++col;
	//		}

	//		
	//		if (mMovingLine1.mTargetX > mMovingLine2.mTargetX)
	//			mMovingLine1.mDone = mMovingLine2.mDone = true;
	//	}
	//}

	//UpdateHungarrPosition(x, y);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::Pause(bool p)
{
	//if (p)
	//{
	//	// Since when we're paused we don't update each frame, call
	//	// MarkDirty here so that we ensure the "PAUSED" overlay appears
	//	MarkDirty();
	//	++gPauseLevel;
	//}
	//else
	//{
	//	if (--gPauseLevel == 0)
	//	{
	//		mAnimating = true;
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void GameBoard::KeyChar(char theChar)
{
	switch( theChar )
	{
	//case '1':
	//	mGridSize = 6;
	//	CreateBoardObjects();
	//	break;
	//case '2':
	//	mGridSize = 7;
	//	CreateBoardObjects();
	//	break;
	//case '3':
	//	mGridSize = 8;
	//	CreateBoardObjects();
	//	break;
	//case '4':
	//	mGridSize = 9;
	//	CreateBoardObjects();
	//	break;
	//case '5':
	//	mGridSize = 10;
	//	CreateBoardObjects();
	//	break;

	//case '3':
	//	if( mState == State::EditingLevel &&
	//		mEditTileSelected != NULL )
	//	{
	//		mEditTileSelected->mNumberOverlay->mDigit = -1;
	//		mEditTileSelected->mMatchLevel = 3;
	//	}
	//	break;
	//case '4':
	//	if( mState == State::EditingLevel &&
	//		mEditTileSelected != NULL )
	//	{
	//		mEditTileSelected->mNumberOverlay->mDigit = 4;
	//		mEditTileSelected->mMatchLevel = 4;
	//	}
	//	break;
	//case '5':
	//	if( mState == State::EditingLevel &&
	//		mEditTileSelected != NULL )
	//	{
	//		mEditTileSelected->mNumberOverlay->mDigit = 5;
	//		mEditTileSelected->mMatchLevel = 5;
	//	}
	//	break;

	//case 'b':
	//case 'B':
	//	if( mState == State::EditingLevel &&
	//		mEditTileSelected != NULL )
	//	{
	//		if( mEditTileSelected->mIsBlock )
	//		{
	//			mEditTileSelected->mIsBlock = false;
	//			mEditTileSelected->mIsActive = false;
	//		}
	//		else
	//		{
	//			mEditTileSelected->mIsBlock = true;
	//			mEditTileSelected->mIsActive = true;
	//		}
	//	}
	//	break;

	//case 'm':
	//case 'M':
	//	mIsMonkey ^= 1;
	//	break;

	//case 'l':
	//case 'L':
	//	if( mState == State::EditingLevel )
	//		LoadLevel(myLevel);
	//	break;

	//case 'e':
	//case 'E':
	//	if( mState != State::EditingLevel )
	//	{
	//		mDoFinger = true;
	//		LoadLevel(myLevel);
	//		mState = State::EditingLevel;
	//		mStoredState = mState;
	//		mGameScreen->mEditTile.mIsActive = true;
	//		mGameScreen->CreateEditorButtons();

	//		for( int i=0; i<MAX_GRID_SIZE*MAX_GRID_SIZE; i++ )
	//		{
	//			mBoardTiles[i].mDoFinger = true;
	//		}

	//		gSexyAppBase->mWidgetManager->SetFocus(this);
	//	}
	//	else
	//	{
	//		SaveLevel(myLevel);
	//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
	//			&myLevel, sizeof(Level));

	//		for( int i=0; i<MAX_GRID_SIZE*MAX_GRID_SIZE; i++ )
	//		{
	//			mBoardTiles[i].mDoFinger = true;
	//		}

	//		mDoFinger = false;
	//		mState = State::WaitingForUser;
	//		mStoredState = mState;
	//		mGameScreen->mEditTile.mIsActive = false;
	//		mGameScreen->RemoveEditorButtons();
	//		gSexyAppBase->mWidgetManager->SetFocus(this);
	//	}
	//	break;

	//case 'w':
	//case 'W':
	//	if( mState == State::EditingLevel )
	//	{
	//		SaveLevel(myLevel);
	//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
	//			&myLevel, sizeof(Level));

	//		LevelPak* levelPak = mApp->mLevelPak;

	//		if( levelPak->mCurrentLevel < levelPak->mLevelCount )
	//		{
	//			levelPak->mCurrentLevel++;
	//			mApp->mGameBoard->LoadLevel(
	//				levelPak->mLevels[levelPak->mCurrentLevel]);
	//		}
	//		else
	//		{
	//			if( levelPak->mCurrentLevel < MAX_LEVEL_COUNT-1 )
	//			{
	//				levelPak->mLevelCount++;
	//				levelPak->mCurrentLevel++;
	//				mApp->mGameBoard->LoadLevel(
	//					levelPak->mLevels[levelPak->mCurrentLevel]);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
	//		{
	//			mFocusedSliderNumber = 0;
	//			break;
	//		}
	//		else
	//		{
	//			mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

	//			switch( mSliderButtons[mFocusedSliderNumber].mRotate )
	//			{
	//			case Direction::Left:
	//				if( mFocusedSliderNumber == mGridSize )
	//					mFocusedSliderNumber = 0;
	//				else
	//					mFocusedSliderNumber -= 2;
	//				break;

	//			case Direction::Right:
	//				mFocusedSliderNumber -= 2;
	//				break;

	//			case Direction::Down:
	//				mFocusedSliderNumber -= mGridSize*3;
	//				break;
	//			}
	//		}

	//		gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
	//		mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
	//	}

	//	break;

	//case 's':
	//case 'S':
	//	if( mState == State::EditingLevel )
	//	{
	//		SaveLevel(myLevel);
	//		memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
	//			&myLevel, sizeof(Level));


	//		LevelPak* levelPak = mApp->mLevelPak;

	//		if( levelPak->mCurrentLevel > 1 )
	//		{
	//			levelPak->mCurrentLevel--;
	//			mApp->mGameBoard->LoadLevel(
	//				levelPak->mLevels[levelPak->mCurrentLevel]);
	//			mApp->mGameBoard->MarkDirty();
	//		}
	//	}
	//	else
	//	{
	//		if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
	//		{
	//			mFocusedSliderNumber = 0;
	//			break;
	//		}
	//		else
	//		{
	//			mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

	//			switch( mSliderButtons[mFocusedSliderNumber].mRotate )
	//			{
	//			case Direction::Left:
	//				mFocusedSliderNumber += 2;
	//				break;

	//			case Direction::Right:
	//				if( mFocusedSliderNumber == (mGridSize*3)-1 )
	//					mFocusedSliderNumber = (mGridSize*4)-1;
	//				else
	//					mFocusedSliderNumber += 2;
	//				break;

	//			case Direction::Up:
	//				mFocusedSliderNumber += mGridSize*3;
	//				break;
	//			}
	//		}

	//		gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
	//		mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
	//	}

	//	break;

	//case 'a':
	//case 'A':
	//	if( mState == State::EditingLevel )
	//	{
	//		if( mCurrentEditColor>0 ) mCurrentEditColor--;
	//		mGameScreen->mEditTile.mColor = mCurrentEditColor;
	//	}
	//	else
	//	{
	//		if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
	//		{
	//			mFocusedSliderNumber = 0;
	//			break;
	//		}
	//		else
	//		{
	//			mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

	//			switch( mSliderButtons[mFocusedSliderNumber].mRotate )
	//			{
	//			case Direction::Right:
	//				mFocusedSliderNumber--;
	//				break;

	//			case Direction::Up:
	//				if( mFocusedSliderNumber == 0 )
	//					mFocusedSliderNumber = mGridSize+1;
	//				mFocusedSliderNumber--;
	//				break;

	//			case Direction::Down:
	//				if( mFocusedSliderNumber == mGridSize*3 )
	//					mFocusedSliderNumber -= 2;
	//				else
	//					mFocusedSliderNumber--;
	//				break;
	//			}
	//		}

	//		gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
	//		mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
	//	}

	//	break;

	//case 'd':
	//case 'D':
	//	if( mState == State::EditingLevel )
	//	{
	//		if( mCurrentEditColor<7 ) mCurrentEditColor++;
	//		mGameScreen->mEditTile.mColor = mCurrentEditColor;
	//	}
	//	else
	//	{
	//		if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
	//		{
	//			mFocusedSliderNumber = 0;
	//		}
	//		else
	//		{
	//			mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

	//			switch( mSliderButtons[mFocusedSliderNumber].mRotate )
	//			{
	//			case Direction::Left:
	//				mFocusedSliderNumber++;
	//				break;

	//			case Direction::Up:
	//				if( mFocusedSliderNumber == mGridSize-1 )
	//					mFocusedSliderNumber += 2;
	//				else
	//					mFocusedSliderNumber++;
	//				break;

	//			case Direction::Down:
	//				if( mFocusedSliderNumber == (mGridSize*4) - 1 )
	//					mFocusedSliderNumber -= mGridSize;
	//				else
	//					mFocusedSliderNumber++;
	//				break;
	//			}
	//		}

	//		gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
	//		mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
	//	}

	//	break;

	//case 'c':
	//case 'C':
	//	if( mState == State::EditingLevel &&
	//		mEditTileSelected != NULL )
	//	{
	//		mEditTileSelected->mIsActive = false;
	//		mEditTileSelected->mColor = -1;
	//	}
	//	break;

	//case 'r':
	//case 'R':
	//	RandomizeBoard(true);
	//	break;

	//case 'u':
	//case 'U':
	//	mState = State::SlidingTiles;
	//	StartTileShiftByOne(Direction::Up);
	//	mIsAddingNewTiles = true;
	//	break;

	//// TODO: REMOVE!
	//case '9':
	//	mApp->mLevelPak->LoadLevelsFromMB1("puzzles.dat");
	//	LoadLevel(mApp->mLevelPak->mLevels[1]);
	//	FadeInTiles();
	//	break;
	//// TODO: REMOVE!
	//case '0':
	//	mApp->mLevelPak->LoadBonusLevelsFromMB1("puzzles.dat");
	//	LoadLevel(mApp->mLevelPak->mLevels[1]);
	//	FadeInTiles();
	//	break;


//	case:
//		break;

	}

}

void GameBoard::MouseWheel(int theDelta)
{
	if( mState == State::EditingLevel )
	{
		if( theDelta > 0 )
		{
			if( mCurrentEditColor<7 ) mCurrentEditColor++;
			mGameScreen->mEditTile.mColor = mCurrentEditColor;
		}
		else
		{
			if( mCurrentEditColor>0 ) mCurrentEditColor--;
			mGameScreen->mEditTile.mColor = mCurrentEditColor;
		}
	}
}

void GameBoard::KeyDown( KeyCode theKey )
{
	switch( theKey )
	{
	case KEYCODE_SPACE:
	case KEYCODE_NUMPAD0:
		if( mState == State::EditingLevel &&
			mEditTileSelected != NULL )
		{
			mEditTileSelected->mIsActive = true;
			mEditTileSelected->mColor = mCurrentEditColor;
		}
		else if( mFocusedSliderNumber >= 0 && mFocusedSliderNumber < mGridSize * 4 )
		{
			mSliderButtons[mFocusedSliderNumber].MouseDown(0,0,1);
		}
		break;

	case KEYCODE_UP:
		if( mState == State::EditingLevel )
		{
			SaveLevel(myLevel);

			memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
				&myLevel, sizeof(Level));

			LevelPak* levelPak = mApp->mLevelPak;

			if( levelPak->mCurrentLevel < levelPak->mLevelCount )
			{
				levelPak->mCurrentLevel++;
				mApp->mGameBoard->LoadLevel(
					levelPak->mLevels[levelPak->mCurrentLevel]);
			}
			else
			{
				if( levelPak->mCurrentLevel < MAX_LEVEL_COUNT-1 )
				{
					levelPak->mLevelCount++;
					levelPak->mCurrentLevel++;
					mApp->mGameBoard->LoadLevel(
						levelPak->mLevels[levelPak->mCurrentLevel]);
				}
			}
		}
		else
		{
			if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
			{
				mFocusedSliderNumber = 0;
			}
			else
			{
				mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

				switch( mSliderButtons[mFocusedSliderNumber].mRotate )
				{
				case Direction::Left:
					if( mFocusedSliderNumber == mGridSize )
						mFocusedSliderNumber = 0;
					else
						mFocusedSliderNumber -= 2;
					break;

				case Direction::Right:
					mFocusedSliderNumber -= 2;
					break;

				case Direction::Down:
					mFocusedSliderNumber -= mGridSize*3;
					break;
				}
			}

			gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
			mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
		}

		break;

	case KEYCODE_DOWN:
		if( mState == State::EditingLevel )
		{
			SaveLevel(myLevel);
			memcpy(&(mApp->mLevelPak->mLevels[mApp->mLevelPak->mCurrentLevel]),
				&myLevel, sizeof(Level));


			LevelPak* levelPak = mApp->mLevelPak;

			if( levelPak->mCurrentLevel > 1 )
			{
				levelPak->mCurrentLevel--;
				mApp->mGameBoard->LoadLevel(
					levelPak->mLevels[levelPak->mCurrentLevel]);
				mApp->mGameBoard->MarkDirty();
			}
		}
		else
		{
			if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
			{
				mFocusedSliderNumber = mGridSize;
			}
			else
			{
				mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

				switch( mSliderButtons[mFocusedSliderNumber].mRotate )
				{
				case Direction::Left:
					mFocusedSliderNumber += 2;
					break;

				case Direction::Right:
					if( mFocusedSliderNumber == (mGridSize*3)-1 )
						mFocusedSliderNumber = (mGridSize*4)-1;
					else
						mFocusedSliderNumber += 2;
					break;

				case Direction::Up:
					mFocusedSliderNumber += mGridSize*3;
					break;
				}
			}

			gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
			mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
		}

		break;

	case KEYCODE_LEFT:
		if( mState == State::EditingLevel )
		{
			if( mCurrentEditColor>0 ) mCurrentEditColor--;
			mGameScreen->mEditTile.mColor = mCurrentEditColor;
		}
		else
		{
			if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
			{
				mFocusedSliderNumber = mGridSize;
			}
			else
			{
				mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

				switch( mSliderButtons[mFocusedSliderNumber].mRotate )
				{
				case Direction::Right:
					mFocusedSliderNumber--;
					break;

				case Direction::Up:
					if( mFocusedSliderNumber == 0 )
						mFocusedSliderNumber = mGridSize+1;
					mFocusedSliderNumber--;
					break;

				case Direction::Down:
					if( mFocusedSliderNumber == mGridSize*3 )
						mFocusedSliderNumber -= 2;
					else
						mFocusedSliderNumber--;
					break;
				}
			}

			gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
			mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
		}

		break;

	case KEYCODE_RIGHT:
		if( mState == State::EditingLevel )
		{
			if( mCurrentEditColor<7 ) mCurrentEditColor++;
			mGameScreen->mEditTile.mColor = mCurrentEditColor;
		}
		else
		{
			if( mFocusedSliderNumber == MAX_GRID_SIZE * 4 )
			{
				mFocusedSliderNumber = 0;
			}
			else
			{
				mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

				switch( mSliderButtons[mFocusedSliderNumber].mRotate )
				{
				case Direction::Left:
					mFocusedSliderNumber++;
					break;

				case Direction::Up:
					if( mFocusedSliderNumber == mGridSize-1 )
						mFocusedSliderNumber += 2;
					else
						mFocusedSliderNumber++;
					break;

				case Direction::Down:
					if( mFocusedSliderNumber == (mGridSize*4) - 1 )
						mFocusedSliderNumber -= mGridSize;
					else
						mFocusedSliderNumber++;
					break;
				}
			}

			gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[mFocusedSliderNumber]);
			mSliderButtons[mFocusedSliderNumber].mFadeState = 1;
		}

		break;

	case KEYCODE_ESCAPE:
		Pause(gPauseLevel == 0);
		break;

	}

	//switch( theKey )
	//{
	//	gSexyAppBase->mWidgetManager->SetFocus(&mSliderButtons[0]);
	//}
}


void GameBoard::ShiftTiles(float theFrac)
{
	SliderButton *aButton;
	bool areAnyTilesMoving = false;
	bool areTilesMoving = false;

	static bool wereAnyTilesMoving = false;

	int slideDirection = Direction::None;

	char tempMatchLevel;
	Digit* tempNumberOverlay;

	SoundInstance *hitSound = NULL;
	int numHitSoundsPlaying = 0;


	for( int i = 0; i < mGridSize * 4; i++ )
	{
		aButton = &mSliderButtons[i];

		if( aButton->mIsAttractor )
		{
			Tile *aTile;
			Tile *aNewTile;
			int tileNumber = 0;

			switch( aButton->mRotate )
			{
			case Direction::Up:
				slideDirection = aButton->mRotate;

				for( tileNumber = 1; tileNumber < mGridSize; tileNumber++ )
				{
					aTile = &mBoardTiles[(tileNumber * mGridSize) + aButton->mColumn];

					if( aTile->mIsActive && aTile->mIsSliding )
					{
						if( ((aTile->mPosition.y + aTile->mVelocity.y + aTile->mAcceleration.y)
							< aTile->mThresholdY) ||
							mApp->isMonkeyingAllLevels )
						{

							if( !mIsMonkey )
							{
								if( numHitSoundsPlaying == 0 )
								{
									hitSound = gSexyAppBase->mSoundManager->GetSoundInstance(SOUND_BLOX_HIT);

									if( hitSound != NULL )
									{
										hitSound->Play(false, true);
										numHitSoundsPlaying++;
									}
								}
							}

							aTile->Stop();
							aTile->mIsSliding = false;
							aTile->mIsActive = false;
							aTile->mPosition.x = aTile->mHomeX;
							aTile->mPosition.y = aTile->mHomeY;

							// place as new tile
							aNewTile = &mBoardTiles[(tileNumber * mGridSize) -
								(aTile->mShiftCount * mGridSize) + aButton->mColumn];
							aNewTile->mIsActive = true;
							aNewTile->mIsSliding = false;
							aNewTile->Stop();
							aNewTile->mIsBlock = false;
							aNewTile->mColor = aTile->mColor;

							tempMatchLevel = aNewTile->mMatchLevel;
							aNewTile->mMatchLevel = aTile->mMatchLevel;
							aTile->mMatchLevel = tempMatchLevel;

							tempNumberOverlay = aNewTile->mNumberOverlay;
							aNewTile->mNumberOverlay = aTile->mNumberOverlay;
							aTile->mNumberOverlay = tempNumberOverlay;

							if( aTile->mIsWild )
							{
								aNewTile->mIsWild = true;
								aTile->mIsWild = false;
							}
							else
								aNewTile->mIsWild = false;
						}
						else
						{
							areTilesMoving = true;
						}

					}
				}

				break;

			case Direction::Down:
				slideDirection = aButton->mRotate;

				for( tileNumber = mGridSize-1; tileNumber >= 0; tileNumber-- )
				{
					aTile = &mBoardTiles[(tileNumber * mGridSize) + aButton->mColumn];

					if( aTile->mIsActive && aTile->mIsSliding )
					{
						if( ((aTile->mPosition.y + aTile->mVelocity.y + aTile->mAcceleration.y)
							> aTile->mThresholdY) ||
							mApp->isMonkeyingAllLevels )
						{
							if( !mIsMonkey )
							{
								if( numHitSoundsPlaying == 0 )
								{
									hitSound = gSexyAppBase->mSoundManager->GetSoundInstance(SOUND_BLOX_HIT);

									if( hitSound != NULL )
									{
										hitSound->Play(false, true);
										numHitSoundsPlaying++;
									}
								}
							}

							aTile->Stop();
							aTile->mIsSliding = false;
							aTile->mIsActive = false;
							aTile->mPosition.x = aTile->mHomeX;
							aTile->mPosition.y = aTile->mHomeY;

							// place as new tile
							aNewTile = &mBoardTiles[(tileNumber * mGridSize) +
								(aTile->mShiftCount * mGridSize) + aButton->mColumn];
							aNewTile->mIsActive = true;
							aNewTile->mIsSliding = false;
							aNewTile->Stop();
							aNewTile->mIsBlock = false;
							aNewTile->mColor = aTile->mColor;

							tempMatchLevel = aNewTile->mMatchLevel;
							aNewTile->mMatchLevel = aTile->mMatchLevel;
							aTile->mMatchLevel = tempMatchLevel;

							tempNumberOverlay = aNewTile->mNumberOverlay;
							aNewTile->mNumberOverlay = aTile->mNumberOverlay;
							aTile->mNumberOverlay = tempNumberOverlay;


							if( aTile->mIsWild )
							{
								aNewTile->mIsWild = true;
								aTile->mIsWild = false;
							}
							else
								aNewTile->mIsWild = false;

						}
						else
						{
							areTilesMoving = true;
						}

					}
				}

				break;

			case Direction::Left:
				slideDirection = aButton->mRotate;

				for( tileNumber = 1; tileNumber < mGridSize; tileNumber++ )
				{
					aTile = &mBoardTiles[(aButton->mRow * mGridSize) + tileNumber];

					if( aTile->mIsActive && aTile->mIsSliding )
					{
						if( ((aTile->mPosition.x + aTile->mVelocity.x + aTile->mAcceleration.x)
							< aTile->mThresholdX) ||
							mApp->isMonkeyingAllLevels )
						{
							if( !mIsMonkey )
							{
								if( numHitSoundsPlaying == 0 )
								{
									hitSound = gSexyAppBase->mSoundManager->GetSoundInstance(SOUND_BLOX_HIT);

									if( hitSound != NULL )
									{
										hitSound->Play(false, true);
										numHitSoundsPlaying++;
									}
								}
							}

							aTile->Stop();
							aTile->mIsSliding = false;
							aTile->mIsActive = false;
							aTile->mPosition.x = aTile->mHomeX;
							aTile->mPosition.y = aTile->mHomeY;

							// place as new tile
							aNewTile = &mBoardTiles[(aButton->mRow * mGridSize) +
								tileNumber - aTile->mShiftCount];
							aNewTile->mIsActive = true;
							aNewTile->mIsSliding = false;
							aNewTile->Stop();
							aNewTile->mIsBlock = false;
							aNewTile->mColor = aTile->mColor;

							tempMatchLevel = aNewTile->mMatchLevel;
							aNewTile->mMatchLevel = aTile->mMatchLevel;
							aTile->mMatchLevel = tempMatchLevel;

							tempNumberOverlay = aNewTile->mNumberOverlay;
							aNewTile->mNumberOverlay = aTile->mNumberOverlay;
							aTile->mNumberOverlay = tempNumberOverlay;



							if( aTile->mIsWild )
							{
								aNewTile->mIsWild = true;
								aTile->mIsWild = false;
							}
							else
								aNewTile->mIsWild = false;
						}
						else
						{
							areTilesMoving = true;
						}

					}
				}

				break;

			case Direction::Right:
				slideDirection = aButton->mRotate;

				for( tileNumber = mGridSize-1; tileNumber >= 0; tileNumber-- )
				{
					aTile = &mBoardTiles[(aButton->mRow * mGridSize) + tileNumber];

					if( aTile->mIsActive && aTile->mIsSliding )
					{
						if( ((aTile->mPosition.x + aTile->mVelocity.x + aTile->mAcceleration.x)
							> aTile->mThresholdX) ||
							mApp->isMonkeyingAllLevels )
						{
							if( !mIsMonkey )
							{
								if( numHitSoundsPlaying == 0 )
								{
									hitSound = gSexyAppBase->mSoundManager->GetSoundInstance(SOUND_BLOX_HIT);

									if( hitSound != NULL )
									{
										hitSound->Play(false, true);
										numHitSoundsPlaying++;
									}
								}
							}

							aTile->Stop();
							aTile->mIsSliding = false;
							aTile->mIsActive = false;
							aTile->mPosition.x = aTile->mHomeX;
							aTile->mPosition.y = aTile->mHomeY;

							// place as new tile
							aNewTile = &mBoardTiles[(aButton->mRow * mGridSize) +
								tileNumber + aTile->mShiftCount];
							aNewTile->mIsActive = true;
							aNewTile->mIsSliding = false;
							aNewTile->Stop();
							aNewTile->mIsBlock = false;
							aNewTile->mColor = aTile->mColor;

							tempMatchLevel = aNewTile->mMatchLevel;
							aNewTile->mMatchLevel = aTile->mMatchLevel;
							aTile->mMatchLevel = tempMatchLevel;

							tempNumberOverlay = aNewTile->mNumberOverlay;
							aNewTile->mNumberOverlay = aTile->mNumberOverlay;
							aTile->mNumberOverlay = tempNumberOverlay;



							if( aTile->mIsWild )
							{
								aNewTile->mIsWild = true;
								aTile->mIsWild = false;
							}
							else
								aNewTile->mIsWild = false;
						}
						else
						{
							areTilesMoving = true;
						}

					}
				}

				break;

			}

			if( areTilesMoving == false )
			{
				aButton->mIsAttractor = false;
				areAnyTilesMoving = false;
			}
			else
			{
				areAnyTilesMoving = true;
				wereAnyTilesMoving = true;
			}
		}
	}

	if( !areAnyTilesMoving )
	{
		mState = State::WaitingForUser;

		if( mIsAddingNewTiles )
		{
			mIsAddingNewTiles = false;

			for( int i=0; i<mGridSize; i++ )
			{
				Tile *aTile = &mBoardTiles[((mGridSize-1)*mGridSize)+i];

				if( mGameScreen->mFillTiles[i]->mIsActive &&
					mGameScreen->mFillTiles[i]->mColor != -1 )
				{
					aTile->mColor = mGameScreen->mFillTiles[i]->mColor;
					aTile->mMatchLevel = mGameScreen->mFillTiles[i]->mMatchLevel;
					aTile->mNumberOverlay->mDigit = mGameScreen->mFillTiles[i]->mNumberOverlay->mDigit;
					aTile->mIsActive = true;
				}
			}

			RandomizeFillTiles();
		}

		if( wereAnyTilesMoving || mApp->isMonkeyingAllLevels )
		{
			int isGameOver = 0;
			bool isSlidingAgain = false;

			isGameOver = CheckBoard(true);
			SetBackgroundTiles();

// TODO: remove!
			if( gSexyAppBase->GetBoolean("AUTO_WIN", false ) )
			{
				isGameOver = CHECK_BOARD_RESULT::WIN_GAME;
			}

			switch(isGameOver)
			{
			case CHECK_BOARD_RESULT::WIN_GAME:
				// uncover item
//				mGameScreen->mPuzzlePakItem->mPosition.z =
//					(float)mApp->mLevelPak->mCurrentLevel / mApp->mLevelPak->mLevelCount;

				mApp->mMusicInterface->FadeOut(0,false,0.02f);
				mApp->mMusicFadeInTimer = 500.0f;


				LoadLevel(*(mApp->mLevelPak->GetNextLevel()));
				
				FadeOutTiles();

				// should only be 1 if we completed the pack
				// so advance to next pak
				if( mApp->mLevelPak->mNewCurrentLevel == 1 )
				{					
					mApp->mMusicFadeInTimer = 1600.0f;

					// create property string id
					std::string currentLevelPakName = mApp->mLevelPak->mName;
					std::string nextLevelName = "UNKNOWN";

					if( currentLevelPakName == "Random Puzzles" )
					{
						nextLevelName = "WIN_RANDOM";
					}
					else
					{
						// not random puzzles

						// get next level
						std::vector<std::string> levelPakList;

						LevelPak::FillLevelPakList(levelPakList);

						// show list
						std::vector<std::string>::iterator aName = levelPakList.begin();

						while( aName != levelPakList.end() )
						{
							if( std::string(*aName) == currentLevelPakName )
							{
								aName++;
								if( aName == levelPakList.end() )
								{
									nextLevelName = "WIN_BONUS";
								}
								else
								{
									nextLevelName = std::string(*aName);

									if( nextLevelName == "MatchBlox" )
									{
										nextLevelName = "WIN_GAME";
									}
									break;
								}
							}
							else
							{
								aName++;
							}
						}
					} // end if not random

					if( nextLevelName == "UNKNOWN" )
					{
						// just restart this one
						// advance to this pak
						mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_PAK_MOVIE_ID);
						mGameScreen->LoadPak( mApp->mLevelPak->mName );

						mGameScreen->SetItemUncoveredPart(1.0f);
						mGameScreen->SetItemUncoveredParticleLine(1.0f);
					}
					else
					{
						mGameScreen->mText.SetText("",0);

						if( nextLevelName == "WIN_GAME" )
						{
							//mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_MOVIE_ID);
							mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_PAK_MOVIE_ID);
							mGameScreen->mIsShowingEndGame = true;
							// play win movie after this one completes
							nextLevelName = "MatchBlox";
						}
						else if( nextLevelName == "WIN_BONUS" ) 
						{
							mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_2_MOVIE_ID);
							nextLevelName = "How To Play";
						}
						else if( nextLevelName == "WIN_RANDOM" ) 
						{
							mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_3_MOVIE_ID);
							nextLevelName = "Random Puzzles";
						}
						else
						{
							mGameScreen->PlayMovie(GameScreen::MOVIE_ID::WIN_PAK_MOVIE_ID);
						}

						// advance to this pak

						// save high score
						std::string highScoreKey = currentLevelPakName;
						highScoreKey += ":HighScore";
						mApp->RegistryWriteInteger(highScoreKey,mGameScreen->mHighScore);

						// load hi score
						mGameScreen->mHighScore = 0;
						highScoreKey = nextLevelName;
						highScoreKey += ":HighScore";
						gSexyAppBase->RegistryReadInteger(highScoreKey, &mGameScreen->mHighScore );

						mGameScreen->LoadPak( nextLevelName );

						mGameScreen->SetItemUncoveredPart(1.0f);
						mGameScreen->SetItemUncoveredParticleLine(1.0f);

						// write highest level if not higher than current
						if( nextLevelName != "Random Puzzles" )
						{
							std::string highestLevelPak;
	// only show unlocked paks
	std::string regKey = "";
	regKey += mApp->mPlayer.name;
	regKey += ":HighestLevelPak";
	gSexyAppBase->RegistryReadString(regKey, &highestLevelPak);



							if( highestLevelPak == "" )
							{
								gSexyAppBase->RegistryWriteString(regKey,nextLevelName);
							}
							else
							{
								std::vector<std::string> levelPakList;

								LevelPak::FillLevelPakList( levelPakList );

								// find first in list
								std::vector<std::string>::iterator aName = levelPakList.begin();

								while( aName != levelPakList.end() )
								{
									if( std::string(*aName) == nextLevelName )
									{
										// don't write anything
										break;
									}
									else if( std::string(*aName) == highestLevelPak )
									{
										gSexyAppBase->RegistryWriteString(regKey, nextLevelName );
									}

									aName++;
								}
							}
						}
					}

					mIsAutoFadeIn = false;

					// play pak complete sound
					mApp->PlaySample(SOUND_PAK_COMPLETE);

					mGameScreen->SetHighestLevelPakCompleteNumber();
				}
				else
				{
					// play single level complete sound
					mApp->PlaySample(SOUND_LEVEL_UP);
				}
				break;

			case CHECK_BOARD_RESULT::NO_MORE_MATCHES:

				if( mCanScore )
				{
					if( ((GameScreen*)mParent)->mScore > ((GameScreen*)mParent)->mHighScore )
					{
						((GameScreen*)mParent)->SetHighScore(
							((GameScreen*)mParent)->mScore);
					}
					((GameScreen*)mParent)->mScore = 0;

					mGameScreen->LoseGame();

				}
				

				mNumOfTimesLost++;
				break;

			case CHECK_BOARD_RESULT::KEEP_PLAYING:
				// kill all attractors
				for( int buttonNumber = 0; buttonNumber > mGridSize*4; buttonNumber++ )
				{
					SliderButton* aButtonToKill = &mSliderButtons[buttonNumber];
					aButtonToKill->mIsAttractor = false;

					if( aButtonToKill->mRotate == slideDirection &&
						higheststrand > 1 )
					{
						aButtonToKill->mIsAttractor = true;
						isSlidingAgain = true;
					}
				}

				if( isSlidingAgain )
				{
					StartTileShift();
					mState = State::SlidingTiles;
				}
				break;

			}

			wereAnyTilesMoving = false;
		}
	}


}


void GameBoard::StartTileShift(void)
{
	SliderButton* aButton;

	float tileSlideAcceleration = (float)gSexyAppBase->GetDouble(
		"TILE_SLIDE_ACCELERATION", 3.0f);
	float tileSlideAccelerationFalloff = (float)gSexyAppBase->GetDouble(
		"TILE_SLIDE_ACCELERATION_FALLOFF", 0.2f);

	mNumOfClicks++;

	if( mIsMonkey )
	{
		tileSlideAcceleration *= gSexyAppBase->GetDouble("THE_MONKEY_FACTOR",4.0f);

		//if( mApp->isMonkeyingAllLevels ) tileSlideAcceleration = Width();
	}

	for( int i = 0; i < mGridSize * 4; i++ )
	{
		aButton = &mSliderButtons[i];

		if( aButton->mIsAttractor )
		{
			Tile* aTile;
			Tile* aBlockingTile;
			int tileNumber = 0;

			// reset counter
			int freeSpaces;

			switch( aButton->mRotate )
			{
			case Direction::Up:
				for( int row = 1; row<mGridSize; row++ )
				{
					tileNumber = (row*mGridSize)+aButton->mColumn;
					aTile = &mBoardTiles[tileNumber];

					if( aTile->mIsActive && !aTile->mIsBlock )
					{
						freeSpaces = 0;

						for( int checkRow = row - 1; checkRow>=0; checkRow-- )
						{
#ifdef MATCHBLOX_ASSERT
sprintf(debug, "rotate %d, tilenumber %d", aButton->mRotate, tileNumber);
Assert( tileNumber-mGridSize >= 0, debug );
#endif
							aBlockingTile = &mBoardTiles[(checkRow*mGridSize)+aButton->mColumn];

							if( aBlockingTile->mIsActive )
							{
								if( aBlockingTile->mIsBlock ) break;
							}
							else
							{
								freeSpaces++;
							}
						}

						if( freeSpaces > 0 )
						{
							aTile->mAcceleration.y = ((0-tileSlideAcceleration) + (row*tileSlideAccelerationFalloff))
								* gGraphicsScale;
							aTile->mDirection = aButton->mRotate;
							aTile->mIsSliding = true;
							aTile->mShiftCount = freeSpaces;
							aTile->mThresholdY = aTile->mPosition.y - (aTile->mHeightF * freeSpaces);
						}
					}
				}

				break;


			case Direction::Down:
				for( int row = mGridSize-2; row>=0; row-- )
				{
					tileNumber = (row*mGridSize)+aButton->mColumn;
					aTile = &mBoardTiles[tileNumber];

					if( aTile->mIsActive && !aTile->mIsBlock )
					{
						freeSpaces = 0;

						for( int checkRow = row + 1; checkRow<mGridSize; checkRow++ )
						{
							aBlockingTile = &mBoardTiles[(checkRow*mGridSize)+aButton->mColumn];

							if( aBlockingTile->mIsActive )
							{
								if( aBlockingTile->mIsBlock ) break;
							}
							else
							{
								freeSpaces++;
							}
						}

						if( freeSpaces > 0 )
						{
							aTile->mAcceleration.y = (tileSlideAcceleration - (((mGridSize-1)-row)*tileSlideAccelerationFalloff)) * gGraphicsScale;
							aTile->mDirection = aButton->mRotate;
							aTile->mIsSliding = true;
							aTile->mShiftCount = freeSpaces;
							aTile->mThresholdY = aTile->mPosition.y + (aTile->Height() * freeSpaces);
						}
					}
				}

				break;

			case Direction::Left:
				for( int column = 1; column<mGridSize; column++ )
				{
					tileNumber = (aButton->mRow*mGridSize)+column;
					aTile = &mBoardTiles[tileNumber];

					if( aTile->mIsActive && !aTile->mIsBlock )
					{
						freeSpaces = 0;

						for( int checkColumn = column - 1; checkColumn>=0; checkColumn-- )
						{
							aBlockingTile = &mBoardTiles[(aButton->mRow*mGridSize)+checkColumn];

							if( aBlockingTile->mIsActive )
							{
								if( aBlockingTile->mIsBlock ) break;
							}
							else
							{
								freeSpaces++;
							}
						}

						if( freeSpaces > 0 )
						{
							aTile->mAcceleration.x = ((0-tileSlideAcceleration) + (column*tileSlideAccelerationFalloff)) * gGraphicsScale;
							aTile->mDirection = aButton->mRotate;
							aTile->mIsSliding = true;
							aTile->mShiftCount = freeSpaces;
							aTile->mThresholdX = aTile->mPosition.x - (aTile->Width() * freeSpaces);
						}
					}
				}

				break;

			case Direction::Right:
				for( int column = mGridSize-2; column>=0; column-- )
				{
					tileNumber = (aButton->mRow*mGridSize)+column;
					aTile = &mBoardTiles[tileNumber];

					if( aTile->mIsActive && !aTile->mIsBlock )
					{
						freeSpaces = 0;

						for( int checkColumn = column + 1; checkColumn<mGridSize; checkColumn++ )
						{
							aBlockingTile = &mBoardTiles[(aButton->mRow*mGridSize)+checkColumn];

							if( aBlockingTile->mIsActive )
							{
								if( aBlockingTile->mIsBlock ) break;
							}
							else
							{
								freeSpaces++;
							}
						}

						if( freeSpaces > 0 )
						{
							aTile->mAcceleration.x = (tileSlideAcceleration - (((mGridSize-1)-column)*tileSlideAccelerationFalloff)) * gGraphicsScale;
							aTile->mDirection = aButton->mRotate;
							aTile->mIsSliding = true;
							aTile->mShiftCount = freeSpaces;
							aTile->mThresholdX = aTile->mPosition.x + (aTile->Width() * freeSpaces);
						}
					}
				}

				break;

			}
		}
	}
}






CHECK_BOARD_RESULT GameBoard::CheckBoard(bool canScore)
{
	int boardx, boardy, boardspace;
	BOOL deletedblocks = FALSE;

	mCanScore = canScore;

	ZeroMemory( flagdelete, mGridSize*mGridSize );
	ZeroMemory( matched, mGridSize*mGridSize );

	int currfloatnumber = 0;

	numofstrands = 0;
	higheststrand = 0;

	for( boardy = 0; boardy < mGridSize; boardy++ )
	{
		for( boardx = 0; boardx < mGridSize; boardx++ )
		{
			if( checkspace( boardx, boardy ) == TRUE )
			{
				numofstrands++;
				deletedblocks = TRUE;
			}

		} // end boardx loop

	} // end boardy loop


	if( deletedblocks == TRUE )
	{
		// Add all floaters to main score ...
		if( canScore )
		{
			mApp->PlaySample(SOUND_BLOX_DISAPPEAR);

			for( currfloatnumber=0; currfloatnumber<100; currfloatnumber++ )
			{
				if( floatingtile[currfloatnumber]->mIsActive )
				{
					floatingtile[currfloatnumber]->mForce = &mFloatingTileLogoForce;
					gSexyAppBase->mWidgetManager->AddWidget(floatingtile[currfloatnumber]);
				}
			}

			// Multiply Scores by number of strands ...
			for( currfloatnumber=0; currfloatnumber<100; currfloatnumber++ )
			{
				if( floatingnumber[currfloatnumber].mIsActive )
				{
					floatingnumber[currfloatnumber].amount *= numofstrands;

					gSexyAppBase->mWidgetManager->AddWidget(&floatingnumber[currfloatnumber]);

					floatingnumber[currfloatnumber].SetAcceleration(0, -0.03f, -0.002f);

					((GameScreen*)mParent)->AddScore(floatingnumber[currfloatnumber].amount);
					((GameScreen*)mParent)->SetScore();

					floatingnumber[currfloatnumber].mIsActive = false;

				} // end loop through active floaters
			}
		}
		else
		{
			for( currfloatnumber=0; currfloatnumber<100; currfloatnumber++ )
			{
				if( floatingnumber[currfloatnumber].mIsActive )
				{
					floatingnumber[currfloatnumber].mIsActive = false;
				}

				if( floatingtile[currfloatnumber]->mIsActive )
				{
					floatingtile[currfloatnumber]->mIsActive = false;
				}
			}
		}

		// TODO: queue sound

		for( boardspace = 0; boardspace < mGridSize*mGridSize; boardspace++ )
		{
			if( flagdelete[boardspace] )
			{
				mBoardTiles[boardspace].mIsActive = false;
			} // end if this space is now empty
	
		} // end boardspace

		// Check for win ...
		for( boardspace = 0; boardspace < mGridSize*mGridSize; boardspace++ )
		{
			if( mBoardTiles[boardspace].mIsActive && !mBoardTiles[boardspace].mIsBlock ) break;
		} // end boardspace

		// Found no blocks
		if( boardspace == mGridSize*mGridSize )
		{
			// write monkeyline
			if( mApp->isMonkeyingAllLevels )
			{
				std::stringstream monkeyLine;
				std::string s;

				monkeyLine << mApp->mLevelPak->mName;
				monkeyLine << ", ";
				monkeyLine << mApp->mLevelPak->mCurrentLevel;
				monkeyLine << ", ";
				monkeyLine << mNumOfClicks;
				monkeyLine << ", ";
				monkeyLine << mNumOfTimesLost;
				monkeyLine << "\r\n";

				s = "";
				s += monkeyLine.str();

				mApp->mMonkeyFile.open("MonkeyFile.csv", std::ios_base::app );
				mApp->mMonkeyFile.write(s.c_str(), s.length());
				mApp->mMonkeyFile.close();

				mNumOfClicks = 0;
				mNumOfTimesLost = 0;
			}

			return WIN_GAME;
		}

// Check for game lose ...
		switch( mGameScreen->mGameMode )
		{
		case GameScreen::GameMode::Classic:
			if( nomorematches() )
			{
				//if( mCanScore )
				//{
				//	mApp->PlaySample(SOUND_GAME_OVER_RESTART);
				//}

				mNumOfTimesLost++;

				return NO_MORE_MATCHES;
			} // end if no more matches found
			break;
		}

	} // end if did delete some blocks

	// monkey all limit
	if( mApp->isMonkeyingAllLevels )
	{
		if( mNumOfClicks >= gSexyAppBase->GetInteger("MAX_MONKEY_CLICKS", 100000L) )
		{
			std::stringstream monkeyLine;
			std::string s;

			monkeyLine << mApp->mLevelPak->mName;
			monkeyLine << ", ";
			monkeyLine << mApp->mLevelPak->mCurrentLevel;
			monkeyLine << ", ";
			monkeyLine << mNumOfClicks;
			monkeyLine << ", ";
			monkeyLine << mNumOfTimesLost;
			monkeyLine << "\r\n";

			s = "";
			s += monkeyLine.str();

			mApp->mMonkeyFile.open("MonkeyFile.csv", std::ios_base::app );
			mApp->mMonkeyFile.write(s.c_str(), s.length());
			mApp->mMonkeyFile.close();

			mNumOfClicks = 0;
			mNumOfTimesLost = 0;

			return CHECK_BOARD_RESULT::WIN_GAME;
		}
	}

	return KEEP_PLAYING;

} // end checkboard


// Finds all matching children for a given space. Sets all blocks to be deleted and initializes
// floating scores ... Returns TRUE or FALSE (was block removed?)
int GameBoard::checkspace( int x, int y )
{
	int boardspace;
	
	boardspace = (y*mGridSize) + x;

	if( mBoardTiles[boardspace].mIsWild )
		return NULL;

	if( matched[boardspace] )
		return NULL;

	if( !mBoardTiles[boardspace].mIsActive )
		return NULL;

	if( mBoardTiles[boardspace].mIsBlock )
		return NULL;


	ZeroMemory( checknode, sizeof(CHECKNODE)*(mGridSize*mGridSize) );


// Init starting node ...
	checknode[1].color = mBoardTiles[boardspace].mColor;
	checknode[1].x = x;
	checknode[1].y = y;

	matched[boardspace] = TRUE;

	nextavailablenode = 2;

// Fill in tree data ...
	numofmatches = 0;
	numofchildrenneededformatch = mBoardTiles[boardspace].mMatchLevel - 1;

	getchildren( 1 );


	// if( numofmatches > higheststrand ) higheststrand = numofmatches;

	// match at least 2 neighbors
	if( numofmatches >= numofchildrenneededformatch )
	{
		removeblock( 1 );
		return TRUE;
	}

	return FALSE;

} // end checkspace



// Removes block and ALL children ...
void GameBoard::removeblock( int nodeno )
{
	int boardy, boardx, boardspace;

//	int xincrement, yincrement; // for movement


//sprintf( debugstr, "removeblock( %d )\n", nodeno );
//WRITETODEBUGFILE;



// Remove children ...
	if( checknode[nodeno].childno[0] ) removeblock( checknode[nodeno].childno[0] );
	if( checknode[nodeno].childno[1] ) removeblock( checknode[nodeno].childno[1] );
	if( checknode[nodeno].childno[2] ) removeblock( checknode[nodeno].childno[2] );
	if( checknode[nodeno].childno[3] ) removeblock( checknode[nodeno].childno[3] );


	boardx = checknode[nodeno].x;
	boardy = checknode[nodeno].y;

	boardspace = (boardy*mGridSize) + boardx;

	flagdelete[boardspace] = 1;


	if( mCanScore )
	{
	// Set up new floating tile ...
		floatingtile[nextfloatnumber]->mIsActive = true;
		floatingtile[nextfloatnumber]->mColor = mBoardTiles[boardspace].mColor;

		floatingtile[nextfloatnumber]->Stop();

		// floatingtile[nextfloatnumber]->mVelocity.x = -3.0f;
		// floatingtile[nextfloatnumber]->mVelocity.y = 3.0f;
		float fullsize = gSexyAppBase->GetDouble("BOARD_TILE_WIDTH", 51.2f) * gGraphicsScale;

		floatingtile[nextfloatnumber]->mPosition.x = (float)(this->Left())
			+ ((float)(boardx+1) * fullsize);
		floatingtile[nextfloatnumber]->mPosition.y = (float)(this->Top())
			+ ((float)(boardy+1) * fullsize);
		floatingtile[nextfloatnumber]->mPosition.z = 1;

		floatingtile[nextfloatnumber]->mNumberOverlay->mDigit = mBoardTiles[boardspace].mNumberOverlay->mDigit;

		floatingtile[nextfloatnumber]->mAcceleration.x = (float)(boardx-1-(mGridSize>>2))/20.0f;
		floatingtile[nextfloatnumber]->mAcceleration.y = (float)(boardy-1-(mGridSize>>2))/10.0f;



	// Set up new floating score ...
		floatingnumber[nextfloatnumber].amount = ((numofmatches+1)<<3) + ((numofmatches+1)<<1);

		// handle bonus points for 4 and 5 tiles
		if( numofchildrenneededformatch == 3 )
			floatingnumber[nextfloatnumber].amount *= 2;
		else if( numofchildrenneededformatch == 4 )
			floatingnumber[nextfloatnumber].amount *= 3;

		//floatingnumber[nextfloatnumber].color = board[boardspace]-1;

		floatingnumber[nextfloatnumber].mIsActive = true;

		floatingnumber[nextfloatnumber].mPosition.x = (float)(this->Left())
			+ ((float)(boardx+1) * fullsize)
			+ fullsize / 2.0f;
		floatingnumber[nextfloatnumber].mPosition.y = (float)(this->Top())
			+ ((float)(boardy+1) * fullsize)
			+ fullsize / 2.0f;


		//floatingnumber[nextfloatnumber].mPosition.x = gSexyAppBase->mWidth - ((128.0f * gGraphicsScale) +
		//	(int)((((gSexyAppBase->GetDouble("BOARD_TILE_WIDTH",51.2f)*gGraphicsScale)*mGridSize+1) -
		//	(float)((gSexyAppBase->GetDouble("BOARD_TILE_WIDTH",51.2f)*gGraphicsScale)*(0.5f))))) +
		//	(float)boardx;//+(float)boardx)))));

		//floatingnumber[nextfloatnumber].mPosition.y = (128.0f * gGraphicsScale) +
		//	(float)((gSexyAppBase->GetDouble("BOARD_TILE_WIDTH",51.2f)*gGraphicsScale)*(0.5f))
		//	+((float)boardy*(gSexyAppBase->GetDouble("BOARD_TILE_WIDTH",51.2f)*gGraphicsScale));//+(float)boardy));

	//	gSexyAppBase->mWidgetManager->AddWidget(&floatingnumber[nextfloatnumber]);

	// Turn off the one in front (will be reactivated if used, otherwise stop here) ...
	//	floatingnumber[nextfloatnumber + 1].mIsActive = false;

		// start particles
		
		// choose color
		SexyString particleFilename;

		switch( mBoardTiles[boardspace].mColor )
		{
		default:
			particleFilename = "particles\\blue_stars.psi";
			break;
		case 0:
			particleFilename = "particles\\blue_stars.psi";
			break;
		case 1:
			particleFilename = "particles\\white_stars.psi";
			break;
		case 2:
			particleFilename = "particles\\orange_stars.psi";
			break;
		case 3:
			particleFilename = "particles\\pink_stars.psi";
			break;
		case 4:
			particleFilename = "particles\\green_stars.psi";
			break;
		case 5:
			particleFilename = "particles\\yellow_stars.psi";
			break;
		case 6:
			particleFilename = "particles\\red_stars.psi";
			break;
		case 7:
			particleFilename = "particles\\cyan_stars.psi";
			break;
		}
		

		floatingtile[nextfloatnumber]->mParticleSystem = mApp->mParticleManager->SpawnPS(particleFilename.c_str(), NULL,
			floatingnumber[nextfloatnumber].mPosition.x,
			floatingnumber[nextfloatnumber].mPosition.y);

		// set to next #
		nextfloatnumber++;
		if( nextfloatnumber == 100 ) nextfloatnumber = 0;


		// Update item

		// TODO: change to utility method!
		if( mGameScreen->mIsItemUncoverEnabled )
		{
			mGameScreen->mPuzzlePakItem->mPartUncovered +=
				1.0f / mApp->mLevelPak->GetTotalTileCount();

			if( mGameScreen->mPuzzlePakItem->mPartUncovered > 1.0f )
				mGameScreen->mPuzzlePakItem->mPartUncovered = 1.0f;

			mGameScreen->SetItemUncoveredPart( mGameScreen->mPuzzlePakItem->mPartUncovered );
		}

		// TODO: set in motion

	} // end if mCanScore

} // end removeblock



// Recursive function ... returns number of children found that match
int GameBoard::getchildren( int nodeno )
{
	int boardspace;
	int checkcolor;
	int checkx, checky;

/*
sprintf( debugstr, "getchildren( %d )\n", nodeno );
WRITETODEBUGFILE;
sprintf( debugstr, " checknode[%d].x = %d\n", nodeno, checknode[nodeno].x );
WRITETODEBUGFILE;
sprintf( debugstr, " checknode[%d].y = %d\n", nodeno, checknode[nodeno].y );
WRITETODEBUGFILE;
*/

	checkcolor = checknode[nodeno].color;


//CHECKUP:
	checkx = checknode[nodeno].x;
	checky = checknode[nodeno].y - 1;

	if( checky < 0 ) goto CHECKDOWN;

	boardspace = (checky*mGridSize) + checkx;

	if( matched[boardspace] ) goto CHECKDOWN;

	if( mBoardTiles[boardspace].mIsActive == true &&
		mBoardTiles[boardspace].mIsBlock != true &&
		(mBoardTiles[boardspace].mColor == checkcolor ||
		mBoardTiles[boardspace].mIsWild == true) )
	{
		matched[boardspace] = TRUE;
		numofmatches++;

		if( mBoardTiles[boardspace].mMatchLevel > numofchildrenneededformatch+1 )
			numofchildrenneededformatch = mBoardTiles[boardspace].mMatchLevel-1;

		// Create new node and assign as child ...
		checknode[nodeno].childno[0] = nextavailablenode;
		checknode[nodeno].numofchildren++;

		checknode[nextavailablenode].parentno = nodeno;
		checknode[nextavailablenode].color = checkcolor;
		checknode[nextavailablenode].x = checkx;
		checknode[nextavailablenode].y = checky;

		nextavailablenode++;
		getchildren( nextavailablenode - 1 );
	} // end if match found


CHECKDOWN:
	checkx = checknode[nodeno].x;
	checky = checknode[nodeno].y + 1;

	if( checky > mGridSize-1 ) goto CHECKLEFT;

	boardspace = (checky*mGridSize) + checkx;

	if( matched[boardspace] ) goto CHECKLEFT;

	if( mBoardTiles[boardspace].mIsActive == true &&
		mBoardTiles[boardspace].mIsBlock != true &&
		(mBoardTiles[boardspace].mColor == checkcolor ||
		mBoardTiles[boardspace].mIsWild == true) )
	{
		matched[boardspace] = TRUE;
		numofmatches++;

		if( mBoardTiles[boardspace].mMatchLevel > numofchildrenneededformatch+1 )
			numofchildrenneededformatch = mBoardTiles[boardspace].mMatchLevel-1;

		// Create new node and assign as child ...
		checknode[nodeno].childno[1] = nextavailablenode;
		checknode[nodeno].numofchildren++;

		checknode[nextavailablenode].parentno = nodeno;
		checknode[nextavailablenode].color = checkcolor;
		checknode[nextavailablenode].x = checkx;
		checknode[nextavailablenode].y = checky;

		nextavailablenode++;
		getchildren( nextavailablenode - 1 );
	} // end if match found


CHECKLEFT:
	checkx = checknode[nodeno].x - 1;
	checky = checknode[nodeno].y;

	if( checkx < 0 ) goto CHECKRIGHT;

	boardspace = (checky*mGridSize) + checkx;

	if( matched[boardspace] ) goto CHECKRIGHT;

	if( mBoardTiles[boardspace].mIsActive == true &&
		mBoardTiles[boardspace].mIsBlock != true &&
		(mBoardTiles[boardspace].mColor == checkcolor ||
		mBoardTiles[boardspace].mIsWild == true) )
	{
		matched[boardspace] = TRUE;
		numofmatches++;

		if( mBoardTiles[boardspace].mMatchLevel > numofchildrenneededformatch+1 )
			numofchildrenneededformatch = mBoardTiles[boardspace].mMatchLevel-1;

		// Create new node and assign as child ...
		checknode[nodeno].childno[2] = nextavailablenode;
		checknode[nodeno].numofchildren++;

		checknode[nextavailablenode].parentno = nodeno;
		checknode[nextavailablenode].color = checkcolor;
		checknode[nextavailablenode].x = checkx;
		checknode[nextavailablenode].y = checky;

		nextavailablenode++;
		getchildren( nextavailablenode - 1 );
	} // end if match found



CHECKRIGHT:
	checkx = checknode[nodeno].x + 1;
	checky = checknode[nodeno].y;

	if( checkx > mGridSize-1 ) goto CHECKSDONE;

	boardspace = (checky*mGridSize) + checkx;

	if( matched[boardspace] ) goto CHECKSDONE;

	if( mBoardTiles[boardspace].mIsActive == true &&
		mBoardTiles[boardspace].mIsBlock != true &&
		(mBoardTiles[boardspace].mColor == checkcolor ||
		mBoardTiles[boardspace].mIsWild == true) )
	{
		matched[boardspace] = TRUE;
		numofmatches++;

		if( mBoardTiles[boardspace].mMatchLevel > numofchildrenneededformatch+1 )
			numofchildrenneededformatch = mBoardTiles[boardspace].mMatchLevel-1;

		// Create new node and assign as child ...
		checknode[nodeno].childno[3] = nextavailablenode;
		checknode[nodeno].numofchildren++;

		checknode[nextavailablenode].parentno = nodeno;
		checknode[nextavailablenode].color = checkcolor;
		checknode[nextavailablenode].x = checkx;
		checknode[nextavailablenode].y = checky;

		nextavailablenode++;
		getchildren( nextavailablenode - 1 );
	} // end if match found


CHECKSDONE:

	return checknode[nodeno].numofchildren;

} // end getchildren




// Returns TRUE if cannot find any more matches ...
BOOL GameBoard::nomorematches( void )
{
	char blockcolor[8];	// number of each block color left
	char blockcolormatchesneeded[8] =
		{3,3,3,3,3,3,3,3};	// number of each matches needed for each block color left
	char boardslot;		// 0-highest


	ZeroMemory( blockcolor, 8 );

	for( boardslot = 0; boardslot < mGridSize*mGridSize; boardslot++ )
	{
		if( mBoardTiles[boardslot].mIsActive && !mBoardTiles[boardslot].mIsBlock )
		{
			blockcolor[mBoardTiles[boardslot].mColor]++;
			if( blockcolormatchesneeded[mBoardTiles[boardslot].mColor] < mBoardTiles[boardslot].mMatchLevel )
				blockcolormatchesneeded[mBoardTiles[boardslot].mColor] = mBoardTiles[boardslot].mMatchLevel;
		} // end if block here

	} // end count all blocks


/*	sprintf( tempstr, "%d %d %d %d %d %d %d %d", blockcolor[0], blockcolor[1], blockcolor[2], blockcolor[3], blockcolor[4], blockcolor[5], blockcolor[6], blockcolor[7] );
	MessageBox( hwndmain, tempstr, "blox found", MB_OK );
*/

	bool isAtLeastOneMatch = false;
	bool isAtLeastOneWithoutMatch = false;


	for( boardslot = 0; boardslot < 8; boardslot++ )
	{
		if( blockcolor[boardslot] >= blockcolormatchesneeded[boardslot] )
		{
			isAtLeastOneMatch = true;
		}

		if( blockcolor[boardslot] < blockcolormatchesneeded[boardslot] && 
			blockcolor[boardslot] != 0 )
		{
			// mark bad blocks
			if( mCanScore )
			{
				GameApp *app = (GameApp*)gSexyAppBase;
				GameScreen *ts = app->mGameScreen;
				GameBoard *gb = app->mGameBoard;
				HGE::hgeParticleSystem *ps = NULL;

				for( int i=0; i<MAX_BOARD_TILES; i++ )
				{
					Tile *t = &(gb->mBoardTiles[i]);

					if( t->mIsActive && t->mIsBlock != true && t->mColor == boardslot )
					{
						app->mParticleManager->SpawnPS(
							"particles\\bad_block.psi",
							NULL,
							gb->Left() + t->Left() + (t->Width()>>1),
							gb->Top() + t->Top() + (t->Height()>>1));

						ps = app->mParticleManager->SpawnPS(
							"particles\\bad_block_hilite.psi",
							NULL,
							gb->Left() + t->Left() + (t->Width()>>1),
							gb->Top() + t->Top() + (t->Height()>>1));

						if( ps != NULL )
						{
							// set up random way points for highlight
							for( int i=0; i<16; i++ )
							{
								ps->mWayPoints.push_back(
									Sexy::Point(gb->Left() + t->Left() + (Sexy::Rand()%t->Width()),
									gb->Top() + t->Top() + (Sexy::Rand()%t->Height())));
							}

							ps->mPlayTime = 4;

							ps->Play();
						}

					}
				}

			} // end if mCanScore

			isAtLeastOneWithoutMatch = true;

		}

	}

	if( isAtLeastOneWithoutMatch == true )
	{
		return TRUE;
	}


	if( isAtLeastOneMatch == true )
	{
		return FALSE;
	}

	return TRUE;

} // end nomorematches


void GameBoard::RandomizeBoard(bool includeBlocks, int numberOfColors)
{
	int isGameOver = 0;
	int bgTileCountBucket[20];

	// SetArcadeSlideInDelay(1);
	// mGameScreen->mArcadeLevel = 1;

	//tileColors = (Rand() % 3) + 3;
	tileColors = numberOfColors;
	startingTileColor = (Rand() % 6);

	if( startingTileColor + tileColors > 8 )
	{
		startingTileColor = 8-tileColors;
//		tileColors = 8 - startingTileColor;
//		mApp->debugStream << "reset starting tile color to " << startingTileColor << std::endl;
	}

	if( tileColors > 8 ) tileColors = 8;
	if( startingTileColor < 0 ) startingTileColor = 0;

	for( int i=0; i<8; i++ )
	{
		mIsColorUsed[i] = false;
	}

//	for( int i=startingTileColor; i<startingTileColor+tileColors; i++ )
//	{
//		mIsColorUsed[i] = true;
//	}

//	mApp->debugStream << "starting tile color " << startingTileColor << ", tileColors " << tileColors << std::endl;

	for( int i=0; i<tileColors; i++ )
	{
		int color = Rand()%8;

		while( mIsColorUsed[color] == true )
			color = Rand()%8;

		mIsColorUsed[color] = true;

//		mApp->debugStream << i << ": Picked color " << color << std::endl;
	}

	emptySpaceChance = (Rand() % 5) + 3;
	wildTileChance = (Rand() % 20) + 2;

	emptyFillTileChance = 2;

	ZeroMemory( &bgTileCountBucket, 20*sizeof(int) );

	do
	{
		for( int tileNo = 0; tileNo < mGridSize*mGridSize; tileNo++ )
		{
			mBoardTiles[tileNo].mColor = (Rand() % 8);

			while( mIsColorUsed[mBoardTiles[tileNo].mColor] == false )
				mBoardTiles[tileNo].mColor = (Rand() % 8);

			if(mBoardTiles[tileNo].mColor >= 8) throw std::exception("BAD TILE COLOR");

			mBoardTiles[tileNo].mMatchLevel = 3;
			mBoardTiles[tileNo].mNumberOverlay->mDigit = -1;
			mBoardTiles[tileNo].mNumberOverlay->mIsActive = false;

			mBoardTiles[tileNo].mIsBlock = false;

			if( Rand() % emptySpaceChance > 0 )
			{
				bgTileCountBucket[mBoardTiles[tileNo].mColor]++;
				mBoardTiles[tileNo].mIsActive = true;

				if( Rand() % 15 == 0 )
				{
					switch( Rand() % 7 )
					{
					case 0:
					case 1:
					case 2:
						mBoardTiles[tileNo].mMatchLevel = 4;
						mBoardTiles[tileNo].mNumberOverlay->mDigit = 4;
						mBoardTiles[tileNo].mNumberOverlay->mIsActive = true;
						break;

					case 3:
						mBoardTiles[tileNo].mMatchLevel = 5;
						mBoardTiles[tileNo].mNumberOverlay->mDigit = 5;
						mBoardTiles[tileNo].mNumberOverlay->mIsActive = true;
						break;

					case 4:
					case 5:
					case 6:
						if( includeBlocks )
							mBoardTiles[tileNo].mIsBlock = true;
						break;


					}
				}

//<<<<<<< .mine
//
//				if( Rand() % wildTileChance == 0 )
//				{
//					mBoardTiles[tileNo].mIsWild = true;
//					mBoardTiles[tileNo].mWildTick = 0.0f;
//				}
//				else
//					mBoardTiles[tileNo].mIsWild = false;
//
//			}
//=======
			}
			else
			{
				mBoardTiles[tileNo].mIsActive = false;
				mBoardTiles[tileNo].mIsWild = false;
			}
		}

		isGameOver = CheckBoard(false);

	} while( isGameOver );

	for( int i = 0; i < 8; i++ )
	{
		if( bgTileCountBucket[i] < 3 )
		{
			((GameScreen*)mParent)->RemoveBackgroundTile(i);
		}
		else
		{
			((GameScreen*)mParent)->AddBackgroundTile(i,i,52*gGraphicsScale);
		}
	}

	SaveLevel(myLevel);

//	mApp->debugStream << "RandomizeBoard: numberOfColors " << numberOfColors << std::endl;

	RandomizeFillTiles();

//	FadeOutTiles();

	SetBackgroundTiles();

	/*
	for( int buttonNo = 0; buttonNo < mGridSize*4; buttonNo++ )
	{
		SliderButton* aButton;
		aButton = &mSliderButtons[buttonNo];

		if( aButton->mRotate == Direction::Down )
		{
			aButton->mIsAttractor = true;
		}

		mState = State::SlidingTiles;
		StartTileShift();
	}
	*/
}


void GameBoard::FocusStolen(void)
{
	if( mFocusedSliderNumber < mGridSize*4 )
	{
		mSliderButtons[mFocusedSliderNumber].mFadeState = -1;

		// todo: set to focus of button under cursor
		mFocusedSliderNumber = MAX_GRID_SIZE*4;
	}
}



void GameBoard::SetBackgroundTiles(void)
{
	int boardx, boardy;
	int bgTileCountBucket[20];

	ZeroMemory( &bgTileCountBucket, 20*sizeof(int) );

	for( boardy = 0; boardy < mGridSize; boardy++ )
	{
		for( boardx = 0; boardx < mGridSize; boardx++ )
		{
			int i = (boardy*mGridSize) + boardx;

			if( mBoardTiles[i].mIsActive && !mBoardTiles[i].mIsBlock )
			{
				bgTileCountBucket[mBoardTiles[i].mColor]++;
			}

		} // end boardx loop

	} // end boardy loop

	for( int i = 0; i < 8; i++ )
	{
		if( bgTileCountBucket[i] < 3 )
			((GameScreen*)mParent)->RemoveBackgroundTile(i);
		else
		{
			int tileSize = bgTileCountBucket[i]*2;
			if( tileSize < 12 ) tileSize = 12;
			if( tileSize > 52 ) tileSize = 52;

			((GameScreen*)mParent)->AddBackgroundTile(i,i,tileSize*gGraphicsScale);
		}
	}
}


void GameBoard::LoadLevel(Level &theLevel)
{
	//// check for intro levels
	//if( mGameScreen->mLevelPakName == gSexyAppBase->GetString("TUTORIAL_PAK_NAME", "Intro") )
	//{
	//	mApp->mWidgetManager->BringToFront(&mGameScreen->mText);

	//	switch( mApp->mLevelPak->mCurrentLevel )
	//	{
	//	case 0:
	//	case 1:
	//		mGameScreen->mText.SetText("Move the colored blox by clicking the arrows at the edges of the board.", 40);
	//		mLimitToSliderNumber = gSexyAppBase->GetInteger("INTRO_LEVEL_1_SLIDER_NUMBER_1",-1);
	//		for( int i=0;i<mGridSize*4;i++ )
	//		{
	//			if( i == mLimitToSliderNumber )
	//			{
	//				mSliderButtons[i].mMouseVisible = true;
	//				mSliderButtons[i].SetVisible(true);
	//			}
	//			else
	//			{
	//				mSliderButtons[i].mMouseVisible = false;
	//				mSliderButtons[i].SetVisible(false);
	//			}
	//		}
	//		break;

	//	case 2:
	//		mGameScreen->mText.SetText("Clear the board of all colored blox to advance to the next puzzle.", 40);
	//		break;

	//	case 3:
	//		mGameScreen->mText.SetText("Be careful not to leave just one or two blox of the same color on the board.", 40);
	//		break;

	//	case 4:
	//		mGameScreen->mText.SetText("Stone blox cannot be moved and never match other blox. ", 40);
	//		break;

	//	case 5:
	//		mGameScreen->mText.SetText("A tile with a number must be matched with at least that many blox to be removed.", 40);
	//		break;

	//	case 6:
	//		mGameScreen->mText.SetText("", 0);
	//		break;
	//	}

	//	// TODO: allow custom text positions!
	//	mGameScreen->mText.SetPosition(
	//		Left(),
	//		Top() + Height() + (32*gGraphicsScale));
	//		

	//}
	//else
	//{
	//	for( int i=0;i<mGridSize*4;i++ )
	//	{
	//		mSliderButtons[i].mMouseVisible = true;
	//		mSliderButtons[i].SetVisible(true);
	//	}

	//	mLimitToSliderNumber = -1;
	//	mGameScreen->mText.SetText("", 0);

	//	mApp->mWidgetManager->BringToBack(&mGameScreen->mText);
	//}

	// Copy to local level member
	memcpy(&myLevel, &theLevel, sizeof(Level));

	//int tileColors = (Rand() % 3) + 2;
	//int startingTileColor = (Rand() % 7);
	//if( startingTileColor + tileColors > 7 )
	//{
	//	tileColors = 7 - startingTileColor;
	//}
	//if( tileColors > 8 ) tileColors = 8;
	//if( startingTileColor < 0 ) startingTileColor = 0;
	//int emptySpaceChance = (Rand() % 5) + 3;
}

void GameBoard::ChangeToNewTiles(void)
{
	int bgTileCountBucket[20];
	int isGameOver = 0;

	ZeroMemory( &bgTileCountBucket, 20*sizeof(int) );

	mGridSize = myLevel.mGridSize;

	for( int tileNo = 0; tileNo < mGridSize*mGridSize; tileNo++ )
	{
		mBoardTiles[tileNo].mColor = myLevel.mTiles[tileNo];
		mBoardTiles[tileNo].mMatchLevel = myLevel.mMatchLevel[tileNo];
		mBoardTiles[tileNo].mIsBlock = myLevel.mBlock[tileNo];
		if( mBoardTiles[tileNo].mMatchLevel == 3 )
		{
			mBoardTiles[tileNo].mNumberOverlay->mDigit = -1;
			mBoardTiles[tileNo].mNumberOverlay->mIsActive = false;
		}
		else
		{
			mBoardTiles[tileNo].mNumberOverlay->mDigit = mBoardTiles[tileNo].mMatchLevel;
			mBoardTiles[tileNo].mNumberOverlay->mIsActive = true;
		}
		

		if(mBoardTiles[tileNo].mColor >= 8) throw std::exception("BAD TILE COLOR");

		if( mBoardTiles[tileNo].mColor != -1 )
		{
			mBoardTiles[tileNo].mIsActive = true;
			bgTileCountBucket[mBoardTiles[tileNo].mColor]++;
		}
		else
		{
			mBoardTiles[tileNo].mIsActive = false;
		}
	}

//	isGameOver = CheckBoard(false);

	for( int i = 0; i < 8; i++ )
	{
		if( bgTileCountBucket[i] < 3 )
		{
			((GameScreen*)mParent)->RemoveBackgroundTile(i);
		}
		else
		{
			((GameScreen*)mParent)->AddBackgroundTile(i,i,52*gGraphicsScale);
		}
	}

	SetBackgroundTiles();

	FadeInTiles();

	mGameScreen->mLevelPakName = mGameScreen->mNewLevelPakName;
	mApp->mLevelPak->mCurrentLevel = mApp->mLevelPak->mNewCurrentLevel;


	// check for intro levels
	if( mGameScreen->mLevelPakName == gSexyAppBase->GetString("TUTORIAL_PAK_NAME", "How To Play") )
	{
		mApp->mWidgetManager->BringToFront(&mGameScreen->mText);

		switch( mApp->mLevelPak->mCurrentLevel )
		{
		case 0:
		case 1:
			mGameScreen->mText.SetText("Move the colored blox by clicking the arrows at the edges of the board.", 40);
			mLimitToSliderNumber = gSexyAppBase->GetInteger("INTRO_LEVEL_1_SLIDER_NUMBER_1",-1);
			for( int i=0;i<mGridSize*4;i++ )
			{
				if( i == mLimitToSliderNumber )
				{
					mSliderButtons[i].mMouseVisible = true;
					mSliderButtons[i].SetVisible(true);
				}
				else
				{
					mSliderButtons[i].mMouseVisible = false;
					mSliderButtons[i].SetVisible(false);
				}
			}
			break;

		case 2:
			for( int i=0;i<mGridSize*4;i++ )
			{
				mSliderButtons[i].mMouseVisible = true;
				mSliderButtons[i].SetVisible(true);
			}

			mLimitToSliderNumber = -1;
			mGameScreen->mText.SetText("You can also use the keyboard arrow keys and the space bar to slide the blox.", 40);
			break;

		case 3:
			for( int i=0;i<mGridSize*4;i++ )
			{
				mSliderButtons[i].mMouseVisible = true;
				mSliderButtons[i].SetVisible(true);
			}

			mLimitToSliderNumber = -1;
			mGameScreen->mText.SetText("Be careful not to leave just one or two blox of the same color on the board.", 40);
			break;

		case 4:
			for( int i=0;i<mGridSize*4;i++ )
			{
				mSliderButtons[i].mMouseVisible = true;
				mSliderButtons[i].SetVisible(true);
			}

			mLimitToSliderNumber = -1;
			mGameScreen->mText.SetText("Stone blox cannot be moved and never match other blox. ", 40);
			break;

		case 5:
			for( int i=0;i<mGridSize*4;i++ )
			{
				mSliderButtons[i].mMouseVisible = true;
				mSliderButtons[i].SetVisible(true);
			}

			mLimitToSliderNumber = -1;
			mGameScreen->mText.SetText("A block with a number must be matched with at least that many blox to be removed.", 40);
			break;

		case 6:
			for( int i=0;i<mGridSize*4;i++ )
			{
				mSliderButtons[i].mMouseVisible = true;
				mSliderButtons[i].SetVisible(true);
			}

			mLimitToSliderNumber = -1;
			mGameScreen->mText.SetText("", 0);
			break;
		}

		// TODO: allow custom text positions!
		mGameScreen->mText.SetPosition(
			mHomeX,
			mHomeY + Height() + (30*gGraphicsScale));
			

	}
	else
	{
		for( int i=0;i<mGridSize*4;i++ )
		{
			mSliderButtons[i].mMouseVisible = true;
			mSliderButtons[i].SetVisible(true);
		}

		mLimitToSliderNumber = -1;
		mGameScreen->mText.SetText("", 0);

		mApp->mWidgetManager->BringToBack(&mGameScreen->mText);
	}


}

void GameBoard::SaveLevel(Level &theLevel)
{
	mGridSize = theLevel.mGridSize;

	for( int tileNo = 0; tileNo < mGridSize*mGridSize; tileNo++ )
	{
		theLevel.mTiles[tileNo] = mBoardTiles[tileNo].mColor;
		theLevel.mMatchLevel[tileNo] = mBoardTiles[tileNo].mMatchLevel;
		theLevel.mBlock[tileNo] = (mBoardTiles[tileNo].mIsBlock ? 1 : 0 );

		if(mBoardTiles[tileNo].mColor >= 8) throw std::exception("BAD TILE COLOR");

		if( mBoardTiles[tileNo].mIsActive == false )
		{
			theLevel.mTiles[tileNo] = -1;
			theLevel.mMatchLevel[tileNo] = 3;
			theLevel.mBlock[tileNo] = 0;
		}
	}
}


void GameBoard::CreateBoardObjects(void)
{
	// create slider buttons and tiles
	SliderButton* aButton;
	int buttonIndex = 0;

	float cellSize = (614.4f*gGraphicsScale) / (float)(mGridSize+2);

	int tileNo = 0;

	for( int y = 0; y < mGridSize + 2; y++ )
	{
		for( int x = 0; x < mGridSize + 2; x++ )
		{
			// top
			if( y == 0 && x != 0 && x != mGridSize + 1 )
			{
				aButton = &mSliderButtons[buttonIndex];

				if( aButton->mWidgetManager != NULL )
					RemoveWidget(aButton);

				aButton->mX = x * cellSize;
				aButton->mY = 0;

				aButton->mRotate = 0;

				aButton->mColumn = x - 1;
				aButton->mRow = y;

				AddWidget( &mSliderButtons[buttonIndex++] );
			}
			// bottom
			else if( y == mGridSize + 1 && x != 0 && x != mGridSize + 1 )
			{
				aButton = &mSliderButtons[buttonIndex];

				aButton->mX = x * cellSize;
				aButton->mY = y * cellSize;

				aButton->mRotate = 2;

				aButton->mColumn = x - 1;
				aButton->mRow = y - 1;

				AddWidget( &mSliderButtons[buttonIndex++] );
			}
			// left
			else if( x == 0 && y != 0 && y != mGridSize + 1 )
			{
				aButton = &mSliderButtons[buttonIndex];

				aButton->mX = 0;
				aButton->mY = y * cellSize;

				aButton->mRotate = 3;

				aButton->mColumn = x;
				aButton->mRow = y - 1;

				AddWidget( &mSliderButtons[buttonIndex++] );
			}
			// right
			else if( x == mGridSize + 1 && y != 0 && y != mGridSize + 1 )
			{
				aButton = &mSliderButtons[buttonIndex];

				aButton->mX = x * cellSize;
				aButton->mY = y * cellSize;

				aButton->mRotate = 1;

				aButton->mColumn = x - 1;
				aButton->mRow = y - 1;

				AddWidget( &mSliderButtons[buttonIndex++] );
			}
			else if( x != 0 && x != mGridSize + 1 && y != 0 && y != mGridSize + 1 )
			{
				// board tile
				mBoardTiles[tileNo].mPosition.x = x * cellSize;
				mBoardTiles[tileNo].mPosition.y = y * cellSize;

				mBoardTiles[tileNo].mHomeX = mBoardTiles[tileNo].mPosition.x;
				mBoardTiles[tileNo].mHomeY = mBoardTiles[tileNo].mPosition.y;

				mBoardTiles[tileNo].mImage = IMAGE_BLOCKS;

				mBoardTiles[tileNo].mColor = 0;

				mBoardTiles[tileNo].mIsBoardTile = true;

				mBoardTiles[tileNo].mIsActive = false;

				AddWidget(&mBoardTiles[tileNo]);

#ifdef MATCHBLOX_ASSERT
if(tileNo == mGridSize*mGridSize) MessageBox(NULL, "BAD TILE", "BAD TILE", MB_OK);
#endif
				tileNo++;
			}
		}
	}
}



void GameBoard::StartTileShiftByOne(Direction theDirection)
{
	bool didHitTop = false;
	SliderButton* aButton;

	float tileSlideAcceleration = (float)gSexyAppBase->GetDouble(
		"TILE_SLIDE_ACCELERATION", 3.0f);
	float tileSlideAccelerationFalloff = (float)gSexyAppBase->GetDouble(
		"TILE_SLIDE_ACCELERATION_FALLOFF", 0.2f);

	for( int column = 0; column<mGridSize; column++ )
	{
		Tile* aTile;
		Tile* aBlockingTile;
		int tileNumber = 0;

		// reset counter
		int freeSpaces = 0;

		switch( theDirection )
		{
		case Direction::Up:
			// start shift
			if( !mGameScreen->mFillTiles[column]->mIsActive ) continue;

			for( int i=0; i < mGridSize; i++ )
			{
				aButton = &mSliderButtons[i];
				aButton->mIsAttractor = true;
			}

			for( int row = mGridSize-1; row >= 0; row-- )
			{
				freeSpaces = 0;

				aTile = &mBoardTiles[(row*mGridSize)+column];

				if( aTile->mIsActive &&
					!aTile->mIsBlock &&
					mGameScreen->mFillTiles[column]->mIsActive &&
					mGameScreen->mFillTiles[column]->mColor != -1)
				{

					int checkRow = row - 1;

					// TODO: LOSE GAME HERE!
					if( row == 0 )
					{
						didHitTop = true;

						// mark bad blocks

						GameApp *app = (GameApp*)gSexyAppBase;
						GameScreen *ts = app->mGameScreen;
						GameBoard *gb = app->mGameBoard;
						HGE::hgeParticleSystem* ps = NULL;

						for( int b=column; b<mGridSize*mGridSize; b+=mGridSize )
						{
							Tile *t = &(gb->mBoardTiles[b]);

							app->mParticleManager->SpawnPS(
								"particles\\bad_block.psi",
								NULL,
								gb->Left() + t->Left() + (t->Width()>>1),
								gb->Top() + t->Top() + (t->Height()>>1));

							ps = app->mParticleManager->SpawnPS(
								"particles\\bad_block_hilite.psi",
								NULL,
								gb->Left() + t->Left() + (t->Width()>>1),
								gb->Top() + t->Top() + (t->Height()>>1));

							if( ps != NULL )
							{
								// set up random way points for highlight
								for( int i=0; i<16; i++ )
								{
									ps->mWayPoints.push_back(
										Sexy::Point(gb->Left() + t->Left() + (Sexy::Rand()%t->Width()),
										gb->Top() + t->Top() + (Sexy::Rand()%t->Height())));
								}

								ps->mPlayTime = 4;

								ps->Play();
							}

						}

						break;

//						aTile->mIsActive = false;
//						continue;
					}


					aTile->mAcceleration.y = ((0-tileSlideAcceleration) + (row*tileSlideAccelerationFalloff))
						* gGraphicsScale;
					aTile->mDirection = Direction::Up;
					aTile->mIsSliding = true;
					aTile->mShiftCount = 1;
					aTile->mThresholdY = aTile->mPosition.y - aTile->mHeightF;


					aBlockingTile = &mBoardTiles[(checkRow*mGridSize)+column];

					if( aBlockingTile->mIsActive && !aBlockingTile->mIsSliding)
					{
						if( aBlockingTile->mIsBlock )
						{
							// TODO: LOSE GAME HERE!
							continue;
						}
					}
					else
					{
						freeSpaces++;
						break;
					}
				}
				else
				{
					break;
				}
			}

			break;

		}

		if( didHitTop )
		{
			for( int i = 0; i < mGridSize * mGridSize; i++ )
			{
				mBoardTiles[i].mDirection = Direction::None;
				mBoardTiles[i].mIsSliding = false;
				mBoardTiles[i].mShiftCount = 0;

				mBoardTiles[i].Stop();
				mBoardTiles[i].mPosition.x = mBoardTiles[i].mHomeX;
				mBoardTiles[i].mPosition.y = mBoardTiles[i].mHomeY;
			}

			// deactivate all sliders
			for( int i=0; i<mGridSize; i++ )
			{
				mSliderButtons[i].mIsAttractor = false;
			}
			
			mGameScreen->LoseGame();
			
			break;
		}
	}
}


void GameBoard::RandomizeFillTiles(void)
{
	Tile* aTile;

	int baseLevel = mGameScreen->mArcadeLevel % 10;

	int chanceOf4Tile = baseLevel >> 1;
	int chanceOf5Tile = baseLevel >> 2;

	int numberedTileCheck = 0;


	for( int i=0; i<mGridSize; i++ )
	{
		aTile = mGameScreen->mFillTiles[i];

		aTile->mColor = Rand()%8;
		while( !mIsColorUsed[aTile->mColor] )
			aTile->mColor = Rand()%8;

		if( i > 0 )
		{
			while( mGameScreen->mFillTiles[i-1]->mColor == aTile->mColor )
			{
				aTile->mColor = Rand()%8;
				while( !mIsColorUsed[aTile->mColor] )
					aTile->mColor = Rand()%8;
			}
		}

		aTile->mMatchLevel = 3;
		aTile->mNumberOverlay->mDigit = -1;

		numberedTileCheck = Rand(80);

		if( numberedTileCheck < chanceOf5Tile ) 
		{
			aTile->mMatchLevel = 5;
			aTile->mNumberOverlay->mDigit = 5;
		}
		else
		{
			if( numberedTileCheck < chanceOf4Tile ) 
			{
				aTile->mMatchLevel = 4;
				aTile->mNumberOverlay->mDigit = 4;
			}
		}

//		mApp->debugStream << "RandomizeFillTiles: i " << i << ", ArcadeLevel " << mGameScreen->mArcadeLevel << ", Color " << aTile->mColor << std::endl;
	}
}


void GameBoard::SetArcadeSlideInDelay(int theLevel)
{
	int baseslideindelay = 
		gSexyAppBase->GetInteger("BASE_SLIDE_IN_DELAY",600);
	int maxslideindelay = 
		gSexyAppBase->GetInteger("FASTEST_SLIDE_IN_DELAY",100);
	int delayincreaseperlevel = 
		gSexyAppBase->GetInteger("DELAY_INCREASE_PER_LEVEL",50);
	int numberoflevelsbeforewraparound = 
		gSexyAppBase->GetInteger("NUMBER_OF_LEVELS_BEFORE_RESTART",10);

	mArcadeSlideInDelay = baseslideindelay - ((theLevel%numberoflevelsbeforewraparound) * delayincreaseperlevel);
	if( mArcadeSlideInDelay < maxslideindelay ) mArcadeSlideInDelay = maxslideindelay;
}

void GameBoard::AddNewFillColor()
{
	int i;

	for(i=0; i<8; i++)
	{
		if( !mIsColorUsed[i] ) break;
	}

	if( i!=8 )
	{
		int newTileColor = Rand()%8;
		if( tileColors < 8 )
		{
			while( mIsColorUsed[newTileColor] )
				newTileColor = Rand()%8;

			mIsColorUsed[newTileColor] = true;
			tileColors++;
//			mApp->debugStream << "AddNewFillColor: tileColors" << tileColors << ", newTileColor" << newTileColor << std::endl;
		}
	}
}


void GameBoard::FadeInTiles(void)
{
	mState = State::FadeIn;
	mTileTransitionEndTime = mTimer + 25.0f;

	for( int i=0; i<MAX_BOARD_TILES; i++ )
	{
		mBoardTiles[i].mPosition.z = 0.0f;
		mBoardTiles[i].mVelocity.z = 0.04f;
	}

	// fade in board
	mFadeTimer = 0.0f;
	Stop();
	mPosition.z = 0.0f;
	mPosition.y = mHomeY - (21 * gGraphicsScale);
	mVelocity.y = 6.4f * gGraphicsScale;
	mAcceleration.y = -1.20f * gGraphicsScale;
	mAcceleration.z = 0.002f;
}


void GameBoard::FadeOutTiles(void)
{
	mState = State::TileDisappear;
	mTileTransitionEndTime = mTimer + 25.0f;

	for( int i=0; i<MAX_BOARD_TILES; i++ )
	{
		mBoardTiles[i].mPosition.z = 1.0f;
		mBoardTiles[i].mVelocity.z = -0.04f;
	}
}


void GameBoard::ClearTiles(void)
{
	for( int i=0; i<mApp->mGameBoard->mGridSize*mApp->mGameBoard->mGridSize; i++)
	{
		mApp->mGameBoard->mBoardTiles[i].mIsActive = false;
		mApp->mGameBoard->mBoardTiles[i].mColor = -1;
		mApp->mGameBoard->mBoardTiles[i].mIsBlock = false;
		mApp->mGameBoard->mBoardTiles[i].mMatchLevel = 3;
	}
}