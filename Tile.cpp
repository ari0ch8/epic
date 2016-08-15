#include "../SexyAppFramework/SexyAppBase.h"
#include "Tile.h"
#include "GameBoard.h"
#include "res.h"
#include "globals.h"

#include "GameScreen.h"
#include "gameapp.h"

using namespace Sexy;

Tile::Tile(void)
{
	mIsBoardTile = false;
	mIsEditTile = false;
	mIsWild = false;
	mWildTick = 0.0f;
	mIsBlock = false;

	mMatchLevel = 3;
	mNumberOverlay = new Digit();
	mNumberOverlay->mImage = IMAGE_NUMBERS;
	mNumberOverlay->mDigit = -1;
	mNumberOverlay->mIsActive = false;

	mWidth = gSexyAppBase->GetDouble("BOARD_TILE_WIDTH", IMAGE_BLOCKS->mHeight)
		* gGraphicsScale;
	mHeight = gSexyAppBase->GetDouble("BOARD_TILE_HEIGHT", IMAGE_BLOCKS->mHeight)
		* gGraphicsScale;

	mWidthF = (float)mWidth;
	mHeightF = (float)mHeight;

	mForce = NULL;
	mParticleSystem = NULL;

	mBlendColor = Color(Color::White);

	mIsSliding = false;
	mShiftCount = 0;
	Stop();
}

Tile::~Tile(void)
{
//	if( mForce != NULL )
//		delete mForce;

	if( mNumberOverlay != NULL )
		delete mNumberOverlay;
}

void Tile::Draw(Graphics* g)
{
	Rect rect;

	if( mPosition.z < 1.0f )
	{
		// scale with z value
		float size = (float)mWidth * (0.25f + (mPosition.z *0.75f));
		if( size < 0.0f ) size = 0.0f;
		float halfSize = size / 2.0f;
		float halfWidth = ((float)mWidth/2.0f);

		rect.mX = halfWidth - halfSize;
		rect.mY = halfWidth - halfSize;
		rect.mWidth = size;
		rect.mHeight = size;
	}
	else
	{
		rect.mX = 0;
		rect.mY = 0;
		rect.mWidth = mWidth;
		rect.mHeight = mHeight;
	}

#ifdef MATCHBLOX_ASSERT
sprintf(debug,"mColor: %d",mColor);
Assert(mColor < 8, "BAD TILE COLOR");
#endif

	if( mIsActive )
	{
		int alpha = 255;

		if( mPosition.z < 1 )
		{
			if( mPosition.z < 0 )
			{
				alpha = 0;
			}
			else
			{
				alpha = 255 * mPosition.z;
			}
		}

		if( alpha > ((GameApp*)gSexyAppBase)->mGameScreen->mTitleAlpha )
			alpha = ((GameApp*)gSexyAppBase)->mGameScreen->mTitleAlpha;

		//if( gPauseLevel )// || ((GameApp*)gSexyAppBase)->mGameScreen->mMovie->mWidgetManager != NULL )
		//	mBlendColor = Color(128,128,128,(int)alpha);
		//else
		//{
			mBlendColor = Color(255,255,255,(int)alpha);
		//}

		g->SetColor(mBlendColor);
		g->SetColorizeImages(true);

		if( mIsBlock )
			g->DrawImageCel(mImage, rect, 2, 1);
		else
		{
			g->DrawImageCel(mImage, rect, mColor);

			if( mNumberOverlay != NULL )
			{
				if( mNumberOverlay->mDigit == 4 )
					g->DrawImageCel(mImage, rect, 0, 1);
				if( mNumberOverlay->mDigit == 5 )
					g->DrawImageCel(mImage, rect, 1, 1);
			}
		}
		
		g->SetColorizeImages(false);
	}

	//Digit aDigit;
	//aDigit.Resize(rect);
	//aDigit.mDigit = mMatchLevel;
	//aDigit.Scale(0.4f);
	//aDigit.mIsActive = true;
	//aDigit.mImage = IMAGE_NUMBERS;
	//aDigit.mBlendColor = Color(255,255,255,64);
	//aDigit.mPosition.z = 0.5f;
	//aDigit.Draw(g);

}


void Tile::MouseMove(int x, int y)
{
//	if( mIsBoardTile == true )
//	{
//		((GameBoard*)mParent)->mEditTileSelected = this;
//		//((GameBoard*)mParent)->mEditTile.Move(Left()+(Width()/2), Top()+(Height()/2));
//		//((GameBoard*)mParent)->mEditTile.mPosition.x = Left()+(Width()/2)+x;
//		//((GameBoard*)mParent)->mEditTile.mPosition.y = Top()+(Height()/2)+y;
//		((GameBoard*)mParent)->mGameScreen->mEditTile.mPosition.x = 
//			((GameBoard*)mParent)->Left() + Left()+(Width()/2)+x;
//		((GameBoard*)mParent)->mGameScreen->mEditTile.mPosition.y =
//			((GameBoard*)mParent)->Top() + Top()+(Height()/2)+y;
////		((GameBoard*)mParent)->mGameScreen->mCursorForce.mSource.x =
////			Left() + ((GameBoard*)mParent)->Left() + x + (40*gGraphicsScale);
////		((GameBoard*)mParent)->mGameScreen->mCursorForce.mSource.y =
////			Top() + ((GameBoard*)mParent)->Top() + y + (40*gGraphicsScale);
//
//	}
//<<<<<<< .mine
}

void Tile::MouseDown(int x, int y, int theClickCount)
{
	//if( mIsEditTile )
	//{
	//	((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mColor = mColor;
	//	((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mIsBlock = false;
	//}

	//if( mIsBoardTile && ((GameApp*)gSexyAppBase)->mGameBoard->mState == GameBoard::State::EditingLevel )
	//{
	//	if( mColor == ((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mColor )
	//	{
	//		mColor = -1;
	//		mIsActive = false;
	//		mIsBlock = false;
	//		mMatchLevel = 3;
	//	}
	//	else
	//	{
	//		mColor = ((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mColor;
	//		mIsBlock = ((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mIsBlock;
	//		mMatchLevel = ((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mMatchLevel;
	//		mNumberOverlay->mDigit = ((GameApp*)gSexyAppBase)->mGameScreen->mEditTile.mNumberOverlay->mDigit;
	//		mIsActive = true;
	//	}
	//}
}


void Tile::Update(void)
{
	Floater::Update();

	// TODO: change type to optimize
	float theFrac = 1.25;

	if( mIsWild )
	{
		mWildTick += theFrac;
		mColor = ((((int)mWildTick%80))/10)%8;
		mColor = (int)mWildTick%8;
	}

	if( mForce != NULL && mForce->mActive )
	{
		mForce->UpdateFloater(*this, theFrac);
	}

	if(mPosition.z < 0)
	{
		gSexyAppBase->mWidgetManager->RemoveWidget(this);
		mIsActive = false;
		mForce = NULL;
	}

	// check for particle update
	if( mParticleSystem != NULL )
	{
		mParticleSystem->FireAt(
			this->Left() + (Sexy::Rand()%this->Width()),
			this->Top() + (Sexy::Rand()%this->Height()));
	}


//=======
}

//
//void Tile::Update(void)
//{
//	MarkDirty();
//	//if( mForce->mActive )
//	//{
//	//	mForce->UpdateFloater(*this, theFrac);
//	//}
//>>>>>>> .r156
//}