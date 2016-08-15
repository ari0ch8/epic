#include "LoadingProgress.h"
#include "globals.h"


using namespace Sexy;


LoadingProgress::LoadingProgress(void)
{
}

LoadingProgress::~LoadingProgress(void)
{
}


void LoadingProgress::Draw(Graphics* g)
{
	float alpha;

	alpha = mPosition.z * 255.0f;
	if( alpha > 255.0f ) alpha = 255.0f;
	if( alpha < 0.0f ) alpha = 0.0f;

	g->SetColor(Color(255,255,255,(int)alpha)); //mLoadingAlpha));

	g->SetColorizeImages(true);

	// loading bar
	if( IMAGE_LOADER_LOADINGTXT )
	{
		g->DrawImage(IMAGE_LOADER_LOADINGTXT,
			(mWidth/2)-(IMAGE_LOADER_LOADINGTXT->GetWidth()/2),
			0,
			IMAGE_LOADER_LOADINGTXT->GetWidth(),
			IMAGE_LOADER_LOADINGTXT->GetHeight());	
	}

	if( IMAGE_LOADER_BAR )
	{
		// We want to draw our loader bar to indicate the progress made in
		// loading all our resources. As you recalll, GameApp::LoadingThreadProc is
		// the thread that is actually loading everything. The app has a function,
		// GetLoadingThreadProgress, that returns a value from 0.0 to 1.0 indicating
		// the % complete it is. We will multiply this % complete by the width
		// of our progress bar, so that we give the illusion of a growing bar.
		int loaderBarWidth = IMAGE_LOADER_BAR->GetWidth();
		int drawWidth = (int)(gSexyAppBase->GetLoadingThreadProgress() * loaderBarWidth);
		if (drawWidth > 0)
		{
			// As you may recall from Demo3 where we drew the frames of animation
			// for the lightning image, the DrawImage call can take a source rectangle
			// which indicates what chunk of the original image we want to draw.
			// In our case, we want to start from from the upper left corner of
			// the loader bar, but we only want to draw "drawWidth" wide. This will
			// give the illusion that the progress bar is expanding as the resources
			// are loaded in.
			g->DrawImage(IMAGE_LOADER_BAR,
						0, 
						IMAGE_LOADER_LOADINGTXT->GetHeight(), 
						Rect(0, 0, drawWidth,
						IMAGE_LOADER_BAR->GetHeight()));
		}
	}

	g->SetColorizeImages(false);

}


void LoadingProgress::Update(void)
{
	Floater::Update();

	if(mPosition.z < 0)
	{
		gSexyAppBase->mWidgetManager->RemoveWidget(this);
		mIsActive = false;
	}
}