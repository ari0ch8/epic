#pragma once

#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonListener.h"
#include "../SexyAppFramework/EditWidget.h"
#include "../SexyAppFramework/EditListener.h"
#include "../SexyAppFramework/SexyMatrix.h"


namespace Sexy
{

class GamePanel :
	public Sexy::Widget
{
public:
	GamePanel(void);
public:
	~GamePanel(void);

	void Draw(Graphics* g);

};

}
