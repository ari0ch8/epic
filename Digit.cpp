#include "../SexyAppFramework/SexyAppBase.h"
#include "Digit.h"
#include "res.h"
#include "globals.h"

using namespace Sexy;

Digit::Digit(void)
{
	mDigit = -1;

	//mWidth = 36 * gGraphicsScale;
	//mHeight = 60 * gGraphicsScale;
	mWidth = 30 * gGraphicsScale;
	mHeight = 40 * gGraphicsScale;

	// start offscreen!
	mX = -200;
	mY = -200;

	// mBlendColor = Color(Color::White);

	mBlendColor.mRed = gSexyAppBase->GetInteger("DEFAULT_NUMBER_COLOR_R",255);
	mBlendColor.mGreen = gSexyAppBase->GetInteger("DEFAULT_NUMBER_COLOR_G",255);
	mBlendColor.mBlue = gSexyAppBase->GetInteger("DEFAULT_NUMBER_COLOR_B",128);

	Stop();
}


Digit::~Digit(void)
{
}


void Digit::Scale(float theFrac)
{
	mWidth *= theFrac;
	mHeight *= theFrac;
}


void Digit::Draw(Graphics* g)
{
	if( mIsActive && mImage != NULL && mDigit!=-1 )
	{
		Rect rect;
		rect.mX = 0;
		rect.mY = 0;
		rect.mWidth = mWidth;
		rect.mHeight = mHeight;

		

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

			mBlendColor.mAlpha = alpha;

			g->SetColor(mBlendColor);
			g->SetColorizeImages(true);

			g->DrawImageCel(mImage, rect, mDigit);

			g->SetColorizeImages(false);
		}
		else
		{
			mBlendColor.mAlpha = 255;

			g->SetColor(mBlendColor);
			g->SetColorizeImages(true);

			g->DrawImageCel(mImage, rect, mDigit);

			g->SetColorizeImages(false);
		}
	}

}
