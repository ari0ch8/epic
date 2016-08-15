#pragma once

#include <string>




#define TURNSPERMINUTE (__int64)10
#define MINUTESPERHOUR (__int64)60
#define HOURSPERDAY (__int64)24
#define DAYSPERYEAR (__int64)365

#define TURNSPERYEAR (DAYSPERYEAR*HOURSPERDAY*MINUTESPERHOUR*TURNSPERMINUTE)
#define TURNSPERDAY (HOURSPERDAY*MINUTESPERHOUR*TURNSPERMINUTE)
#define TURNSPERHOUR (MINUTESPERHOUR*TURNSPERMINUTE)




class GameProperties
{
public:
	GameProperties(void);
	~GameProperties(void);

public:
	__int64 mTurn;

	int mYear;
	int mDayOfTheYear;
	int mMonth;
	int mDayOfTheMonth;
	int mHour;
	int mMinute;

	std::string mDayAsText;
	std::string mTimeAsText;

	bool	isLogShowingMovement;


	void SetTimeFromTurn(void);
	void Advance(int numberOfTurns);
};
