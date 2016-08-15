#include "GameProperties.h"

#include <sstream>
#include <string>



GameProperties::GameProperties(void)
{
	mTurn = 0;

	isLogShowingMovement = false;

	SetTimeFromTurn();
}

GameProperties::~GameProperties(void)
{
}

void GameProperties::SetTimeFromTurn(void)
{
	__int64 turnRemainder;
	int offsetturn = mTurn + ((31+28+31+30+31+30+31+31+3)*TURNSPERDAY) + (9*TURNSPERHOUR); // sept 4, 10am

	mYear = (int)(offsetturn / TURNSPERYEAR) + 312;
	turnRemainder = offsetturn % TURNSPERYEAR;

	mDayOfTheYear = (int)(turnRemainder / TURNSPERDAY) + 1;
	turnRemainder = turnRemainder % TURNSPERDAY;

	mHour = (int)(turnRemainder / TURNSPERHOUR) + 1;
	turnRemainder = turnRemainder % TURNSPERHOUR;

	mMinute = (int)(turnRemainder / TURNSPERMINUTE);

	// don't see a need to get turn within minute

	// funny code
	if( mDayOfTheYear <= 31 )
	{
		mMonth = 1;
		mDayOfTheMonth = mDayOfTheYear;
	}
	else if( mDayOfTheYear <= 31+28 )
	{
		mMonth = 2;
		mDayOfTheMonth = mDayOfTheYear - 31;
	}
	else if( mDayOfTheYear <= 31+28+31 )
	{
		mMonth = 3;
		mDayOfTheMonth = mDayOfTheYear - (31+28);
	}
	else if( mDayOfTheYear <= 31+28+31+30 )
	{
		mMonth = 4;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31 )
	{
		mMonth = 5;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30 )
	{
		mMonth = 6;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30+31 )
	{
		mMonth = 7;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30+31+31 )
	{
		mMonth = 8;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30+31);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30+31+31+30 )
	{
		mMonth = 9;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30+31+31);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30+31+31+30+31 )
	{
		mMonth = 10;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30+31+31+30);
	}
	else if( mDayOfTheYear <= 31+28+31+30+31+30+31+31+30+31+30 )
	{
		mMonth = 11;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30+31+31+30+31);
	}
	else
	{
		mMonth = 12;
		mDayOfTheMonth = mDayOfTheYear - (31+28+31+30+31+30+31+31+30+31+30);
	}

	std::stringstream textStream;
	textStream.str("");

	switch( mMonth )
	{
	case 1:
		textStream << "January";
		break;
	case 2:
		textStream << "February";
		break;
	case 3:
		textStream << "March";
		break;
	case 4:
		textStream << "April";
		break;
	case 5:
		textStream << "May";
		break;
	case 6:
		textStream << "June";
		break;
	case 7:
		textStream << "July";
		break;
	case 8:
		textStream << "August";
		break;
	case 9:
		textStream << "September";
		break;
	case 10:
		textStream << "October";
		break;
	case 11:
		textStream << "November";
		break;
	case 12:
		textStream << "December";
		break;

	}

	textStream << " " << mDayOfTheMonth << ", " << mYear;
	mDayAsText = textStream.str();

	textStream.str("");
	textStream << ((mHour-1)%12)+1 << ":" << (mMinute < 10 ? "0" : "") << mMinute;

	if( mHour <= 12 )
	{
		textStream << " am";
	}
	else
	{
		textStream << " pm";
	}

	mTimeAsText = textStream.str();
}


void GameProperties::Advance(int numberOfTurns)
{
	mTurn += numberOfTurns;

	SetTimeFromTurn();
}
