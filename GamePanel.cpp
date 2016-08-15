#include "GamePanel.h"
#include "res.h"

#include <sstream>

#include "../SexyAppFramework/Font.h"

#include "globals.h"

using namespace Sexy;

GamePanel::GamePanel(void)
{
	Resize(768,0,256,768);

	mMouseVisible = false;
}

GamePanel::~GamePanel(void)
{
}


void GamePanel::Draw(Graphics* g)
{
	int xmid, xright, xleft, y;

	g->DrawImage(IMAGE_gamepanel, 0, 0);

	xleft = 12;
	xmid = Width()/2;
	xright = Width()-12;
	y = 32;

	g->SetColor(Color(255,255,128,255));
	g->SetFont(FONT_DEFAULT);

	WriteString(g,gGameProperties.mDayAsText,xmid,y,-1,TEXT_JUSTIFY_CENTER,true);

	y += 32;
	WriteString(g,gGameProperties.mTimeAsText,xmid,y,-1,TEXT_JUSTIFY_CENTER,true);

	g->SetColor(Color(255,255,128,255));
	g->SetFont(FONT_DEFAULT);

	y += 48;

	WriteString(g,gActorManager.mActors[0].mName,xleft,y,-1,TEXT_JUSTIFY_LEFT,true);

	// log
	g->SetColor(Color(65,54,29,255));
	g->SetFont(FONT_BERLINSANS14);

	if( gEventManager.mEventLog.size() > 0 )
	{
		std::list<Event>::iterator anEvent = gEventManager.mEventLog.begin();

		std::stringstream ss;
		int eventCount = 0;

		int maxLines = 24;
		int currentLine = 23;
		int currentDrawY = 660;
		int fontAlpha = 255;

		while (anEvent != gEventManager.mEventLog.end())
		{
			eventCount++;

			SexyString text = anEvent->mDescription;

			int lineheight = FONT_BERLINSANS14->GetHeight();

			//int linesneeded = (g->GetWordWrappedHeight(Width() - 24, text, 20) / lineheight) + 1;
			int heightneeded = g->GetWordWrappedHeight(Width() - 24, text, -1) + FONT_BERLINSANS14->GetHeight();

			currentDrawY -= heightneeded;

			g->SetColor(Color(65, 54, 29, fontAlpha));

			WriteWordWrapped(g, Rect(12, currentDrawY, Width() - 24, heightneeded), text, -1, TEXT_JUSTIFY_LEFT);

			//ss << anEvent->mDescription << '\n';

			if (eventCount >= 100) break;

			if (currentDrawY <= 180) break;

			fontAlpha -= heightneeded / 3;

			anEvent++;
		}

		//while( anEvent != gEventManager.mEventLog.end() )
		//{
		//	eventCount++;
		//	ss << anEvent->mDescription << '\n';

		//	if( eventCount >= 10 ) break;

		//	anEvent++;
		//}
		//WriteWordWrapped(g, Rect(12, 460, Width()-24, 200), ss.str(), 20, TEXT_JUSTIFY_LEFT);
	}

}
