#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"

#include "ParticleWidget.h"
#include "GameApp.h"

using namespace Sexy;



void ParticleWidget::Draw(Graphics* g)
{
	((GameApp *)gSexyAppBase)->mParticleManager->Render( g );
}