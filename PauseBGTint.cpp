#include "PauseBGTint.h"

using namespace Sexy;

PauseBGTint::PauseBGTint(void)
{
	mMouseVisible = false;
}

PauseBGTint::~PauseBGTint(void)
{
}


void PauseBGTint::Draw(Graphics* g)
{
	float alpha = 255.0f * mPosition.z;

	if( alpha < 0.0f ) alpha = 0.0f;
	if( alpha > 160.0f ) alpha = 160.0f;

	g->SetColor(Color(0,0,0,alpha));
	g->SetColorizeImages(true);

	g->FillRect(0,0,mWidth,mHeight);
	
	g->SetColorizeImages(false);
}

void PauseBGTint::Update(void)
{
	// always update this floater
	int oldpauselevel = gPauseLevel;
	
	gPauseLevel = 0;

	Floater::Update();

	gPauseLevel = oldpauselevel;
}
