#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"

#include "floater.h"
#include "Digit.h"

#include "res.h"


namespace Sexy
{

#define MAX_FLOATING_NUMBER_DIGITS			8

class FloatingNumber :
	public Floater
{
public:
	FloatingNumber(void);
	FloatingNumber(int theMaxNumberOfDigits);
public:
	~FloatingNumber(void);

	int			amount;
	Digit*		mDigits[MAX_FLOATING_NUMBER_DIGITS];
	int			mMaxNumberOfDigits;


	virtual void			AddedToManager(WidgetManager* theWidgetManager);
	virtual void			RemovedFromManager(WidgetManager* theWidgetManager);

	virtual void			Update(void);

	void					CreateDigits(WidgetManager *theWidgetManager);
	void					RemoveDigits(WidgetManager *theWidgetManager);

	void					SetAcceleration(float x, float y, float z);

};

}