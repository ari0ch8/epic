#include "SliderButton.h"
#include "..//sexyappframework//sexyappbase.h"
#include "..//sexyappframework//widgetmanager.h"
#include "res.h"
#include "gameboard.h"
#include "GameScreen.h"
#include "Globals.h"

using namespace Sexy;

SliderButton::SliderButton(void)
{
	mWidth = gSexyAppBase->GetDouble("SLIDER_BUTTON_WIDTH", 51.2f)
		* gGraphicsScale;
	mHeight = gSexyAppBase->GetDouble("SLIDER_BUTTON_HEIGHT", 51.2f)
		* gGraphicsScale;

	mRotate = 0;
	mBlendColor = Color::White;
	mHoverBlendColor = Color::White;
	mHoverBlendColor.mAlpha = 0;

	mBoardRowColor = Color(
		gSexyAppBase->GetInteger("BOARD_ROW_HOVER_R", 255),
		gSexyAppBase->GetInteger("BOARD_ROW_HOVER_G", 255),
		gSexyAppBase->GetInteger("BOARD_ROW_HOVER_B", 255),
		gSexyAppBase->GetInteger("BOARD_ROW_HOVER_A", 64) );

	mAnimateCurrentTime = 0.0f;
	mAnimationLength = gSexyAppBase->GetDouble("TILE_SLIDE_ANIMATION_LENGTH", 50.0f);

	mFadeState = 0;
	mFadeLevel = 0;

	mIsAttractor = false;

	mDoFinger = true;
}

SliderButton::~SliderButton(void)
{
//	mParent = NULL;
}


void SliderButton::Draw(Graphics* g)
{
	Image* aSliderButtonImage;
	Image* aSliderButtonHoverImage;
	Rect destRect;

	if( ((GameBoard*)mParent)->mStoredState != GameBoard::State::EditingLevel )
	{
		destRect.mX = 0;
		destRect.mY = 0;

		destRect.mWidth = this->Width();
		destRect.mHeight = this->Height();

		g->SetColorizeImages(true);

		// draw button
		mBlendColor.mAlpha = (int)(255.0f * ((GameBoard*)mParent)->mPosition.z);
		if( mBlendColor.mAlpha > 255 )
			mBlendColor.mAlpha = 255;
		if( mBlendColor.mAlpha < 0 )
			mBlendColor.mAlpha = 0;

		g->SetColor(mBlendColor);

		if( mHoverBlendColor.mAlpha != 255 || ((GameBoard*)mParent)->mState == GameBoard::State::FadeOut)
			g->DrawImageCel(IMAGE_SLIDER_BUTTON, destRect, mRotate, 0);
			
		g->SetColor(mHoverBlendColor);

		if( mHoverBlendColor != Color::Black && ((GameBoard*)mParent)->mState != GameBoard::State::FadeOut)
		{
			g->SetColor(mHoverBlendColor);
			g->DrawImageCel(IMAGE_SLIDER_BUTTON, destRect, mRotate, 1);
		}

		g->SetColorizeImages(false);
	}

}

void SliderButton::DrawBoardHover(Graphics* g)
{
	switch( ((GameBoard*)mParent)->mState )
	{
	case GameBoard::State::WaitingForUser:
		g->SetColor(mBoardRowCurrentColor);

		switch( mRotate )
		{
		case 0:
		case 2:
			g->FillRect(
				Left(), Height(),
				Width(), Height() * ((GameBoard*)mParent)->mGridSize);
			break;

		case 1:
		case 3:
			g->FillRect(
				Width(), Top(),
				Width() * ((GameBoard*)mParent)->mGridSize, Height());
			break;

		}

		break;
	}

}

void SliderButton::Update(void)
{
	Widget::Update();

	// TODO: change type to optimize
	float theFrac = 1.25;

	int colorLevel;

	switch( mFadeState )
	{
	case 1:	// fading in
		mFadeLevel += theFrac * 12;
		if( mFadeLevel > 255)
		{
			mFadeLevel = 255;
		}
		colorLevel = mFadeLevel;

		mHoverBlendColor = Color( 255, 255, 255, colorLevel );

		mBoardRowCurrentColor = mBoardRowColor;
		mBoardRowCurrentColor.mAlpha = mBoardRowColor.mAlpha +
			(mBoardRowColor.mAlpha *
			(0.5f - ((mFadeLevel/255) * theFrac * 0.66f)));
		break;

	case -1: // fading out

		mFadeLevel -= theFrac * 12;
		if( mFadeLevel < 0)
		{
			mFadeLevel = 0;
			mFadeState = 0;
		}

		colorLevel = mFadeLevel;

		mHoverBlendColor = Color( 255, 255, 255, colorLevel );

		mBoardRowCurrentColor = mBoardRowColor;
		mBoardRowCurrentColor.mAlpha = mBoardRowColor.mAlpha +
			(mBoardRowColor.mAlpha *
			(0.5f - ((mFadeLevel/255) * theFrac * 0.66f)));
		break;

	case 0:

//		if( alpha > ((GameApp*)gSexyAppBase)->mGameScreen->mTitleAlpha )
//			alpha = ((GameApp*)gSexyAppBase)->mGameScreen->mTitleAlpha;

		//if( gPauseLevel )// || ((GameApp*)gSexyAppBase)->mGameScreen->mMovie->mWidgetManager != NULL )
		//	mBlendColor = Color(128,128,128,((GameBoard*)mParent)->mAlpha);
		//else
			mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);

		//mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);
		mHoverBlendColor = Color::White;
		mHoverBlendColor.mAlpha = 0;
		break;
	}

	switch( ((GameBoard *)mParent)->mState )
	{
	case GameBoard::State::SlidingTiles:
		mAnimateCurrentTime += theFrac;

		if( mIsAttractor ) mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);

//		if( mAnimateCurrentTime > mAnimationLength )
//		{
//			((GameBoard *)mParent)->mState = GameBoard::State::WaitingForUser;
////			ShowFinger(true);
//			mAnimateCurrentTime = 0.0f;
//			mAnimationLength = gSexyAppBase->GetDouble("TILE_SLIDE_ANIMATION_LENGTH", 50.0f);
//
//		}

		break;

	case GameBoard::State::WaitingForUser:
		mAnimateCurrentTime = 0.0f;
		// mAnimationLength = gSexyAppBase->GetDouble("TILE_SLIDE_ANIMATION_LENGTH", 50.0f);
		break;

	}

	//mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);
}

void SliderButton::MouseEnter()
{
	mWidgetManager->SetFocus(this);
	mFadeState = 1;

	if( ((GameBoard *)mParent)->mState != GameBoard::State::WaitingForUser	)
	{
		mFadeLevel = 0;
	}

	((GameBoard *)mParent)->FocusStolen();

}

void SliderButton::MouseLeave()
{
	mWidgetManager->SetFocus((Widget *)(this->mParent));
	mFadeState = -1;
}

void SliderButton::MouseDown(int x, int y, int theClickCount)
{
	if( ((GameBoard *)mParent)->mState == GameBoard::State::WaitingForUser)
	{
		gSexyAppBase->PlaySample(SOUND_BUTTON);

		mIsAttractor = true;

		((GameBoard *)mParent)->mState = GameBoard::State::SlidingTiles;
		((GameBoard *)mParent)->StartTileShift();


		// first intro level special case
		if( ((GameBoard *)mParent)->mLimitToSliderNumber == gSexyAppBase->GetInteger("INTRO_LEVEL_1_SLIDER_NUMBER_1",-2) )
		{
			((GameBoard *)mParent)->mGameScreen->mText.SetText("Match 3 or more blox of the same color to remove them from the board.", 40);
			//((GameBoard *)mParent)->mGameScreen->mText.SetPosition(gSexyAppBase->GetInteger("BACKGROUND_TILES_LEFT") * gGraphicsScale,
			//	(gSexyAppBase->GetInteger("BACKGROUND_TILES_TOP") * gGraphicsScale) +
			//	FONT_DEFAULT->GetAscent());
			((GameBoard *)mParent)->mGameScreen->mText.SetPosition(
				((GameBoard *)mParent)->Left(),
				((GameBoard *)mParent)->Top() + ((GameBoard *)mParent)->Height() + (30*gGraphicsScale));

			((GameBoard *)mParent)->mLimitToSliderNumber = gSexyAppBase->GetInteger("INTRO_LEVEL_1_SLIDER_NUMBER_2",-1);
			for( int i=0;i<((GameBoard *)mParent)->mGridSize*4;i++ )
			{
				if( i == ((GameBoard *)mParent)->mLimitToSliderNumber )
				{
					((GameBoard *)mParent)->mSliderButtons[i].mMouseVisible = true;
					((GameBoard *)mParent)->mSliderButtons[i].SetVisible(true);
				}
				else
				{
					((GameBoard *)mParent)->mSliderButtons[i].mMouseVisible = false;
					((GameBoard *)mParent)->mSliderButtons[i].SetVisible(false);
				}
			}

		}
		else if( ((GameBoard *)mParent)->mLimitToSliderNumber == gSexyAppBase->GetInteger("INTRO_LEVEL_1_SLIDER_NUMBER_2",-2) )
		{
			((GameBoard *)mParent)->mGameScreen->mText.SetText("Match 3 or more blox of the same color to remove them from the board.", 40);
			//((GameBoard *)mParent)->mGameScreen->mText.SetPosition(gSexyAppBase->GetInteger("BACKGROUND_TILES_LEFT") * gGraphicsScale,
			//	(gSexyAppBase->GetInteger("BACKGROUND_TILES_TOP") * gGraphicsScale) +
			//	FONT_DEFAULT->GetAscent());
			((GameBoard *)mParent)->mGameScreen->mText.SetPosition(
				((GameBoard *)mParent)->Left(),
				((GameBoard *)mParent)->Top() + ((GameBoard *)mParent)->Height() + (30*gGraphicsScale));

			((GameBoard *)mParent)->mLimitToSliderNumber = -1;

			for( int i=0;i<((GameBoard *)mParent)->mGridSize*4;i++ )
			{
				((GameBoard *)mParent)->mSliderButtons[i].mMouseVisible = true;
				((GameBoard *)mParent)->mSliderButtons[i].SetVisible(true);
			}
		}

		// handle change from keyboard keys help to clear all tiles
		if( ((GameApp*)gSexyAppBase)->mLevelPak->mCurrentLevel == 2 )
		{
			((GameBoard *)mParent)->mGameScreen->mText.SetText("Clear the board of all colored blox to advance to the next puzzle.", 40);

			// TODO: allow custom text positions!
			((GameBoard *)mParent)->mGameScreen->mText.SetPosition(
				((GameBoard *)mParent)->mHomeX,
				((GameBoard *)mParent)->mHomeY + ((GameBoard *)mParent)->Height() + (30*gGraphicsScale));

		}


		// play sound if not the monkey
		// TODO: reenable with GOOD sound effect
//		if(!((GameBoard *)mParent)->mIsMonkey)
//			gSexyAppBase->PlaySample(SOUND_PLANET);


		// TODO: hide all fingers
		//((GameBoard *)mParent)->ShowFinger(false);

//		mWidgetManager->SetFocus((Widget *)(this->mParent));
//		ShowFinger(false);
	}
}

void SliderButton::KeyDown( KeyCode theKey )
{
	((GameBoard *)mParent)->KeyDown(theKey);
}