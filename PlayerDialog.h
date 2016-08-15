#pragma once

//////////////////////////////////////////////////////////////////////////
//					LEVELPAK.h
//
//	Represents a dialog box displaying and allowing selection of level
//  paks.
//////////////////////////////////////////////////////////////////////////


#ifndef __PLAYER_DIALOG_H__
#define __PLAYER_DIALOG_H__

#include "../SexyAppFramework/Dialog.h"
#include "../SexyAppFramework/ListListener.h"
#include "../SexyAppFramework/ListWidget.h"
#include "../SexyAppFramework/ScrollbarWidget.h"
#include "../SexyAppFramework/ScrollbuttonWidget.h"

#include "PlayerDialogHost.h"

namespace Sexy
{
	class Graphics;
	class Slider;
	class DialogButton;
	class Checkbox;

	class PlayerDialog : public Dialog,
		public ListListener

	{

	protected:


		ListWidget*		mListWidget;
		ScrollbarWidget* mScrollbarWidget;
		std::list<std::string>		mPlayerNames;

		PlayerDialogHost*	mHost;


	public:

		enum
		{
			DIALOG_ID,
			LISTWIDGET_ID,
			SCROLLBAR_ID,
		};

	public:

        //////////////////////////////////////////////////////////////////////////
        //	Function: LevelPakDialog
		//	Parameters:
		//		b - A pointer to the board, used to unpause when dialog closes
		//
		//	Returns: none
		//////////////////////////////////////////////////////////////////////////		
		PlayerDialog(PlayerDialogHost* theHost);
		virtual ~PlayerDialog();


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


		void			ListClicked(int theId, int theIdx, int theClickCount);

		void			MouseWheel(int theDelta);

	};




}


#endif