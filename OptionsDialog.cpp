#include "OptionsDialog.h"
#include "Res.h"
#include "Board.h"
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/WidgetManager.h"
#include "../SexyAppFramework/Font.h"
#include "../SexyAppFramework/ImageFont.h"
#include "../SexyAppFramework/DialogButton.h"
#include "../SexyAppFramework/Checkbox.h"
#include "../SexyAppFramework/Slider.h"

using namespace Sexy;


OptionsDialog::OptionsDialog(OptionsDialogHost* theHost) :
Dialog(IMAGE_DIALOG_LARGE, IMAGE_DIALOG_BUTTON, 1.0f, OptionsDialog::DIALOG_ID, true, "Options", "", "Return to Game", Dialog::BUTTONS_FOOTER)
{
	//((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);
	//((ImageFont*)(FONT_DEFAULT))->SetScale(gGraphicsScale);

	mScale = gGraphicsScale;

	// TODO: read from XML
//	mScreenSizeList[0][0] = 480;
//	mScreenSizeList[0][1] = 360;

	mScreenSizeList[0][0] = 640;
	mScreenSizeList[0][1] = 480;

	mScreenSizeList[1][0] = 800;
	mScreenSizeList[1][1] = 600;

	mScreenSizeList[2][0] = 1024;
	mScreenSizeList[2][1] = 768;


	mContentInsets = Insets(48*gGraphicsScale, 20*gGraphicsScale, 48*gGraphicsScale, 32*gGraphicsScale);
	mSpaceAfterHeader = 24*gGraphicsScale;

	mHost = theHost;

	// set fonts
	SetHeaderFont(FONT_DEFAULT);
	SetLinesFont(FONT_DEFAULT);
	SetButtonFont(FONT_DEFAULT);

	Dialog::SetColor(COLOR_HEADER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_A",255)
		));

	Dialog::SetColor(COLOR_LINES, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
		));

	Dialog::SetColor(COLOR_FOOTER, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_A",255)
		));

	Dialog::SetColor(COLOR_BUTTON_TEXT, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_A",255)
		));

	Dialog::SetColor(COLOR_BUTTON_TEXT_HILITE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_A",255)
		));
	
	Dialog::SetColor(COLOR_BKG, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_A",255)
		));

	Dialog::SetColor(COLOR_OUTLINE, 
		Color(
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
		gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_A",255)
		));

	mMusicVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, OptionsDialog::MUSIC_SLIDER_ID, this);
	mMusicVolumeSlider->SetValue(gSexyAppBase->GetMusicVolume());

	mSfxVolumeSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, OptionsDialog::SFX_SLIDER_ID, this);
	mSfxVolumeSlider->SetValue(gSexyAppBase->GetSfxVolume());

	//mScreenDimensionsSlider = new Slider(IMAGE_SLIDER_TRACK, IMAGE_SLIDER_THUMB, OptionsDialog::SCREENDIMENSIONS_SLIDER_ID, this);

	int aScreensize = 2;
	gSexyAppBase->RegistryReadInteger("ScreenSize", &aScreensize);
	if( aScreensize < 0 )
		aScreensize = 0;
	if( aScreensize > 2 )
		aScreensize = 2;

	//mScreenDimensionsSlider->SetValue( (double)aScreensize / 2 );

	mScreenSizeChanged = false;

	//mQuitBtn = new DialogButton(IMAGE_DIALOG_BUTTON, OptionsDialog::QUIT_BTN_ID, this);
	//mQuitBtn->mLabel = "QUIT GAME";
	//mQuitBtn->SetFont(FONT_DEFAULT);

	m3DCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::HARDWARE_CHECKBOX_ID, this);
	mFSCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::FS_CHECKBOX_ID, this);
	mCustomCursorsCheckbox = new Checkbox(IMAGE_CHECKBOX, IMAGE_CHECKBOX, OptionsDialog::FS_CHECKBOX_ID, this);

	mAlpha = 255.0f;

	Stop();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
OptionsDialog::~OptionsDialog()
{
	delete mMusicVolumeSlider;
	delete mSfxVolumeSlider;
	//delete mScreenDimensionsSlider;
//	delete mQuitBtn;
	delete mCustomCursorsCheckbox;
	delete m3DCheckbox;
	delete mFSCheckbox;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::Draw(Graphics* g)
{
	//// draw custom background
	//g->DrawImage(IMAGE_DIALOG_LARGE, 0, 0, Width(), Height() );

	mAlpha = this->mPosition.z * 255.0f;

	if( mAlpha < 0.0f ) mAlpha = 0.0f;
	if( mAlpha > 255.0f ) mAlpha = 255.0f;

	g->SetColorizeImages(true);
	g->SetColor(Color(255,255,255,mAlpha));

	Dialog::Draw(g);

	// Let's draw some labels for the sliders
	g->SetFont(FONT_DEFAULT);


//	float textAlpha = mAlpha;

//	((ImageFont*)FONT_DEFAULT)->SetScale(gGraphicsScale);

	// TODO: optimize!
	g->SetColor(Color(
		gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
		gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
		mAlpha // gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
		));

	//((ImageFont*)FONT_DEFAULT)->SetScale(gGraphicsScale);

	// We draw the X/Y at the slider's X/Y but we subtract off the dialog box's
	// X/Y's from that. Why? The slider's XY's are in absolute screen coordinates,
	// as are the dialog boxes. But as you may recall from Board::Draw(), where
	// we explain graphics translation, once we get inside the draw loop of a
	// widget, the graphics object is already moved (translated) to the widget's
	// coordinates. Thus, in using the Graphics object, g, (0, 0) is located
	// at the top left corner of the widget (our dialog box). So we
	// just take the difference between the widget locations to produce the 
	// actual location we want to drawour text at:
	std::string s;
	int x, y;

	s = "Music Volume";
	x = mMusicVolumeSlider->mX - Dialog::mX + (mMusicVolumeSlider->Width() / 2)
		- (FONT_DEFAULT->StringWidth(s) / 2);
	y = mMusicVolumeSlider->mY - Dialog::mY;
	g->DrawString(s,x,y);

	s = "Sound Volume";
	x = mSfxVolumeSlider->mX - Dialog::mX + (mSfxVolumeSlider->Width() / 2)
		- (FONT_DEFAULT->StringWidth(s) / 2);
	y = mSfxVolumeSlider->mY - Dialog::mY;
	g->DrawString(s,x,y);

	//if( mFSCheckbox->IsChecked() )
	//{
	//	s = "Graphic Detail";
	//}
	//else
	//{
	//	s = "Window Size";
	//}

	//x = mScreenDimensionsSlider->mX - Dialog::mX + (mScreenDimensionsSlider->Width() / 2)
	//	- (FONT_DEFAULT->StringWidth(s) / 2);
	//y = mScreenDimensionsSlider->mY - Dialog::mY;
	//g->DrawString(s,x,y);

	// draw the value of screen size
	//std::string s = StrFormat("%d", mScreenDimensionsSlider->mVal * 1024);

	// Do the same for the checkboxes:

	s = "Enhanced Special Effects (3D)";
	x = m3DCheckbox->mX - Dialog::mX + (IMAGE_CHECKBOX->mWidth>>2) + (IMAGE_CHECKBOX->mWidth>>1);
	y = m3DCheckbox->mY - Dialog::mY + (IMAGE_CHECKBOX->mHeight>>2) + (FONT_DEFAULT->GetHeight() >> 1);
	g->DrawString(s,x,y);

	s = "Full Screen";
	y = mFSCheckbox->mY - Dialog::mY + (IMAGE_CHECKBOX->mHeight>>2) + (FONT_DEFAULT->GetHeight() >> 1);
	g->DrawString(s,x,y);

	s = "Custom Cursors";
	y = mCustomCursorsCheckbox->mY - Dialog::mY + (IMAGE_CHECKBOX->mHeight>>2) + (FONT_DEFAULT->GetHeight() >> 1);
	g->DrawString(s,x,y);



	//g->DrawString("3D Mode:", m3DCheckbox->mX - mX - 25, m3DCheckbox->mY - mY - m3DCheckbox->mHeight + 20);
	//g->DrawString("Full Screen:", mFSCheckbox->mX - mX - 25, mFSCheckbox->mY - mY - mFSCheckbox->mHeight + 20);
	//g->DrawString("Custom Cursors:", mCustomCursorsCheckbox->mX - mX - 25, 
	//				mCustomCursorsCheckbox->mY - mY - mCustomCursorsCheckbox->mHeight + 20);

	g->SetColorizeImages(false);


}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::AddedToManager(WidgetManager* theWidgetManager)
{
	Dialog::AddedToManager(theWidgetManager);

	//// scale to game
	//mMusicVolumeSlider->Resize(
	//	mMusicVolumeSlider->Left(),
	//	mMusicVolumeSlider->Top(),
	//	mMusicVolumeSlider->Width() * gGraphicsScale,
	//	mMusicVolumeSlider->Height() * gGraphicsScale
	//	);


	//mSfxVolumeSlider->Resize(
	//	mSfxVolumeSlider->Left(),
	//	mSfxVolumeSlider->Top(),
	//	mSfxVolumeSlider->Width() * gGraphicsScale,
	//	mSfxVolumeSlider->Height() * gGraphicsScale
	//	);

	//mScreenDimensionsSlider->Resize(
	//	mScreenDimensionsSlider->Left(),
	//	mScreenDimensionsSlider->Top(),
	//	mScreenDimensionsSlider->Width() * gGraphicsScale,
	//	mScreenDimensionsSlider->Height() * gGraphicsScale
	//	);

	//m3DCheckbox->Resize(
	//	m3DCheckbox->Left(),
	//	m3DCheckbox->Top(),
	//	m3DCheckbox->Width() * gGraphicsScale,
	//	m3DCheckbox->Height() * gGraphicsScale
	//	);

	//mFSCheckbox->Resize(
	//	mFSCheckbox->Left(),
	//	mFSCheckbox->Top(),
	//	mFSCheckbox->Width() * gGraphicsScale,
	//	mFSCheckbox->Height() * gGraphicsScale
	//	);

	//mCustomCursorsCheckbox->Resize(
	//	mCustomCursorsCheckbox->Left(),
	//	mCustomCursorsCheckbox->Top(),
	//	mCustomCursorsCheckbox->Width() * gGraphicsScale,
	//	mCustomCursorsCheckbox->Height() * gGraphicsScale
	//	);

	// add widgets
	theWidgetManager->AddWidget(mMusicVolumeSlider);
	theWidgetManager->AddWidget(mSfxVolumeSlider);
	//theWidgetManager->AddWidget(mScreenDimensionsSlider);

	int checkWidth = IMAGE_CHECKBOX->GetWidth() / 2;
	m3DCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	m3DCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	mFSCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mFSCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	mCustomCursorsCheckbox->mUncheckedRect = Rect(0, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());
	mCustomCursorsCheckbox->mCheckedRect = Rect(checkWidth, 0, checkWidth, IMAGE_CHECKBOX->GetHeight());

	m3DCheckbox->mChecked = gSexyAppBase->Is3DAccelerated();
	mFSCheckbox->mChecked = !gSexyAppBase->mIsWindowed;
	mCustomCursorsCheckbox->mChecked = gSexyAppBase->mCustomCursorsEnabled;

	theWidgetManager->AddWidget(m3DCheckbox);
	theWidgetManager->AddWidget(mFSCheckbox);
	theWidgetManager->AddWidget(mCustomCursorsCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::RemovedFromManager(WidgetManager* theWidgetManager)
{
	Dialog::RemovedFromManager(theWidgetManager);
	theWidgetManager->RemoveWidget(mMusicVolumeSlider);
	theWidgetManager->RemoveWidget(mSfxVolumeSlider);
	//theWidgetManager->RemoveWidget(mScreenDimensionsSlider);
	theWidgetManager->RemoveWidget(mCustomCursorsCheckbox);
	theWidgetManager->RemoveWidget(m3DCheckbox);
	theWidgetManager->RemoveWidget(mFSCheckbox);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Dialog::Resize(theX, theY, theWidth, theHeight);

	int x, y, width, height;

	//// special case for small size 480x360
	//if( gGraphicsScale == 1.0f * (480.0f/1024.0f) )
	//{
	//	// music volume slider
	//	width = theWidth/2;
	//	height = IMAGE_SLIDER_THUMB->GetHeight();

	//	x = theX + (theWidth/6);
	//	y = theY + (theHeight/3);

	//	mMusicVolumeSlider->Resize(x, y, width, height);

	//	// sound volume slider
	//	y += (theHeight/5);

	//	mSfxVolumeSlider->Resize(x, y, width, height);

	//	// screen size slider
	//	y += (theHeight/5);

	//	mScreenDimensionsSlider->Resize(x, y, width, height);

	//	// 3d checkbox
	//	width = m3DCheckbox->Width(); 
	//	height = m3DCheckbox->Height();
	//	y = theY + (theHeight/4);
	//	x += (theWidth/2) + (IMAGE_CHECKBOX->mWidth / 2);

	//	m3DCheckbox->Resize(x, y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

	//	// fullscreen checkbox
	//	y += (theHeight/5);

	//	mFSCheckbox->Resize(x, y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

	//	// custom cursors
	//	y += (theHeight/5);

	//	mCustomCursorsCheckbox->Resize(x, y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);
	//}
	//else
	//{
		// music volume slider
		width = theWidth/2;
		height = IMAGE_SLIDER_THUMB->GetHeight();

		x = theX + (theWidth/2) - (width/2);
		y = theY + (theHeight/5);

		y += (theHeight/16);

		mMusicVolumeSlider->Resize(x, y, width, height);

		// sound volume slider
		y += (theHeight/8);

		mSfxVolumeSlider->Resize(x, y, width, height);

		// screen size slider
		//y += (theHeight/8);

		//mScreenDimensionsSlider->Resize(x + (48*gGraphicsScale), y, width-(96*gGraphicsScale), height);

		// 3d checkbox
		width = m3DCheckbox->Width(); 
		height = m3DCheckbox->Height();
		y += (theHeight/8);

		m3DCheckbox->Resize(x - (40*gGraphicsScale), y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

		// fullscreen checkbox
		y += (theHeight/10);

		mFSCheckbox->Resize(x - (40*gGraphicsScale), y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);

		// custom cursors
		y += (theHeight/10);

		mCustomCursorsCheckbox->Resize(x - (40*gGraphicsScale), y, IMAGE_CHECKBOX->mWidth / 2, IMAGE_CHECKBOX->mHeight);
	//}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::SliderVal(int theId, double theVal)
{
	switch( theId )
	{
	case OptionsDialog::MUSIC_SLIDER_ID:
		// Let's set the music volume to whatever the slider position is
		gSexyAppBase->SetMusicVolume(theVal);
		break;
	case OptionsDialog::SFX_SLIDER_ID:
		// Set the sound value
		gSexyAppBase->SetSfxVolume(theVal);

		if (!mSfxVolumeSlider->mDragging)
			gSexyAppBase->PlaySample(SOUND_TIMER);
		break;
	//case OptionsDialog::SCREENDIMENSIONS_SLIDER_ID:
	//	int aWidth;
	//	int aHeight;
	//	int aScreenSize;

	//	aScreenSize = (int)((theVal * 2.0f) + 0.5f);

	//	if( aScreenSize < 0 ) aScreenSize = 0;
	//	if( aScreenSize > 2 ) aScreenSize = 2;

	//	aWidth = mScreenSizeList[aScreenSize][0];
	//	aHeight = mScreenSizeList[aScreenSize][1];

	//	gSexyAppBase->RegistryWriteInteger("ScreenSize", aScreenSize);
	//	gSexyAppBase->RegistryWriteInteger("ScreenWidth", aWidth);
	//	gSexyAppBase->RegistryWriteInteger("ScreenHeight", aHeight);

	//	if( aWidth != gSexyAppBase->mWidth )
	//	{
	//		mScreenSizeChanged = true;

	//		SexyString title;
	//		SexyString text;

	//		if( mFSCheckbox->IsChecked() )
	//		{
	//			title = "Graphic Detail";
	//			text = "The graphic detail change will not take effect until the game is restarted.";
	//		}
	//		else
	//		{
	//			title = "Window Size";
	//			text = "The window size change will not take effect until the game is restarted.";
	//		}


	//		Dialog *aDialog = gSexyAppBase->DoDialog(MESSAGE_BOX_ID,
	//			true,
	//			title,
	//			text,
	//			"OK",
	//			Dialog::BUTTONS_FOOTER);

	//		// set fonts
	//		aDialog->SetHeaderFont(FONT_DEFAULT);
	//		aDialog->SetLinesFont(FONT_DEFAULT);
	//		aDialog->SetButtonFont(FONT_DEFAULT);

	//		aDialog->SetColor(COLOR_HEADER, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_A",255)
	//			));

	//		aDialog->SetColor(COLOR_LINES, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_TEXT_A",255)
	//			));

	//		aDialog->SetColor(COLOR_FOOTER, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_A",255)
	//			));

	//		aDialog->SetColor(COLOR_BUTTON_TEXT, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_A",255)
	//			));

	//		aDialog->SetColor(COLOR_BUTTON_TEXT_HILITE, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_A",255)
	//			));
	//		
	//		aDialog->SetColor(COLOR_BKG, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_A",255)
	//			));


	//		aDialog->SetColor(COLOR_OUTLINE, 
	//			Color(
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
	//			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_A",255)
	//			));


	//		int x, y, width, height;

	//		width = IMAGE_DIALOG_SMALL->mWidth;
	//		height = IMAGE_DIALOG_SMALL->mHeight;
	//		x = Dialog::Left() + (Dialog::Width() >> 1)	- (width >> 1);
	//		y = Dialog::Top() + (Dialog::Height() >> 1)	- (height >> 1);

	//		aDialog->Resize(x,y,width,height);
	//	}

	//	mScreenDimensionsSlider->mVal = (double)aScreenSize / 2.0f;
		//break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::ButtonDepress(int theId)
{
	Dialog::ButtonDepress(theId);

	// Previously, before framework 1.2, the line below looked like:
	// else if (theId == 2000 + mId)
	// With the 1.2 changes, there's just 1 slight incompatibility.
	// If the DialogListener for a dialog your app (by default), and
	// you don't implement your own DialogButton(De)press method (see V12Demo for
	// more info on this), and instead are trapping button presses within ButtonDepress
	// inside the dialog itself, then instead of the button ID being 2000 or 3000 + mId,
	// it's just 0 for no/cancel/1-button dialog, and 1 for yes/ok. Hence we changed our
	// check to look for 0, which is our default single close button at the bottom of this
	// dialog.
	if (theId == ID_YES)
	{
		// Close button pressed. Tell the app to remove ourselves. This
		// will automatically safe delete the dialog at a later time.
		// Let's apply the 3D and fullscreen mode settings first though.
		// We call SwitchScreenMode. The first parameter is whether or not to run
		// windowed (false means fullscreen), the second is whether or not to do 3d.

		// TODO: Check if current display is this size or smaller!
		gSexyAppBase->SwitchScreenMode(!mFSCheckbox->mChecked, m3DCheckbox->mChecked);

		gSexyAppBase->EnableCustomCursors(mCustomCursorsCheckbox->mChecked);

		mHost->OnOptionsClosed();

		// this->mAcceleration.z = -0.01f;
		// this->mAcceleration.y = 0.02f;
		gSexyAppBase->KillDialog(this);

	}


}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void OptionsDialog::CheckboxChecked(int theId, bool checked)
{
	Dialog* d = NULL;

	// We'll wait until the dialog box is closed before actually applying any effects,
	// since it's rather jarring if as soon as a user clicks the 3d or fullscreen
	// toggle buttons to change right then and there.

	if (theId == m3DCheckbox->mId)
	{
		if (checked)
		{
			// Turn on 3D acceleration. But we need to check if the user is
			// even allowed to have it on. Some cards are just not compatible
			// with the framework (less than 8MB RAM for instance):
			if (!gSexyAppBase->Is3DAccelerationSupported())
			{
				// It's not supported. Don't let the checkbox get checked.
				// Display an error dialog to the user to let them know why this happened.
				// The reason we don't use a Popup or Windows-based message box here is because
				// if the user was in full screen mode, they might not see the error message. Using
				// a game dialog box is the safest way to warn them.
				m3DCheckbox->SetChecked(false);
				d = gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, "Not Supported", 
					"Your video card does not meet the minimum requirements for\n\
					enhanced special effects.",
					"OK", Dialog::BUTTONS_FOOTER);

				d->Move( Widget::Left() + (Widget::Width()/2) - (d->Width()/2),
					Widget::Top() + (Widget::Height()/2) - (d->Height()/2) );
			}
			else if(!gSexyAppBase->Is3DAccelerationRecommended())
			{
				// We can also check if 3D acceleration is not recommended for this computer
				// with a call to Is3DAccelerationRecommended. This allows the user to override
				// the default setting, but with a warning that it might not work or might cause
				// problems. Some cards fail the detection process but wind up being OK to use.
				d = gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, "Warning", 
					"Your video card may not fully support enhanced special effects. \
					If the game runs slowly, disable this option.",
					"OK", Dialog::BUTTONS_FOOTER);

				d->Move( Widget::Left() + (Widget::Width()/2) - (d->Width()/2),
					Widget::Top() + (Widget::Height()/2) - (d->Height()/2) );
			}

		}
	}
	else if (theId == mFSCheckbox->mId)
	{
		// Believe it or not, some people are not allowed to play in windowed mode.
		// You can only run in windowed mode if your desktop is 16 or 32 bit, and
		// if the resolution of their desktop is GREATER THAN the resolution of the
		// game. It doesn't make sense to run a 800x600 game windowed when your desktop
		// is 800x600 or less.
		// We can determine if the user is not allowed to run in windowed mode by
		// checking the value of SexyAppBase's mForceFullScreen variable.
		if (gSexyAppBase->mForceFullscreen && !checked)
		{
			d = gSexyAppBase->DoDialog(OptionsDialog::MESSAGE_BOX_ID, true, "No Windowed Mode",
				"Windowed mode is only available if your desktop is in \
				either 16 bit or 32 bit color mode.", "OK", Dialog::BUTTONS_FOOTER);

			d->Move( Widget::Left() + (Widget::Width()/2) - (d->Width()/2),
				Widget::Top() + (Widget::Height()/2) - (d->Height()/2) );

			// re-check the box to indicate that fullscreen is still the selected mode:
			mFSCheckbox->SetChecked(true);
		}
	}
}


void OptionsDialog::Update()
{
	//int oldPauseLevel;

	//// temporarily disable pause
	//oldPauseLevel = gPauseLevel;

	// Floater::Update();

	//gPauseLevel = oldPauseLevel;


	Dialog::mX = mPosition.x + Dialog::Left();
	Dialog::mY = mPosition.y + Dialog::Top();

	this->Resize( Dialog::mX, Dialog::mY, Dialog::Width(), Dialog::Height() );

	if( mPosition.z < 1.0f )
	{
		Dialog::SetColor(COLOR_HEADER, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_HEADER_TEXT_B",255),
			mAlpha
			));

		Dialog::SetColor(COLOR_LINES, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_TEXT_B",255),
			mAlpha
			));

		Dialog::SetColor(COLOR_FOOTER, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_FOOTER_B",255),
			mAlpha
			));

		Dialog::SetColor(COLOR_BUTTON_TEXT, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_B",255),
			mAlpha
			));

		Dialog::SetColor(COLOR_BUTTON_TEXT_HILITE, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BUTTON_TEXT_HILITE_B",255),
			mAlpha
			));
		
		Dialog::SetColor(COLOR_BKG, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_BKG_B",255),
			mAlpha
			));

		Dialog::SetColor(COLOR_OUTLINE, 
			Color(
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_R",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_G",255),
			gSexyAppBase->GetInteger("DIALOG_COLOR_OUTLINE_B",255),
			mAlpha
			));

		mMusicVolumeSlider->SetVisible(false);
		mSfxVolumeSlider->SetVisible(false);
		//mScreenDimensionsSlider->SetVisible(false);

		m3DCheckbox->SetVisible(false);
		mFSCheckbox->SetVisible(false);
		mCustomCursorsCheckbox->SetVisible(false);
	}

	//if( mPosition.z <= 0.0f )
	//{
	//	gSexyAppBase->KillDialog(this);
	//}
}