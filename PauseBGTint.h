#pragma once

#include "../SexyAppFramework/Graphics.h"

#include "floater.h"
#include "widget.h"

#include "globals.h"


namespace Sexy
{

class PauseBGTint :
	public Floater
{
public:
	PauseBGTint(void);
public:
	~PauseBGTint(void);

	virtual void	Draw(Graphics* g);
	void			Update(void);

};

} // end namespace Sexy