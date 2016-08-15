#include "PuzzlePakItem.h"

#include "globals.h"
#include "res.h"

using namespace Sexy;


PuzzlePakItem::PuzzlePakItem(void)
{
	mName = "";

	mImage = IMAGE_unknown;
	mLargeImage = IMAGE_unknown_large;

	mPartUncovered = 0.0f;
	mCurrentPartUncovered = 0.0f;

	Resize(
		gSexyAppBase->GetInteger("PUZZLE_PAK_IMAGE_X", 162) * gGraphicsScale,
		gSexyAppBase->GetInteger("PUZZLE_PAK_IMAGE_Y", 466) * gGraphicsScale,
		IMAGE_compass->GetWidth(),
		IMAGE_compass->GetHeight());

	mPosition.x = gSexyAppBase->GetInteger("PUZZLE_PAK_IMAGE_X", 162) * gGraphicsScale;
	mPosition.y = gSexyAppBase->GetInteger("PUZZLE_PAK_IMAGE_Y", 466) * gGraphicsScale;
	mPosition.z = 0.0f;
}

PuzzlePakItem::~PuzzlePakItem(void)
{
}


// sets mImage based on current mName
void PuzzlePakItem::SetImage(void)
{
	// TODO: make list?
	if( mName == "Compass" )
	{
		mImage = IMAGE_compass;
		mLargeImage = IMAGE_compass_large;
	}
	else if( mName == "Anchor" )
	{
		mImage = IMAGE_anchor;
		mLargeImage = IMAGE_anchor_large;

	}
	else if( mName == "Journal" )
	{
		mImage = IMAGE_journal;
		mLargeImage = IMAGE_journal_large;
	}
	else if( mName == "Jug" )
	{
		mImage = IMAGE_jug;
		mLargeImage = IMAGE_jug_large;
	}
	else if( mName == "Key" )
	{
		mImage = IMAGE_key;
		mLargeImage = IMAGE_key_large;
	}
	else if( mName == "Lantern" )
	{
		mImage = IMAGE_lantern;
		mLargeImage = IMAGE_lantern_large;
	}
	else if( mName == "Map" )
	{
		mImage = IMAGE_map;
		mLargeImage = IMAGE_map_large;
	}
	else if( mName == "Nav Compass" )
	{
		mImage = IMAGE_navcompass;
		mLargeImage = IMAGE_navcompass_large;
	}
	else if( mName == "Pipe" )
	{
		mImage = IMAGE_pipe;
		mLargeImage = IMAGE_pipe_large;
	}
	else if( mName == "Spyglass" )
	{
		mImage = IMAGE_spyglass;
		mLargeImage = IMAGE_spyglass_large;
	}
	else if( mName == "Watch" )
	{
		mImage = IMAGE_watch;
		mLargeImage = IMAGE_watch_large;
	}
	else if( mName == "Wheel" )
	{
		mImage = IMAGE_wheel;
		mLargeImage = IMAGE_wheel_large;
	}
	else
	{
		// default
		mImage = IMAGE_unknown;
		mLargeImage = IMAGE_unknown_large;
	}

}


void PuzzlePakItem::Draw(Graphics* g)
{
	Rect srcRect, destRect;
	Rect srcRect2, destRect2;
	float srcTargetHeight, destTargetHeight;


	if( mIsActive && mImage != NULL )
	{
		srcTargetHeight = mImage->GetHeight()*mCurrentPartUncovered;
		destTargetHeight = mHeight*mCurrentPartUncovered;

		if( mPosition.z < 1 )
		{
			int alpha = 255;
			if( mPosition.z < 0 )
			{
				alpha = 0;
			}
			else
			{
				alpha = 255 * mPosition.z;
			}

			g->SetColorizeImages(true);
			g->SetColor(Color(255,255,255,alpha));

			srcRect = Rect(0,0,mImage->GetWidth(),srcTargetHeight);
			destRect = Rect(0,0,mWidth,destTargetHeight);

			g->DrawImage(mImage,destRect,srcRect);

			// draw lock
			srcRect = Rect(0,destTargetHeight,IMAGE_lock->GetWidth(),IMAGE_lock->GetHeight()-destTargetHeight);
			destRect = Rect(0,destTargetHeight,mWidth,mHeight-destTargetHeight);

			g->DrawImage(IMAGE_lock,destRect,srcRect);


			//g->DrawImage(mImage,0,0,mWidth,mHeight);

			// testing float coordinates
			//if( gSexyAppBase->Is3DAccelerated() )
			//{
			//	g->DrawImageF(mImage, mPosition.x, mPosition.y);
			//}
			//else
			//{
			//	g->DrawImage(mImage, mPosition.x, mPosition.y);
			//}

			g->SetColorizeImages(false);
		}
		else
		{
			srcRect = Rect(0,0,mImage->GetWidth(),srcTargetHeight);
			destRect = Rect(0,0,mWidth,destTargetHeight);

			g->DrawImage(mImage,destRect,srcRect);

			// draw lock
			srcRect = Rect(0,destTargetHeight,IMAGE_lock->GetWidth(),IMAGE_lock->GetHeight()-destTargetHeight);
			destRect = Rect(0,destTargetHeight,mWidth,mHeight-destTargetHeight);

			g->DrawImage(IMAGE_lock,destRect,srcRect);


			//g->DrawImage(mImage,0,0,mWidth,mHeight);

			// testing float coordinates
			//if( gSexyAppBase->Is3DAccelerated() )
			//{
			//	g->DrawImageF(mImage, mPosition.x, mPosition.y);
			//}
			//else
			//{
			//	g->DrawImage(mImage, mPosition.x, mPosition.y);
			//}
		}
	}
}

void PuzzlePakItem::Update(void)
{
	if( mCurrentPartUncovered < mPartUncovered )
	{
		mCurrentPartUncovered += 0.001f;
	}
	else if( mCurrentPartUncovered > mPartUncovered + 0.001f )
	{
		// hide whole thing if moving too far
		// handles changing level paks
		if( mCurrentPartUncovered > mPartUncovered + 0.5f )
		{
			mCurrentPartUncovered = 0.0f;
			mPartUncovered = 0.002f;
		}
		else
		{
			mCurrentPartUncovered -= 0.001f;
		}
	};

	Floater::Update();
}