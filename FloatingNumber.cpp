#include "FloatingNumber.h"

#include "globals.h"


using namespace Sexy;

FloatingNumber::FloatingNumber(int theMaxNumberOfDigits)
{
	mMaxNumberOfDigits = theMaxNumberOfDigits;
	if( mMaxNumberOfDigits > MAX_FLOATING_NUMBER_DIGITS )
		mMaxNumberOfDigits = MAX_FLOATING_NUMBER_DIGITS;

	mIsActive = false;
	Stop();
	amount = 0;

	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		mDigits[i] = NULL;
	}
}

FloatingNumber::FloatingNumber(void)
{
	mMaxNumberOfDigits = 4;
	if( mMaxNumberOfDigits > MAX_FLOATING_NUMBER_DIGITS )
		mMaxNumberOfDigits = MAX_FLOATING_NUMBER_DIGITS;

	mIsActive = false;
	Stop();
	amount = 0;

	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		mDigits[i] = NULL;
	}
}

FloatingNumber::~FloatingNumber(void)
{
}

//void FloatingNumber::Draw(Graphics* g)
//{
//}

void FloatingNumber::Update(void)
{
	bool hasAnyDigitsLeft = false;

	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		if( mDigits[i]->mWidgetManager != NULL )
		{
			if(mDigits[i]->mPosition.z < 0)
			{
				gSexyAppBase->mWidgetManager->RemoveWidget(mDigits[i]);
			}
			else
			{
				hasAnyDigitsLeft = true;
			}
		}
	}

	if(!hasAnyDigitsLeft)
	{
		gSexyAppBase->mWidgetManager->RemoveWidget(this);
	}
}


void FloatingNumber::AddedToManager(WidgetManager *theWidgetManager)
{
	Widget::AddedToManager(theWidgetManager);

	CreateDigits(theWidgetManager);

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void FloatingNumber::RemovedFromManager(WidgetManager *theWidgetManager)
{
	Widget::RemovedFromManager(theWidgetManager);

	RemoveDigits(theWidgetManager);
}


void FloatingNumber::CreateDigits(WidgetManager *theWidgetManager)
{
	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		mDigits[i] = new Digit();
	}

	bool isAtEnd = false;
	short int scoreLength = 0;
	char szScore[MAX_FLOATING_NUMBER_DIGITS+1];
	
	// TODO: fix max amount
	if( amount > 1000000 ) amount = 1000000;

	itoa(amount,szScore,10);
	scoreLength = strlen(szScore);
	
	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		mScaleF = 1.0f;
		mDigits[i]->mDigit = szScore[i] - '0';

		if( szScore[i] == 0 )
		{
			isAtEnd = true;
		}

		if( isAtEnd )
		{
			mDigits[i]->mIsActive = false;
			mDigits[i]->mDigit = -1;
		}
		else
		{
			mDigits[i]->mImage = IMAGE_NUMBERS;
			theWidgetManager->AddWidget(mDigits[i]);
			mDigits[i]->mIsActive = true;
			mDigits[i]->mWidthF = mDigits[i]->mWidth;
			mDigits[i]->mHeightF = mDigits[i]->mHeight;
		}
	}

	float spacing = (float)(24*gGraphicsScale);
	if( scoreLength > 2 )
	{
		spacing *= 0.6f;

		for( int i=0; i<mMaxNumberOfDigits; i++ )
		{
			mDigits[i]->Scale(0.6f);
		}
	}

	float x = this->mPosition.x - spacing*(float)scoreLength/2.0f;
	float y = this->mPosition.y - (float)(mDigits[0]->Height())/2.0f;

	for( int i=0; i<scoreLength; i++ )
	{
		mDigits[i]->mPosition.x = x;
		mDigits[i]->mPosition.y = y;

		x += spacing;
	}
}

void FloatingNumber::RemoveDigits(WidgetManager *theWidgetManager)
{
	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		if( mDigits[i]->mWidgetManager != NULL )
			theWidgetManager->RemoveWidget(mDigits[i]);

		delete mDigits[i];
	}
}


void FloatingNumber::SetAcceleration(float x, float y, float z)
{
	for( int i=0; i<mMaxNumberOfDigits; i++ )
	{
		if( mDigits[i] != NULL )
		{
			mDigits[i]->mAcceleration.x = x;
			mDigits[i]->mAcceleration.y = y;
			mDigits[i]->mAcceleration.z = z;
		}
	}
}
