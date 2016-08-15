//#include "FancyTextWidget.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/SoundInstance.h"
#include "../SexyAppFramework/SoundManager.h"

#include "GameBoard.h"
#include "Res.h"
#include "SliderButton.h"
#include "GameScreen.h"

#include "Globals.h"

#include <sstream>


using namespace Sexy;


FancyTextWidget::FancyTextWidget(void)
{
	this->mMouseVisible = false;

	for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	{
		this->mTextLines[i].mText = "";
		this->mTextLines[i].mTextX = 0;
		this->mTextLines[i].mTextY = 0;
	}

	mIsDrawingTalkBubble = false;
}

FancyTextWidget::~FancyTextWidget(void)
{
}


void FancyTextWidget::Draw(Graphics* g)
{
	int		alpha = 255;
	int		longestlinewidth = 0;


	mFillWidth = 0;

	for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	{
		if( mFillWidth < FONT_DEFAULT->StringWidth(mTextLines[i].mText) )
			mFillWidth = FONT_DEFAULT->StringWidth(mTextLines[i].mText);
	}

	// width padding
	mFillWidth += TEXT_LINE_HEIGHT*gGraphicsScale;

	//for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	//{
	//	if( mTextLines[i].mText != "" )
	//	{
	//		alpha = 255;

	//		g->SetColor(Color(255,255,255,alpha));
	//		g->FillRect( mTextLines[i].mTextX - 10,
	//			mTextLines[i].mTextY - FONT_DEFAULT->GetAscent() - 10,
	//			longestlinewidth + 20,
	//			(FONT_DEFAULT->GetHeight()*1.1f) + 20);
	//	}
	//}



	if( mIsDrawingTalkBubble )
	{
		mFillHeight = 0;
		for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
		{
			if( mTextLines[i].mText != "" )
			{
				mFillHeight += (TEXT_LINE_HEIGHT*gGraphicsScale);
			}
		}

		// add height padding
		mFillHeight += TEXT_LINE_HEIGHT * gGraphicsScale;

		g->SetColor(Color(255,255,255,240));
		g->FillRect(Rect(
			mTextLines[0].mTextX - (TEXT_LINE_HEIGHT*gGraphicsScale*0.4f),
			mTextLines[0].mTextY - FONT_DEFAULT->GetAscent() - (TEXT_LINE_HEIGHT*gGraphicsScale*0.3f),
			mFillWidth,
			mFillHeight));

		g->SetColor(Color(64,64,64,192));
		g->DrawRect(Rect(
			mTextLines[0].mTextX - (TEXT_LINE_HEIGHT*gGraphicsScale*0.4f),
			mTextLines[0].mTextY - FONT_DEFAULT->GetAscent() - (TEXT_LINE_HEIGHT*gGraphicsScale*0.3f),
			mFillWidth,
			mFillHeight));

	}


	for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	{
		if( mTextLines[i].mText != "" )
		{
			g->SetFont(FONT_DEFAULT);
			g->SetColor(Color(255,255,0,alpha));
			g->DrawString( mTextLines[i].mText, mTextLines[i].mTextX, mTextLines[i].mTextY );
		}
	}
}

void FancyTextWidget::SetPosition(int x, int y)
{
	mFillX = mTextLines[0].mTextX;
	mFillY = mTextLines[0].mTextY - FONT_DEFAULT->GetAscent();

	mFillHeight = (TEXT_LINE_HEIGHT*gGraphicsScale) * (NUM_OF_TEXT_LINES - 1);

	for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	{
		this->mTextLines[i].mTextX = x;
		this->mTextLines[i].mTextY = y + ((TEXT_LINE_HEIGHT*gGraphicsScale) * i);//(FONT_DEFAULT->GetLineSpacing() * i);
	}
}


void FancyTextWidget::SetText(std::string text, int maxlength)
{

	for( int i=0;i<NUM_OF_TEXT_LINES;i++ )
	{
		this->mTextLines[i].mText = "";
		this->mTextLines[i].mTextX = 0;
		this->mTextLines[i].mTextY = 0;
	}

	if( text == "" )
	{
		SetVisible(false);
	}
	else
	{
		SetVisible(true);
		
		// see if split is needed
		if( text.length() > maxlength*3 )
		{
			// find first space
			int splitspace = 0;

			while( splitspace < text.length() )
			{
				if( splitspace >= maxlength && text.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == text.length() )
			{
				mTextLines[0].mText = text;
			}
			else
			{
				mTextLines[0].mText = text.substr(0, splitspace);
				mTextLines[1].mText = text.substr(splitspace+1, text.length()-(splitspace+1));
			}

			splitspace = 0;

			while( splitspace < mTextLines[1].mText.length() )
			{
				if( splitspace >= maxlength && mTextLines[1].mText.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == mTextLines[1].mText.length() )
			{
				mTextLines[1].mText = mTextLines[1].mText;
			}
			else
			{
				mTextLines[2].mText = mTextLines[1].mText.substr(splitspace+1, mTextLines[1].mText.length()-(splitspace+1));
				mTextLines[1].mText = mTextLines[1].mText.substr(0, splitspace);
			}

			splitspace = 0;

			while( splitspace < mTextLines[2].mText.length() )
			{
				if( splitspace >= maxlength && mTextLines[2].mText.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == mTextLines[2].mText.length() )
			{
				mTextLines[2].mText = mTextLines[2].mText;
			}
			else
			{
				mTextLines[3].mText = mTextLines[2].mText.substr(splitspace+1, mTextLines[2].mText.length()-(splitspace+1));
				mTextLines[2].mText = mTextLines[2].mText.substr(0, splitspace);
			}

		}
		else if( text.length() > maxlength*2 )
		{
			// find first space
			int splitspace = 0;

			while( splitspace < text.length() )
			{
				if( splitspace >= maxlength && text.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == text.length() )
			{
				mTextLines[0].mText = text;
			}
			else
			{
				mTextLines[0].mText = text.substr(0, splitspace);
				mTextLines[1].mText = text.substr(splitspace+1, text.length()-(splitspace+1));
			}

			splitspace = 0;

			while( splitspace < mTextLines[1].mText.length() )
			{
				if( splitspace >= maxlength && mTextLines[1].mText.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == mTextLines[1].mText.length() )
			{
				mTextLines[1].mText = mTextLines[1].mText;
			}
			else
			{
				mTextLines[2].mText = mTextLines[1].mText.substr(splitspace+1, mTextLines[1].mText.length()-(splitspace+1));
				mTextLines[1].mText = mTextLines[1].mText.substr(0, splitspace);
			}

		}
		else if( text.length() > maxlength )
		{
			// find first space
			int splitspace = 0;

			while( splitspace < text.length() )
			{
				if( splitspace >= maxlength && text.substr(splitspace,1) == " " )
				{
					break;
				}

				splitspace++;
			}

			if( splitspace == text.length() )
			{
				mTextLines[0].mText = text;
			}
			else
			{
				mTextLines[0].mText = text.substr(0, splitspace);
				mTextLines[1].mText = text.substr(splitspace+1, text.length()-(splitspace+1));
			}
		}
		else
		{
			mTextLines[0].mText = text;
		}
	}
}