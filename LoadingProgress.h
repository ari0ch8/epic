#pragma once
#include "floater.h"
#include "graphics.h"
#include "res.h"



namespace Sexy
{

class LoadingProgress :
	public Sexy::Floater
{
public:
	LoadingProgress(void);
public:
	~LoadingProgress(void);

	virtual void Draw(Graphics* g);
	void Update(void);
};

}