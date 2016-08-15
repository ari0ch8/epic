#pragma once

//////////////////////////////////////////////////////////////////////////
//					LEVELPAK.h
//
//	Represents a dialog box displaying and allowing selection of level
//  paks.
//////////////////////////////////////////////////////////////////////////


#ifndef __LEVELPAK_DIALOG_H__
#define __LEVELPAK_DIALOG_H__

#include "../SexyAppFramework/Dialog.h"
#include "../SexyAppFramework/SliderListener.h"
#include "../SexyAppFramework/CheckboxListener.h"
#include "../SexyAppFramework/ListListener.h"
#include "../SexyAppFramework/ListWidget.h"
#include "../SexyAppFramework/ScrollbarWidget.h"
#include "../SexyAppFramework/ScrollbuttonWidget.h"

#include "LevelPakDialogHost.h"
#include "PlayerDialog.h"
#include "PlayerDialogHost.h"

namespace Sexy
{
	class Graphics;
	class Slider;
	class DialogButton;
	class Checkbox;

	class LevelPakDialog : public Dialog,
		public SliderListener,
		public CheckboxListener,
		public ListListener,
		public PlayerDialogHost

	{

	protected:

		Slider*			mMusicVolumeSlider;
		Slider*			mSfxVolumeSlider;
		Slider*			mScreenDimensionsSlider;

		DialogButton*	mQuitBtn;

		ListWidget*		mListWidget;
		ScrollbarWidget* mScrollbarWidget;

		DialogButton*	mArcadeButtonWidget;
		DialogButton*	mRandomButtonWidget;

		DialogButton*	mPlayerButtonWidget;


		std::list<std::string>		mLevelPakNames;

		LevelPakDialogHost*	mHost;

		int				mScreenSizeList[4][2];
		bool			mScreenSizeChanged;


	public:

		enum
		{
			MUSIC_SLIDER_ID,
			SFX_SLIDER_ID,
			SCREENDIMENSIONS_SLIDER_ID,
			QUIT_BTN_ID,
			FS_CHECKBOX_ID,
			HARDWARE_CHECKBOX_ID,
			CUSTOM_CURSORS_CHECKBOX_ID,
			MESSAGE_BOX_ID,
			DIALOG_ID,
			LISTWIDGET_ID,
			SCROLLBAR_ID,
			ARCADE_BUTTON_ID,
			RANDOM_BUTTON_ID,
			PLAYER_BUTTON_ID
		};


		Checkbox*		m3DCheckbox;		
		Checkbox*		mFSCheckbox;		
		Checkbox*		mCustomCursorsCheckbox;


	public:

        //////////////////////////////////////////////////////////////////////////
        //	Function: LevelPakDialog
		//	Parameters:
		//		b - A pointer to the board, used to unpause when dialog closes
		//
		//	Returns: none
		//////////////////////////////////////////////////////////////////////////		
		LevelPakDialog(LevelPakDialogHost* theHost);
		virtual ~LevelPakDialog();


		//////////////////////////////////////////////////////////////////////////
		//	Draw
		//////////////////////////////////////////////////////////////////////////		
		virtual void	Draw(Graphics* g);

		//////////////////////////////////////////////////////////////////////////
		//	ButtonDepress
		//////////////////////////////////////////////////////////////////////////		
		virtual void	ButtonDepress(int theId);

		//////////////////////////////////////////////////////////////////////////
		//	AddedToManager
		//////////////////////////////////////////////////////////////////////////		
		virtual void	AddedToManager(WidgetManager* theWidgetManager);

		//////////////////////////////////////////////////////////////////////////
		//	RemovedFromManager
		//////////////////////////////////////////////////////////////////////////		
		virtual void	RemovedFromManager(WidgetManager* theWidgetManager);

		//////////////////////////////////////////////////////////////////////////
		//	Resize
		//////////////////////////////////////////////////////////////////////////		
		virtual void	Resize(int theX, int theY, int theWidth, int theHeight);

		//////////////////////////////////////////////////////////////////////////
		//	SliderVal
		//////////////////////////////////////////////////////////////////////////		
		virtual void	SliderVal(int theId, double theVal);

		//////////////////////////////////////////////////////////////////////////
		//	CheckboxChecked
		//////////////////////////////////////////////////////////////////////////		
		void			CheckboxChecked(int theId, bool checked);

		void			ListClicked(int theId, int theIdx, int theClickCount);

		void			MouseWheel(int theDelta);



		void			FillLevelPakNames(void);

		void			OnPlayerDialogClosed(std::string playerName);
	};




}


#endif