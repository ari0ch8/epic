#pragma once

#include "..\sexyappframework\image.h"

#include "Floater.h"

namespace Sexy
{

class Digit :
	public Floater
{
public:
	Digit(void);
public:
	~Digit(void);


public:
	short 	mDigit;
	Color	mBlendColor;

	virtual void	Draw(Graphics* g);
	void			Scale(float theFrac);

};

}