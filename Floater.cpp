#include "Floater.h"
#include "globals.h"

using namespace Sexy;

Floater::Floater(void)
{
	mIsActive = false;
	mScaleF = 1.0f;

	Stop();
	mPosition.z = 1.0f;

	mFalloff = 0.65f;
}

Floater::~Floater(void)
{
}

void Floater::Update(void)
{
	Widget::Update();

	// TODO: change type to optimize
	float theFrac = 1.25f;

	if( gPauseLevel == 0 )
	{
		float theFracSquared = theFrac * theFrac;

		// falloff
		mAcceleration.x = mAcceleration.x * mFalloff * theFrac;
		mAcceleration.y = mAcceleration.y * mFalloff * theFrac;
		mAcceleration.z = mAcceleration.z * mFalloff * theFrac;
		
		mVelocity.x += mAcceleration.x * theFrac;
		mVelocity.y += mAcceleration.y * theFrac;
		mVelocity.z += mAcceleration.z * theFrac;

		mPosition.x += (mVelocity.x * theFrac) +
			((mAcceleration.x / 2.0f) * theFracSquared);
		mPosition.y += (mVelocity.y * theFrac) +
			((mAcceleration.y / 2.0f) * theFracSquared);
		mPosition.z += (mVelocity.z * theFrac) +
			((mAcceleration.z / 2.0f) * theFracSquared);

		mX = mPosition.x;
		mY = mPosition.y;

		MarkDirty();
	}
}

void Floater::Draw(Graphics* g)
{
	if( mIsActive && mImage != NULL )
	{
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

			g->DrawImage(mImage, 0,0,mWidth,mHeight);
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
			g->DrawImage(mImage,0,0,mWidth,mHeight);
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


void Floater::Stop(void)
{
	mAcceleration.x = 0;
	mAcceleration.y = 0;
	mAcceleration.z = 0;

	mVelocity.x = 0;
	mVelocity.y = 0;
	mVelocity.z = 0;
}