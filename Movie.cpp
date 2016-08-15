#include "Movie.h"
#include "GameApp.h"
#include "GameBoard.h"
#include "GameScreen.h"


using namespace Sexy;

Movie::Movie(MovieHost* theHost)
{
//	((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);
//	((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);

	mLength = 0;
	mMovieTimer = 0.0f;

	mHasContinueButton = false;

	mHost = theHost;

	mType = MovieType::FLOATINGIMAGE;

	mText = "";

	mMainTextFloater = new Floater();
	mMainTextForce = new Force();

	mContinueButton = NULL;

	mMainTextForce->mMass = gSexyAppBase->GetDouble("MOVIE_MAIN_TEXT_FORCE_MASS",0.0001f);

	mImage = NULL;

	mContinueButtonX = gSexyAppBase->GetInteger("CONTINUE_BUTTON_X",396) * gGraphicsScale;
	mContinueButtonY = gSexyAppBase->GetInteger("CONTINUE_BUTTON_Y",686) * gGraphicsScale;
}

Movie::~Movie(void)
{
}

void Movie::Update(void)
{
	Widget::Update();

	// TODO: change type to optimize
	float theFrac = 1.25f;

	mMovieTimer += theFrac;

	if( !mHasContinueButton && mMovieTimer > mLength )
	{
		mHost->OnMovieEnd();
	}

	if( mHasContinueButton )
	{
		if( mMainTextFloater->mPosition.z > 1.0f )
		{
			mMainTextFloater->mPosition.z = 1.0f;
			mMainTextFloater->Stop();

		}
		else if( mMainTextFloater->mPosition.z < 0.0f )
		{
			mMainTextFloater->mPosition.z = 0.0f;
			mMainTextFloater->Stop();

			mHost->OnMovieEnd();			
		}
		else
		{
			mMainTextForce->UpdateFloater(*mMainTextFloater, theFrac);
		}

		mContinueButton->mAlpha = mMainTextFloater->mPosition.z;
		mContinueButton->SetColor(0, Color(
			mContinueButton->mColors[0].GetRed(),
			mContinueButton->mColors[0].GetGreen(),
			mContinueButton->mColors[0].GetBlue(),
			255.0f*mMainTextFloater->mPosition.z));
	}
	else
	{
		mMainTextForce->UpdateFloater(*mMainTextFloater, theFrac);
	}

//	mMainTextForce->mMass +=
//		gSexyAppBase->GetDouble("MOVIE_MAIN_TEXT_FORCE_MASS_DELTA",0.0001f) * theFrac;

//	MarkDirty();
}

void Movie::Draw(Graphics* g)
{
	std::string s;
	int x, y, z;

	float imagewidth, imageheight;


	switch( mType )
	{
	case MovieType::FLOATINGIMAGE:
		if( mImage != NULL )
		{
			x = mMainTextFloater->mPosition.x;
			y = mMainTextFloater->mPosition.y;

			imageheight = mImage->GetHeight();
			imagewidth = mImage->GetWidth();

			g->SetColorizeImages(true);

			z = mMainTextFloater->mPosition.z * 255;
			if( z > 255 ) z = 255;
			if( z < 0 ) z = 0;

			g->SetColor(Color(255,255,255,z));
			g->DrawImage(mImage, x - (imagewidth/2), y - (imageheight/2),
				imagewidth, imageheight );
			g->SetColorizeImages(false);

			//s = StrFormat("floater: (%d, %f, %d), force: (%d)",
			//	z,
			//	mMainTextFloater->mAcceleration.z,
			//	mMainTextFloater->mIsActive,
			//	mMainTextForce->mActive);

			//g->SetColor(Color::White);
			//g->SetFont(FONT_DEFAULT);
			//x = (mWidth/2) - (g->StringWidth(s)/2);
			//y = mHeight/2;
			//g->DrawString(s,x,y);
		}

		break;

	case MovieType::TEXT:
		g->SetFont(FONT_DEFAULT);
//		((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);

		s = mText;

		x = (mWidth/2) - (g->StringWidth(s)/2);
		y = mHeight/2;

		//g->SetColor(Color(0, 0, 0, 255));
		//g->DrawString(s,x+1,y);
		//g->DrawString(s,x-1,y);
		//g->DrawString(s,x,y+1);
		//g->DrawString(s,x,y-1);

		g->SetColor(Color(255, 255, 255, 255));
		g->DrawString(s,x,y);

//		((ImageFont*)(FONT_DEFAULT))->SetScale(1.0f);
		break;
	}


}


void Movie::AddedToManager(WidgetManager* theWidgetManager)
{
	mMainTextFloater->Move(mWidth/2,mHeight/2);

	mMainTextFloater->mPosition.x = mWidth/2;
	mMainTextFloater->mPosition.y = mHeight/2;
	mMainTextFloater->mPosition.z = 0.01f;
	mMainTextFloater->Stop();

	mMainTextForce->mSource.x = mWidth/2;
	mMainTextForce->mSource.y = mHeight/2;
	mMainTextForce->mSource.z = 1.0f;

	mContinueButton = new DialogButton(IMAGE_DIALOG_BUTTON, Widgets::CONTINUE_BUTTON, this);
	mContinueButton->SetFont(FONT_DEFAULT);
	mContinueButton->mLabel = gSexyAppBase->GetString("CONTINUE_BUTTON_TEXT", "Continue");
	mContinueButton->SetVisible(false);


		mContinueButton->SetColor(ButtonWidget::COLOR_LABEL, Color(
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_R",40),
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_G",40),
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_B",40),
			192));
		mContinueButton->SetColor(ButtonWidget::COLOR_LABEL_HILITE, Color(
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_R",255),
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_G",255),
			gSexyAppBase->GetInteger("INTERFACE_BUTTON_TEXT_HOVER_B",255),
			255));	
		mContinueButton->mFrameNoDraw = true;

	if( mHasContinueButton )
	{
		mContinueButton->Resize(
			mContinueButtonX,
			mContinueButtonY,
			gSexyAppBase->GetInteger("CONTINUE_BUTTON_WIDTH",224) * gGraphicsScale,
			40.0f * gGraphicsScale);

		mContinueButton->SetVisible(true);
		mContinueButton->SetDisabled(false);

		mContinueButton->mDoFinger = true;

	}


	theWidgetManager->AddWidget(mContinueButton);
	theWidgetManager->AddWidget(mMainTextFloater);

}


void Movie::RemovedFromManager(WidgetManager* theWidgetManager)
{
	theWidgetManager->RemoveWidget(mContinueButton);
	gSexyAppBase->SafeDeleteWidget(mContinueButton);
	mHasContinueButton = false;
	theWidgetManager->RemoveWidget(mMainTextFloater);
}



void Movie::ButtonDepress(int theId)
{
	switch( theId )
	{
	case Widgets::CONTINUE_BUTTON:
		mMainTextForce->mSource.z = 0.0f;
		mContinueButton->SetDisabled(true);
		break;
	}
}