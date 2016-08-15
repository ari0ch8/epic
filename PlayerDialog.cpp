#include "PlayerDialog.h"
#include "Res.h"
#include "Board.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/Font.h"
#include "../SexyAppFramework/DialogButton.h"
#include "../SexyAppFramework/Checkbox.h"
#include "../SexyAppFramework/Slider.h"

#include <iostream>
#include <sstream>

#include <ios>


#include "gameapp.h"
#include "gameboard.h"

using namespace Sexy;


PlayerDialog::PlayerDialog(PlayerDialogHost* theHost) :
Dialog(IMAGE_DIALOG_LARGE, IMAGE_DIALOG_BUTTON, 1.0f, PlayerDialog::DIALOG_ID, true, "Choose Player", "", "Return to Menu", Dialog::BUTTONS_FOOTER)
{
//	((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);
//	((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);

	mScale = gGraphicsScale;


	mContentInsets = Insets(48*gGraphicsScale, 20*gGraphicsScale, 48*gGraphicsScale, 32*gGraphicsScale);

	mSpaceAfterHeader = 24*gGraphicsScale;

	mHost = theHost;

	mListWidget = new ListWidget(LISTWIDGET_ID, FONT_DEFAULT, this);
	mListWidget->SetColor(ListWidget::COLOR_BKG, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_BACKGROUND_R", 255),
		gSexyAppBase->GetInteger("DIALOG_LIST_BACKGROUND_G", 255),
		gSexyAppBase->GetInteger("DIALOG_LIST_BACKGROUND_B", 255),
		gSexyAppBase->GetInteger("DIALOG_LIST_BACKGROUND_A", 255)));

	mListWidget->SetColor(ListWidget::COLOR_TEXT, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_TEXT_R", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_TEXT_G", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_TEXT_B", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_TEXT_A", 255)));

	mListWidget->SetColor(ListWidget::COLOR_HILITE, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_HILITE_R", 128),
		gSexyAppBase->GetInteger("DIALOG_LIST_HILITE_G", 128),
		gSexyAppBase->GetInteger("DIALOG_LIST_HILITE_B", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_HILITE_A", 255)));

	mListWidget->SetColor(ListWidget::COLOR_OUTLINE, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_OUTLINE_R", 0),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_OUTLINE_G", 0),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_OUTLINE_B", 0),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_OUTLINE_A", 255)));

	mListWidget->SetColor(ListWidget::COLOR_SELECT, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_R", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_G", 64),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_B", 255),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_A", 255)));

	mListWidget->SetColor(ListWidget::COLOR_SELECT_TEXT, Color(
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_TEXT_R", 128),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_TEXT_G", 128),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_TEXT_B", 255),
		gSexyAppBase->GetInteger("DIALOG_LIST_COLOR_SELECT_TEXT_A", 255)));

	mListWidget->mJustify = ListWidget::JUSTIFY_CENTER;


	std::stringstream ss;
	std::string s;
	std::string regKey;
	std::string currentLevelPak;
	int currentLevel;
	std::string line;
	int i;


	for( i = 1; i < 16; i++ )
	{
		ss.str("");
		ss << "Player ";
		ss << i;

		s = ss.str();
		currentLevelPak = "";
		currentLevel = 1;
		regKey = s;
		regKey += ":CurrentLevelPak";
		gSexyAppBase->RegistryReadString(regKey, &currentLevelPak);

		line = s;

		if( currentLevelPak != "" )
		{
			line += " - ";
			line += currentLevelPak;
			
			regKey = s;
			regKey += ":CurrentLevel";

			gSexyAppBase->RegistryReadInteger(regKey, &currentLevel);

			line += ", Level ";
			ss.str("");
			ss << currentLevel;
			line += ss.str();
		}

		mListWidget->AddLine(line, false);
	}




	// Let's create a scrollbar for the list widget.
	// The list widget is already a scrollbar listener, so we will
	// specify it as the widget responsible for dealing with the
	// scrollbar's messages. 
	mScrollbarWidget = new ScrollbarWidget(SCROLLBAR_ID, mListWidget);

	int scrollBarWidgetColors[][3] = 
	{
		// Background Color
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_BG_R",255),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BG_G",255),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BG_B",255) },
		//Outline color??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR1_R",224),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR1_G",224),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR1_B",224) },
		//Plain text not hilited??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR2_R",192),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR2_G",192),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR2_B",192) },
		//Text color when mouse is over text??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR3_R",160),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR3_G",160),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR3_B",160) },
		//Color of the bar displayed when an item is selected??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR4_R",128),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR4_G",128),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR4_B",128) },
		//Color of the text if it has the bar behind it and mouse is not over it??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR5_R",64),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR5_G",64),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR5_B",64) },
		//Color of the bar displayed when an item is selected??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR6_R",0),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR6_G",0),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_COLOR6_B",0) }
	};

	// What are scrollbar widget color indexes?
	// TODO: override scrollbar widget with custom colored scrollbar
	mScrollbarWidget->SetColors(scrollBarWidgetColors, 7);

	int scrollBarButtonWidgetColors[][3] = 
	{
		// Background Color
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_BG_R",255),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_BG_G",255),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_BG_B",255) },
		//Outline color??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR1_R",224),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR1_G",224),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR1_B",224) },
		//Plain text not hilited??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR2_R",192),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR2_G",192),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR2_B",192) },
		//Color of the bar displayed when an item is selected??
		{ gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR3_R",0),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR3_G",0),
		gSexyAppBase->GetInteger("DIALOG_SCROLL_BUTTON_COLOR3_B",0) }
	};

	mScrollbarWidget->mUpButton->SetColors(scrollBarButtonWidgetColors, 4);
	mScrollbarWidget->mDownButton->SetColors(scrollBarButtonWidgetColors, 4);

	// We also need to let the list widget know that it actually has a scrollbar.
	// That way, it will automatically resize it as items are added/removed.
	// IMPORTANT: You should do this before resizing the widgets.
	mListWidget->mScrollbar = mScrollbarWidget;


	//mScrollbarWidget->SetInvisIfNoScroll(true);

	//FillLevelPakNames();


	// set fonts
	SetHeaderFont(FONT_DEFAULT);
	SetLinesFont(FONT_DEFAULT);
	SetButtonFont(FONT_DEFAULT);

	SetColor(COLOR_HEADER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_A",255)
		));

	SetColor(COLOR_LINES, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
		));

	SetColor(COLOR_FOOTER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_A",255)
		));

	SetColor(COLOR_BUTTON_TEXT, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_A",255)
		));

	SetColor(COLOR_BUTTON_TEXT_HILITE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_A",255)
		));
	
	SetColor(COLOR_BKG, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_A",255)
		));

	SetColor(COLOR_OUTLINE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_A",255)
		));

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
PlayerDialog::~PlayerDialog()
{
	if( mListWidget ) delete mListWidget;
	if( mScrollbarWidget ) delete mScrollbarWidget;

	//delete mMusicVolumeSlider;
	//delete mSfxVolumeSlider;
	//delete mScreenDimensionsSlider;
	//delete mQuitBtn;
	//delete mCustomCursorsCheckbox;
	//delete m3DCheckbox;
	//delete mFSCheckbox;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PlayerDialog::Draw(Graphics* g)
{
	//// draw custom background
	//g->DrawImage(IMAGE_DIALOG_LARGE, 0, 0, Width(), Height() );

	Dialog::Draw(g);

	//// show puzzle pak header
	//g->SetColor( Color(
	//	gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
	//	gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
	//	gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
	//	224 // gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
	//	));

	//g->DrawString("Choose a Puzzle Pak",
	//	(Width()/2) - (mLinesFont->StringWidth("Choose a Puzzle Pak") / 2),
	//	(110*gGraphicsScale));



	//// TESTES
	//SexyString s;
	//s = StrFormat("p: %d - boardstate: %d",gPauseLevel,((GameApp*)gSexyAppBase)->mGameBoard->mState);
	//g->DrawString(s,20,20);

	//// Let's draw some labels for the sliders
	//g->SetFont(FONT_DEFAULT);
	//g->SetColor(Color::Black);

	//// We draw the X/Y at the slider's X/Y but we subtract off the dialog box's
	//// X/Y's from that. Why? The slider's XY's are in absolute screen coordinates,
	//// as are the dialog boxes. But as you may recall from Board::Draw(), where
	//// we explain graphics translation, once we get inside the draw loop of a
	//// widget, the graphics object is already moved (translated) to the widget's
	//// coordinates. Thus, in using the Graphics object, g, (0, 0) is located
	//// at the top left corner of the widget (our dialog box). So we
	//// just take the difference between the widget locations to produce the 
	//// actual location we want to drawour text at:
	//g->DrawString("Music volume:", mMusicVolumeSlider->mX - mX, 
	//	mMusicVolumeSlider->mY - mY);

	//g->DrawString("Sound volume:", mSfxVolumeSlider->mX - mX, 
	//	mSfxVolumeSlider->mY - mY);

	//g->DrawString("Screen size:", mScreenDimensionsSlider->mX - mX, 
	//	mScreenDimensionsSlider->mY - mY);

	//// draw the value of screen size
	////std::string s = StrFormat("%d", mScreenDimensionsSlider->mVal * 1024);

	//// Do the same for the checkboxes:
	//g->DrawString("3D Mode:", m3DCheckbox->mX - mX - 25, m3DCheckbox->mY - mY - m3DCheckbox->mHeight + 20);
	//g->DrawString("Full Screen:", mFSCheckbox->mX - mX - 25, mFSCheckbox->mY - mY - mFSCheckbox->mHeight + 20);
	//g->DrawString("Custom Cursors:", mCustomCursorsCheckbox->mX - mX - 25, 
	//				mCustomCursorsCheckbox->mY - mY - mCustomCursorsCheckbox->mHeight + 20);

}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PlayerDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);
	//theWidgetManager->AddWidget(mMusicVolumeSlider);
	//theWidgetManager->AddWidget(mSfxVolumeSlider);
	//theWidgetManager->AddWidget(mScreenDimensionsSlider);
	//theWidgetManager->AddWidget(mQuitBtn);

	theWidgetManager->AddWidget(mListWidget);
	theWidgetManager->AddWidget(mScrollbarWidget);


	//int checkWidth = IMAGE_CHECKBOX->GetWidth() / 2;
	//m3DCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	//m3DCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	//mFSCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	//mFSCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	//mCustomCursorsCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	//mCustomCursorsCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	//m3DCheckbox->mChecked = gSexyAppBase->Is3DAccelerated();
	//mFSCheckbox->mChecked = !gSexyAppBase->mIsWindowed;
	//mCustomCursorsCheckbox->mChecked = gSexyAppBase->mCustomCursorsEnabled;

	//theWidgetManager->AddWidget(m3DCheckbox);
	//theWidgetManager->AddWidget(mFSCheckbox);
	//theWidgetManager->AddWidget(mCustomCursorsCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PlayerDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);

	theWidgetManager->RemoveWidget(mListWidget);
	theWidgetManager->RemoveWidget(mScrollbarWidget);


	//theWidgetManager->RemoveWidget(mMusicVolumeSlider);
	//theWidgetManager->RemoveWidget(mSfxVolumeSlider);
	//theWidgetManager->RemoveWidget(mScreenDimensionsSlider);
	//theWidgetManager->RemoveWidget(mQuitBtn);
	//theWidgetManager->RemoveWidget(mCustomCursorsCheckbox);
	//theWidgetManager->RemoveWidget(m3DCheckbox);
	//theWidgetManager->RemoveWidget(mFSCheckbox);

	//if( mScreenSizeChanged )
	//	MessageBox( gSexyAppBase->mHWnd,
	//	"The screen size change will take effect the next time the game is started.",
	//	"Screen Size Change Notification",
	//	MB_OK );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PlayerDialog::MouseWheel(int theDelta)
{
	if( theDelta < 0 )
	{
		mListWidget->mScrollbar->SetValue(
			mListWidget->mScrollbar->mValue + 2 );
	}
	else
	{
		mListWidget->mScrollbar->SetValue(
			mListWidget->mScrollbar->mValue - 2 );
	}
}



void PlayerDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Dialog::Resize(theX, theY, theWidth, theHeight);



	// Let's make the scrollbar invisible until there are enough items in the list box
	// to make them all not fit
	mScrollbarWidget->SetInvisIfNoScroll(true);

	// load local level pak names
//	FillLevelPakNames();



	mListWidget->Resize(theX + mContentInsets.mLeft + (40*gGraphicsScale),
		theY + mContentInsets.mTop + (60 * gGraphicsScale),
		theWidth - mContentInsets.mLeft - mContentInsets.mRight - (80*gGraphicsScale),
		theHeight - mContentInsets.mTop - mContentInsets.mBottom - (120 * gGraphicsScale));

	// Let's resize the scrollbar to the right of the list box. Instead of using
	// Resize(...), we use ResizeScrollbar to indicate that the entire scrollbar,
	// arrows and thumb included, are to be evenly resized. This will appropriately
	// scale all the items in the scrollbar.
	mScrollbarWidget->ResizeScrollbar(mListWidget->mX + mListWidget->mWidth - (20 * gGraphicsScale), 
										mListWidget->mY,
										(20 * gGraphicsScale),						// an arbitrary width for the bar itself
										mListWidget->mHeight);






	//mMusicVolumeSlider->Resize(theX + mContentInsets.mLeft + 50, 
	//	theY + 100, 245, IMAGE_SLIDER_THUMB->GetHeight());

	//mSfxVolumeSlider->Layout(LAY_SameLeft | LAY_Below | LAY_SameWidth | LAY_SameHeight, 
	//	mMusicVolumeSlider, 0, 20, 0, 0);

	//mScreenDimensionsSlider->Layout(LAY_SameLeft | LAY_Below | LAY_SameWidth | LAY_SameHeight,
	//	mSfxVolumeSlider, 0, 20, 0, 0);

	//mQuitBtn->Layout(LAY_SameLeft | LAY_SameSize | LAY_Above, mYesButton);

	//m3DCheckbox->Layout(LAY_SameLeft | LAY_Below, mScreenDimensionsSlider, 0, 40, 0, 0);
	//m3DCheckbox->Resize(m3DCheckbox->mX, m3DCheckbox->mY,
	//	IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

	//mFSCheckbox->Layout(LAY_SameTop | LAY_SameWidth | LAY_SameHeight | LAY_Right, m3DCheckbox, 80);

	//mCustomCursorsCheckbox->Layout(LAY_SameTop | LAY_SameWidth | LAY_SameHeight | LAY_Right, mFSCheckbox, 80);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void PlayerDialog::ButtonDepress(int theId)
{
	Dialog::ButtonDepress(theId);

	//// Previously, before framework 1.2, the line below looked like:
	//// else if (theId == 2000 + mId)
	//// With the 1.2 changes, there's just 1 slight incompatibility.
	//// If the DialogListener for a dialog your app (by default), and
	//// you don't implement your own DialogButton(De)press method (see V12Demo for
	//// more info on this), and instead are trapping button presses within ButtonDepress
	//// inside the dialog itself, then instead of the button ID being 2000 or 3000 + mId,
	//// it's just 0 for no/cancel/1-button dialog, and 1 for yes/ok. Hence we changed our
	//// check to look for 0, which is our default single close button at the bottom of this
	//// dialog.

	switch( theId )
	{
	case ID_YES:
	//	Tell the app to remove ourselves. This
	//	will automatically safe delete the dialog at a later time.
		mHost->OnPlayerDialogClosed("");
		gSexyAppBase->KillDialog(this);
		break;

	}

}


void PlayerDialog::ListClicked(int theId, int theIdx, int theClickCount)
{
	switch( theId )
	{
	case LISTWIDGET_ID:
		// string level and number off name and return player name
		std::string s;
		s = mListWidget->GetStringAt(theIdx);
		std::string::size_type loc;
		

		loc = s.find(" - ");

		if( loc != std::string::npos )
		{
			s = s.substr(0, loc);
		}

		mHost->OnPlayerDialogClosed(s);
		gSexyAppBase->KillDialog(this);
		break;
	}
}


