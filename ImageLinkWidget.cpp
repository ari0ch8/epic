#include "ImageLinkWidget.h"
#include "..//sexyappframework//sexyappbase.h"
#include "..//sexyappframework//widgetmanager.h"
#include "res.h"

using namespace Sexy;

ImageLinkWidget::ImageLinkWidget(void)
{
	mImage = NULL;
	mHoverImage = NULL;

	mLinkTarget = "";


	mWidth = IMAGE_BRANDING->mWidth;
	mHeight =IMAGE_BRANDING->mHeight;

	mBlendColor = Color::White;
	mHoverBlendColor = Color::White;
	mHoverBlendColor.mAlpha = 0;

	mFadeState = 0;
	mFadeLevel = 0;

}

ImageLinkWidget::~ImageLinkWidget(void)
{
}


void ImageLinkWidget::Draw(Graphics* g)
{
	Rect destRect;

	if( mImage != NULL )
	{

		destRect.mX = 0;
		destRect.mY = 0;

		destRect.mWidth = this->Width();
		destRect.mHeight = this->Height();

		g->SetColorizeImages(true);

		// draw button
		mBlendColor.mAlpha = 255;
		if( mBlendColor.mAlpha > 255 )
			mBlendColor.mAlpha = 255;
		if( mBlendColor.mAlpha < 0 )
			mBlendColor.mAlpha = 0;

		g->SetColor(mBlendColor);

		if( mHoverBlendColor.mAlpha != 255 )
			g->DrawImage(mImage, 0, 0);
			
		g->SetColor(mHoverBlendColor);

		if( mHoverImage != NULL && mHoverBlendColor != Color::Black )
		{
			g->SetColor(mHoverBlendColor);
			g->SetDrawMode(Graphics::DRAWMODE_ADDITIVE);
			g->DrawImage(mHoverImage, 0, 0);
			g->SetDrawMode(Graphics::DRAWMODE_NORMAL);
		}

		g->SetColorizeImages(false);

	}

}

void ImageLinkWidget::Update(void)
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
		break;

	case 0:
		mBlendColor = Color(255,255,255,255);

		//mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);
		mHoverBlendColor = Color::White;
		mHoverBlendColor.mAlpha = 0;
		break;
	}

	//mBlendColor = Color(255,255,255,((GameBoard*)mParent)->mAlpha);
}

void ImageLinkWidget::MouseEnter()
{
	mFadeState = 1;
	mFadeLevel = 0;
}

void ImageLinkWidget::MouseLeave()
{
	mFadeState = -1;
}

void ImageLinkWidget::MouseDown(int x, int y, int theClickCount)
{
	ShellExecute(NULL,"open",mLinkTarget.c_str(), NULL, NULL, 0);
}