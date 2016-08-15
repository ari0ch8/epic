#pragma once
#include "../SexyAppFramework/SexyAppBase.h"
#include "../SexyAppFramework/Widget.h"
#include "../SexyAppFramework/ButtonListener.h"
#include "../SexyAppFramework/EditWidget.h"
#include "../SexyAppFramework/EditListener.h"
#include "../SexyAppFramework/SexyMatrix.h"
//#include "OptionsDialogHost.h"
//#include "LevelPakDialog.h"
//#include "Movie.h"
//#include "Attractor.h"
//#include "Tile.h"
//#include "Floater.h"
//#include "Force.h"
//#include "Digit.h"
//#include "FloatingNumber.h"
//#include "PauseBGTint.h"
//#include "globals.h"
//
//#include "FancyTextWidget.h"
//#include "ImageLinkWidget.h"
//#include "PuzzlePakItem.h"

#include "map.h"

#include "action.h"

namespace Sexy
{


class MapView :
	public Sexy::Widget
{
public:
	MapView(void);
	~MapView(void);

	enum EditTileType
	{
		EDIT_TILE_TYPE_GROUND,
		EDIT_TILE_TYPE_WALL
	};

public:
	Map*	mMap;
	
	Action		mPendingAction;

	bool	mIsEditting;
	int		mEditorGroundTileNumber;
	int		mEditorWallTileNumber;

	EditTileType mEditTileType;

	int		mCenterX;
	int		mCenterY;

	int mCursorx;
	int mCursory;
	bool mCancelMouseClick;

	bool mIsShowingGroundTiles;
	bool mIsShowingWallTiles;

	int mSelectedTileNumber;
	Actor*	mSelectedActor;

	int GetSelectedTileNumber(void);
	int GetSelectedTileX(void);
	int GetSelectedTileY(void);


	void SetPendingAction(Action::ActionType theActionType);

	void Draw(Graphics* g);

	virtual void	MouseMove(int x, int y);
	virtual void	MouseDrag(int x, int y);
	virtual void	MouseUp(int x, int y, int theBtnNum, int theClickCount);
	virtual void	MouseDown(int x, int y, int theBtnNum, int theClickCount);

};


}