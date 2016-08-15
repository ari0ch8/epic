#include "MapView.h"

#include <sstream>

#include "event.h"
#include "globals.h"

using namespace Sexy;


MapView::MapView(void)
{
	mMap = NULL;
	mCenterX = 0;
	mCenterY = 0;

	mCancelMouseClick = false;

	mIsShowingGroundTiles = false;
	mIsShowingWallTiles = false;

	mSelectedTileNumber = 0;
	mSelectedActor = NULL;

	mIsEditting = false;
	mEditTileType = EDIT_TILE_TYPE_GROUND;

	mEditorGroundTileNumber = 0;
	mEditorWallTileNumber = 0;

	mCursorx = 0;
	mCursory = 0;

	mPendingAction.mActionType = Action::None;

	Resize(0,0,768,768);

//	mMouseVisible = false;
}

MapView::~MapView(void)
{
}


void MapView::MouseMove(int x, int y)
{
	Widget::MouseMove(x,y);

	int selectedTileDistanceFromMainActor = 0;

	mCursorx = x;
	mCursory = y;

	mSelectedTileNumber = GetSelectedTileNumber();

	if( mIsEditting )
	{
	}
	else
	{
		switch( mPendingAction.mActionType )
		{
		default:
			gSexyAppBase->SetCursor(CURSOR_POINTER);
			mSelectedActor = NULL;
			break;
		case Action::Attack:
		case Action::Talk:
			selectedTileDistanceFromMainActor = mMap->GetDistance( GetSelectedTileX(), GetSelectedTileY(), mCenterX, mCenterY );

			if( selectedTileDistanceFromMainActor <= 1 )
			{
				mSelectedActor = mMap->mActors[mSelectedTileNumber];

				if( mSelectedActor != NULL )
				{
					gSexyAppBase->SetCursor(CURSOR_HAND);
				}
				else
				{
					gSexyAppBase->SetCursor(CURSOR_POINTER);
				}
			}
			else
			{
				gSexyAppBase->SetCursor(CURSOR_POINTER);
			}
			break;
		}
	}
}


void MapView::MouseDrag(int x, int y)
{
	mCursorx = x;
	mCursory = y;

	int mOldSelectedTileNumber = mSelectedTileNumber;
	mSelectedTileNumber = GetSelectedTileNumber();

	if( mOldSelectedTileNumber != mSelectedTileNumber )
	{
		mCancelMouseClick = true;
	}

#ifdef __EPICEDITOR
	if( mIsEditting && !mIsShowingGroundTiles && !mIsShowingWallTiles)
	{
		switch( mEditTileType )
		{
		case EDIT_TILE_TYPE_GROUND:
			mMap->mGroundTiles[mSelectedTileNumber] = &gTileManager.mGroundTiles[mEditorGroundTileNumber];
			break;
		case EDIT_TILE_TYPE_WALL:
			mMap->mWallTiles[mSelectedTileNumber] = &gTileManager.mWallTiles[mEditorWallTileNumber];
			break; //mEditorWallTileNumber
		}
	}

#endif

}


void MapView::MouseDown(int x, int y, int theBtnNum, int theClickCount)
{
	if( mIsEditting )
	{
		switch( theBtnNum )
		{
		case 0:
			switch( mEditTileType )
			{
			case EDIT_TILE_TYPE_GROUND:
				mMap->mGroundTiles[mSelectedTileNumber] = &gTileManager.mGroundTiles[mEditorGroundTileNumber];
				break;
			case EDIT_TILE_TYPE_WALL:
				mMap->mWallTiles[mSelectedTileNumber] = &gTileManager.mWallTiles[mEditorWallTileNumber];
				break; //mEditorWallTileNumber
			}
			break;
		case 1:
			switch( mEditTileType )
			{
			case EDIT_TILE_TYPE_GROUND:
				mIsShowingGroundTiles = true;
				break;
			case EDIT_TILE_TYPE_WALL:
				mIsShowingWallTiles = true;
				break;
			}
			break;
		}
	}

	Widget::MouseDown(x,y,theBtnNum,theClickCount);
}


void MapView::MouseUp(int x, int y, int theBtnNum, int theClickCount)
{
	Widget::MouseUp(x,y,theBtnNum,theClickCount);

#ifdef __EPICEDITOR

	if( mIsEditting )
	{
		switch( theBtnNum )
		{
		case 1:
			switch( mEditTileType )
			{
			case EDIT_TILE_TYPE_GROUND:
				mIsShowingGroundTiles = false;
				mEditorGroundTileNumber = (((y)/64) * 12) + 
					((x)/64);
				break;
			case EDIT_TILE_TYPE_WALL:
				mIsShowingWallTiles = false;
				mEditorWallTileNumber = (((y)/64) * 12) + 
					((x)/64);
				break;
			}
			break;
		}
	}

#endif


	if( mCancelMouseClick )
	{
		mCancelMouseClick = false;
		mSelectedActor = NULL;
	}
	else
	{
		// todo: check button number
		// todo: check that cursor hasn't moved off selected tile
		switch( mPendingAction.mActionType )
		{
		case Action::Talk:
			if( mSelectedActor != NULL )
			{
				// todo: look for conversion and run talk sequence
				Event anEvent;
				std::stringstream textStream;

				if( mSelectedActor == &gActorManager.mActors[0] )
				{
					// todo: change based on sex
					textStream << mSelectedActor->mName << " talks to himself, but finds the conversation very boring.";
				}
				else
				{
					// textStream << gActorManager.mActors[0].mName << " tried talking to " << mSelectedActor->mName << " but was ignored.";
					textStream << mSelectedActor->mName << " asks \"Want to trade?\"";
				}

				anEvent.mDescription = textStream.str();

				gEventManager.AddEventToQueue(anEvent);

				SetPendingAction(Action::None);
			}
			break;

		case Action::Attack:
			if( mSelectedActor != NULL )
			{
				// todo: a lot!
				Event anEvent;
				std::stringstream textStream;

				if( mSelectedActor == &gActorManager.mActors[0] )
				{
					// todo: change based on sex
					textStream << mSelectedActor->mName << " considers hurting himself, but quickly changes his mind.";
				}
				else
				{
					textStream << gActorManager.mActors[0].mName << " attacks " << mSelectedActor->mName << " with his hands and kills him!";
					// todo: remove actor
					mMap->mActors[mSelectedTileNumber] = NULL;
				}

				anEvent.mDescription = textStream.str();

				gEventManager.AddEventToQueue(anEvent);

				SetPendingAction(Action::None);
			}
			break;
		}

	} // end if mouse click was not canceled
}


int MapView::GetSelectedTileNumber(void)
{
	return (GetSelectedTileY() * mMap->mWidth) + GetSelectedTileX();
}

int MapView::GetSelectedTileX(void)
{
	int maptilex;
	int screentilex;

	screentilex = (mCursorx + 32) >> 6;
	maptilex = screentilex + mCenterX - 6;

	return maptilex;
}

int MapView::GetSelectedTileY(void)
{
	int maptiley;
	int screentiley;

	screentiley = (mCursory + 32) >> 6;
	maptiley = screentiley + mCenterY - 6;

	return maptiley;
}



void MapView::Draw(Graphics* g)
{
	int x, y;
	int tileno;
	GroundTile* groundTile;
	ActorTile*	actorTile;
	WallTile*	wallTile;

	for( int mapy=0; mapy<13; mapy++ )
	{
		for( int mapx=0; mapx<13; mapx++ )
		{
			x = -32 + (mapx*64);
			y = -32 + (mapy*64);

			tileno = ((mCenterY - 6 + mapy) * mMap->mWidth) + (mCenterX - 6 + mapx);

			if( tileno < 0 || tileno >= mMap->mTileCount )
			{
				continue;
			}

			groundTile = mMap->mGroundTiles[tileno];
			g->DrawImageCel(groundTile->image, x, y, groundTile->imagecol, groundTile->imagerow);

			wallTile = mMap->mWallTiles[tileno];

			if( wallTile != NULL )
			{
				g->DrawImageCel(wallTile->image, x, y, wallTile->imagecol, wallTile->imagerow);
			}

			if( mMap->mActors[tileno] )
			{
				actorTile = mMap->mActors[tileno]->mActorTile;
				g->DrawImageCel(actorTile->image, x, y, actorTile->imagecol, actorTile->imagerow);
			}

			if( mPendingAction.mActionType != Action::None )
			{
				// todo: change to distance from src actor
				if( mMap->GetDistance( mapx, mapy, 6, 6 ) > 1 )
				{
					g->SetColor(Color(0,0,0,64));
					g->FillRect(x, y, 64, 64);
				}
			}
		}
	}

	Actor* mActor;
	std::stringstream ss;

	switch( mPendingAction.mActionType )
	{
	case Action::Attack:
	case Action::Talk:
		mActor = mMap->mActors[mSelectedTileNumber];

		if( mMap->GetDistance( GetSelectedTileX(), GetSelectedTileY(), mCenterX, mCenterY ) > 1 )
		{
			mActor = NULL;
		}

		g->SetFont(FONT_DEFAULT);
		g->SetColor(Color(255,255,64,255));

		switch( mPendingAction.mActionType )
		{
			case Action::Attack:
				if( mActor == NULL )
				{
					ss << "Who do you want to attack?";
				}
				else
				{
					ss << "Attack " << mActor->mName;
				}
				break;
			case Action::Talk:
				if( mActor == NULL )
				{
					ss << "Who do you want to talk to?";
				}
				else
				{
					ss << "Talk to " << mActor->mName;
				}
				break;
		}

		WriteString(g, ss.str(), Width() / 2, Height() - 24, -1, TEXT_JUSTIFY_CENTER);

		break;
	}

#ifdef __EPICEDITOR

	if( mIsEditting )
	{
		switch( mEditTileType )
		{
		case EDIT_TILE_TYPE_GROUND:
			g->DrawImageCel(gTileManager.mGroundTiles[mEditorGroundTileNumber].image, mCursorx, mCursory, gTileManager.mGroundTiles[mEditorGroundTileNumber].imagecol, gTileManager.mGroundTiles[mEditorGroundTileNumber].imagerow);
			break;
		case EDIT_TILE_TYPE_WALL:
			g->DrawImageCel(gTileManager.mWallTiles[mEditorWallTileNumber].image, mCursorx, mCursory, gTileManager.mWallTiles[mEditorWallTileNumber].imagecol, gTileManager.mWallTiles[mEditorWallTileNumber].imagerow);
			break;
		}

		g->SetFont(FONT_DEFAULT);
		g->SetColor(Color(255,255,64,255));

		ss.str("");
		ss << "Edit Mode (" << mMap->mName << "(" << mMap->mID << ")" << " " << mCenterX << ", " << mCenterY << " mEditTileType=" << mEditTileType << ")";

		WriteString(g, ss.str(), Width() / 2, Height() - 24, -1, TEXT_JUSTIFY_CENTER);
	}

	if( mIsShowingGroundTiles )
	{
		g->SetColor(Color::Black);
		g->FillRect(0,0,Width(),Height());
		g->DrawImage(gTileManager.mGroundTiles[mEditorGroundTileNumber].image, 0, 0);
	}

	if( mIsShowingWallTiles )
	{
		g->SetColor(Color::Black);
		g->FillRect(0,0,Width(),Height());
		g->DrawImage(gTileManager.mWallTiles[mEditorWallTileNumber].image, 0, 0);
	}

#endif

}



void MapView::SetPendingAction(Action::ActionType theActionType)
{
	mPendingAction.mActionType = theActionType;

	// refresh cursor and selected items
	MouseMove(mCursorx, mCursory);

	//switch( mPendingAction.mActionType )
	//{
	//default:
	//	gSexyAppBase->SetCursor(CURSOR_POINTER);
	//	mSelectedActor = NULL;
	//	break;
	//case Action::Talk:
	//	if( mMap->GetDistance( GetSelectedTileX(), GetSelectedTileY(), mCenterX, mCenterY ) <= 1 )
	//	{
	//		gSexyAppBase->SetCursor(CURSOR_HAND);
	//		mSelectedActor = NULL;
	//	}
	//	else
	//	{
	//		gSexyAppBase->SetCursor(CURSOR_POINTER);
	//		mSelectedActor = NULL;
	//	}
	//	break;
	//}
}
