#include "res.h"
#include "SexyAppFramework/ResourceManager.h"

using namespace Sexy;

#pragma warning(disable:4311 4312)

static bool gNeedRecalcVariableToIdMap = false;

bool Sexy::ExtractResourcesByName(ResourceManager *theManager, const char *theName)
{
	if (strcmp(theName,"Game")==0) return ExtractGameResources(theManager);
	if (strcmp(theName,"GameScreen")==0) return ExtractGameScreenResources(theManager);
	if (strcmp(theName,"Init")==0) return ExtractInitResources(theManager);
	return false;
}

Sexy::ResourceId Sexy::GetIdByStringId(const char *theStringId)
{
	typedef std::map<std::string,int> MyMap;
	static MyMap aMap;
	if(aMap.empty())
	{
		for(int i=0; i<RESOURCE_ID_MAX; i++)
			aMap[GetStringIdById(i)] = i;
	}

	MyMap::iterator anItr = aMap.find(theStringId);
	if (anItr == aMap.end())
		return RESOURCE_ID_MAX;
	else
		return (ResourceId) anItr->second;
}

// Game Resources
Image* Sexy::IMAGE_ARROW;
Image* Sexy::IMAGE_BACKGROUND_LAYER_3;
Image* Sexy::IMAGE_BOARD_BG;
Image* Sexy::IMAGE_CHECKBOX;
Image* Sexy::IMAGE_DIALOG_BOX;
Image* Sexy::IMAGE_DIALOG_BUTTON;
Image* Sexy::IMAGE_DIALOG_BUTTON_SMALL;
Image* Sexy::IMAGE_DIALOG_LARGE;
Image* Sexy::IMAGE_DIALOG_SMALL;
Image* Sexy::IMAGE_NUMBERS;
Image* Sexy::IMAGE_PAK_COMPLETE_END_GAME;
Image* Sexy::IMAGE_PAK_COMPLETE_END_GAME_2_TEXT;
Image* Sexy::IMAGE_PAK_COMPLETE_END_GAME_3_TEXT;
Image* Sexy::IMAGE_PAK_COMPLETE_END_GAME_TEXT;
Image* Sexy::IMAGE_PAK_COMPLETE_HOW_TO_PLAY;
Image* Sexy::IMAGE_PAK_TEXT;
Image* Sexy::IMAGE_SLIDER_BUTTON;
Image* Sexy::IMAGE_SLIDER_THUMB;
Image* Sexy::IMAGE_SLIDER_TRACK;
Image* Sexy::IMAGE_YOU_LOSE;
Image* Sexy::IMAGE_actors;
Image* Sexy::IMAGE_anchor;
Image* Sexy::IMAGE_anchor_large;
Image* Sexy::IMAGE_anchor_sm;
Image* Sexy::IMAGE_compass;
Image* Sexy::IMAGE_compass_large;
Image* Sexy::IMAGE_compass_sm;
Image* Sexy::IMAGE_gamepanel;
Image* Sexy::IMAGE_ground;
Image* Sexy::IMAGE_journal;
Image* Sexy::IMAGE_journal_large;
Image* Sexy::IMAGE_journal_sm;
Image* Sexy::IMAGE_jug;
Image* Sexy::IMAGE_jug_large;
Image* Sexy::IMAGE_jug_sm;
Image* Sexy::IMAGE_key;
Image* Sexy::IMAGE_key_large;
Image* Sexy::IMAGE_key_sm;
Image* Sexy::IMAGE_lantern;
Image* Sexy::IMAGE_lantern_large;
Image* Sexy::IMAGE_lantern_sm;
Image* Sexy::IMAGE_lock;
Image* Sexy::IMAGE_map;
Image* Sexy::IMAGE_map_large;
Image* Sexy::IMAGE_map_sm;
Image* Sexy::IMAGE_navcompass;
Image* Sexy::IMAGE_navcompass_large;
Image* Sexy::IMAGE_navcompass_sm;
Image* Sexy::IMAGE_pipe;
Image* Sexy::IMAGE_pipe_large;
Image* Sexy::IMAGE_pipe_sm;
Image* Sexy::IMAGE_small_lock;
Image* Sexy::IMAGE_smile;
Image* Sexy::IMAGE_spyglass;
Image* Sexy::IMAGE_spyglass_large;
Image* Sexy::IMAGE_spyglass_sm;
Image* Sexy::IMAGE_unknown;
Image* Sexy::IMAGE_unknown_large;
Image* Sexy::IMAGE_walls;
Image* Sexy::IMAGE_watch;
Image* Sexy::IMAGE_watch_large;
Image* Sexy::IMAGE_watch_sm;
Image* Sexy::IMAGE_wheel;
Image* Sexy::IMAGE_wheel_large;
Image* Sexy::IMAGE_wheel_sm;
int Sexy::SOUND_BLOX_DISAPPEAR;
int Sexy::SOUND_BLOX_HIT;
int Sexy::SOUND_BUTTON;
int Sexy::SOUND_GAME_OVER_RESTART;
int Sexy::SOUND_LEVEL_UP;
int Sexy::SOUND_PAK_COMPLETE;
int Sexy::SOUND_TIMER;
int Sexy::SOUND_footstep1;

bool Sexy::ExtractGameResources(ResourceManager *theManager)
{
	gNeedRecalcVariableToIdMap = true;

	ResourceManager &aMgr = *theManager;
	try
	{
		IMAGE_ARROW = aMgr.GetImageThrow("IMAGE_ARROW");
		IMAGE_BACKGROUND_LAYER_3 = aMgr.GetImageThrow("IMAGE_BACKGROUND_LAYER_3");
		IMAGE_BOARD_BG = aMgr.GetImageThrow("IMAGE_BOARD_BG");
		IMAGE_CHECKBOX = aMgr.GetImageThrow("IMAGE_CHECKBOX");
		IMAGE_DIALOG_BOX = aMgr.GetImageThrow("IMAGE_DIALOG_BOX");
		IMAGE_DIALOG_BUTTON = aMgr.GetImageThrow("IMAGE_DIALOG_BUTTON");
		IMAGE_DIALOG_BUTTON_SMALL = aMgr.GetImageThrow("IMAGE_DIALOG_BUTTON_SMALL");
		IMAGE_DIALOG_LARGE = aMgr.GetImageThrow("IMAGE_DIALOG_LARGE");
		IMAGE_DIALOG_SMALL = aMgr.GetImageThrow("IMAGE_DIALOG_SMALL");
		IMAGE_NUMBERS = aMgr.GetImageThrow("IMAGE_NUMBERS");
		IMAGE_PAK_COMPLETE_END_GAME = aMgr.GetImageThrow("IMAGE_PAK_COMPLETE_END_GAME");
		IMAGE_PAK_COMPLETE_END_GAME_2_TEXT = aMgr.GetImageThrow("IMAGE_PAK_COMPLETE_END_GAME_2_TEXT");
		IMAGE_PAK_COMPLETE_END_GAME_3_TEXT = aMgr.GetImageThrow("IMAGE_PAK_COMPLETE_END_GAME_3_TEXT");
		IMAGE_PAK_COMPLETE_END_GAME_TEXT = aMgr.GetImageThrow("IMAGE_PAK_COMPLETE_END_GAME_TEXT");
		IMAGE_PAK_COMPLETE_HOW_TO_PLAY = aMgr.GetImageThrow("IMAGE_PAK_COMPLETE_HOW_TO_PLAY");
		IMAGE_PAK_TEXT = aMgr.GetImageThrow("IMAGE_PAK_TEXT");
		IMAGE_SLIDER_BUTTON = aMgr.GetImageThrow("IMAGE_SLIDER_BUTTON");
		IMAGE_SLIDER_THUMB = aMgr.GetImageThrow("IMAGE_SLIDER_THUMB");
		IMAGE_SLIDER_TRACK = aMgr.GetImageThrow("IMAGE_SLIDER_TRACK");
		IMAGE_YOU_LOSE = aMgr.GetImageThrow("IMAGE_YOU_LOSE");
		IMAGE_actors = aMgr.GetImageThrow("IMAGE_actors");
		IMAGE_anchor = aMgr.GetImageThrow("IMAGE_anchor");
		IMAGE_anchor_large = aMgr.GetImageThrow("IMAGE_anchor_large");
		IMAGE_anchor_sm = aMgr.GetImageThrow("IMAGE_anchor_sm");
		IMAGE_compass = aMgr.GetImageThrow("IMAGE_compass");
		IMAGE_compass_large = aMgr.GetImageThrow("IMAGE_compass_large");
		IMAGE_compass_sm = aMgr.GetImageThrow("IMAGE_compass_sm");
		IMAGE_gamepanel = aMgr.GetImageThrow("IMAGE_gamepanel");
		IMAGE_ground = aMgr.GetImageThrow("IMAGE_ground");
		IMAGE_journal = aMgr.GetImageThrow("IMAGE_journal");
		IMAGE_journal_large = aMgr.GetImageThrow("IMAGE_journal_large");
		IMAGE_journal_sm = aMgr.GetImageThrow("IMAGE_journal_sm");
		IMAGE_jug = aMgr.GetImageThrow("IMAGE_jug");
		IMAGE_jug_large = aMgr.GetImageThrow("IMAGE_jug_large");
		IMAGE_jug_sm = aMgr.GetImageThrow("IMAGE_jug_sm");
		IMAGE_key = aMgr.GetImageThrow("IMAGE_key");
		IMAGE_key_large = aMgr.GetImageThrow("IMAGE_key_large");
		IMAGE_key_sm = aMgr.GetImageThrow("IMAGE_key_sm");
		IMAGE_lantern = aMgr.GetImageThrow("IMAGE_lantern");
		IMAGE_lantern_large = aMgr.GetImageThrow("IMAGE_lantern_large");
		IMAGE_lantern_sm = aMgr.GetImageThrow("IMAGE_lantern_sm");
		IMAGE_lock = aMgr.GetImageThrow("IMAGE_lock");
		IMAGE_map = aMgr.GetImageThrow("IMAGE_map");
		IMAGE_map_large = aMgr.GetImageThrow("IMAGE_map_large");
		IMAGE_map_sm = aMgr.GetImageThrow("IMAGE_map_sm");
		IMAGE_navcompass = aMgr.GetImageThrow("IMAGE_navcompass");
		IMAGE_navcompass_large = aMgr.GetImageThrow("IMAGE_navcompass_large");
		IMAGE_navcompass_sm = aMgr.GetImageThrow("IMAGE_navcompass_sm");
		IMAGE_pipe = aMgr.GetImageThrow("IMAGE_pipe");
		IMAGE_pipe_large = aMgr.GetImageThrow("IMAGE_pipe_large");
		IMAGE_pipe_sm = aMgr.GetImageThrow("IMAGE_pipe_sm");
		IMAGE_small_lock = aMgr.GetImageThrow("IMAGE_small_lock");
		IMAGE_smile = aMgr.GetImageThrow("IMAGE_smile");
		IMAGE_spyglass = aMgr.GetImageThrow("IMAGE_spyglass");
		IMAGE_spyglass_large = aMgr.GetImageThrow("IMAGE_spyglass_large");
		IMAGE_spyglass_sm = aMgr.GetImageThrow("IMAGE_spyglass_sm");
		IMAGE_unknown = aMgr.GetImageThrow("IMAGE_unknown");
		IMAGE_unknown_large = aMgr.GetImageThrow("IMAGE_unknown_large");
		IMAGE_walls = aMgr.GetImageThrow("IMAGE_walls");
		IMAGE_watch = aMgr.GetImageThrow("IMAGE_watch");
		IMAGE_watch_large = aMgr.GetImageThrow("IMAGE_watch_large");
		IMAGE_watch_sm = aMgr.GetImageThrow("IMAGE_watch_sm");
		IMAGE_wheel = aMgr.GetImageThrow("IMAGE_wheel");
		IMAGE_wheel_large = aMgr.GetImageThrow("IMAGE_wheel_large");
		IMAGE_wheel_sm = aMgr.GetImageThrow("IMAGE_wheel_sm");
		SOUND_BLOX_DISAPPEAR = aMgr.GetSoundThrow("SOUND_BLOX_DISAPPEAR");
		SOUND_BLOX_HIT = aMgr.GetSoundThrow("SOUND_BLOX_HIT");
		SOUND_BUTTON = aMgr.GetSoundThrow("SOUND_BUTTON");
		SOUND_GAME_OVER_RESTART = aMgr.GetSoundThrow("SOUND_GAME_OVER_RESTART");
		SOUND_LEVEL_UP = aMgr.GetSoundThrow("SOUND_LEVEL_UP");
		SOUND_PAK_COMPLETE = aMgr.GetSoundThrow("SOUND_PAK_COMPLETE");
		SOUND_TIMER = aMgr.GetSoundThrow("SOUND_TIMER");
		SOUND_footstep1 = aMgr.GetSoundThrow("SOUND_footstep1");
	}
	catch(ResourceManagerException&)
	{
		return false;
	}
	return true;
}

// GameScreen Resources
Image* Sexy::IMAGE_BRANDING;
Image* Sexy::IMAGE_LOADER_BAR;
Image* Sexy::IMAGE_LOADER_LOADINGTXT;

bool Sexy::ExtractGameScreenResources(ResourceManager *theManager)
{
	gNeedRecalcVariableToIdMap = true;

	ResourceManager &aMgr = *theManager;
	try
	{
		IMAGE_BRANDING = aMgr.GetImageThrow("IMAGE_BRANDING");
		IMAGE_LOADER_BAR = aMgr.GetImageThrow("IMAGE_LOADER_BAR");
		IMAGE_LOADER_LOADINGTXT = aMgr.GetImageThrow("IMAGE_LOADER_LOADINGTXT");
	}
	catch(ResourceManagerException&)
	{
		return false;
	}
	return true;
}

// Init Resources
Font* Sexy::FONT_BERLINSANS14;
Font* Sexy::FONT_BERLINSANS24;
Font* Sexy::FONT_DEFAULT;
Image* Sexy::IMAGE_BLOCKS;
Image* Sexy::IMAGE_COMPANY_LOGO;
Image* Sexy::IMAGE_CUSTOM_DRAGGING;
Image* Sexy::IMAGE_CUSTOM_HAND;
Image* Sexy::IMAGE_CUSTOM_POINTER;
Image* Sexy::IMAGE_CUSTOM_TEXT;
Image* Sexy::IMAGE_TITLE_LOGO;

bool Sexy::ExtractInitResources(ResourceManager *theManager)
{
	gNeedRecalcVariableToIdMap = true;

	ResourceManager &aMgr = *theManager;
	try
	{
		FONT_BERLINSANS14 = aMgr.GetFontThrow("FONT_BERLINSANS14");
		FONT_BERLINSANS24 = aMgr.GetFontThrow("FONT_BERLINSANS24");
		FONT_DEFAULT = aMgr.GetFontThrow("FONT_DEFAULT");
		IMAGE_BLOCKS = aMgr.GetImageThrow("IMAGE_BLOCKS");
		IMAGE_COMPANY_LOGO = aMgr.GetImageThrow("IMAGE_COMPANY_LOGO");
		IMAGE_CUSTOM_DRAGGING = aMgr.GetImageThrow("IMAGE_CUSTOM_DRAGGING");
		IMAGE_CUSTOM_HAND = aMgr.GetImageThrow("IMAGE_CUSTOM_HAND");
		IMAGE_CUSTOM_POINTER = aMgr.GetImageThrow("IMAGE_CUSTOM_POINTER");
		IMAGE_CUSTOM_TEXT = aMgr.GetImageThrow("IMAGE_CUSTOM_TEXT");
		IMAGE_TITLE_LOGO = aMgr.GetImageThrow("IMAGE_TITLE_LOGO");
	}
	catch(ResourceManagerException&)
	{
		return false;
	}
	return true;
}

static void* gResources[] =
{
	&FONT_DEFAULT,
	&FONT_BERLINSANS24,
	&FONT_BERLINSANS14,
	&IMAGE_CUSTOM_POINTER,
	&IMAGE_CUSTOM_HAND,
	&IMAGE_CUSTOM_DRAGGING,
	&IMAGE_CUSTOM_TEXT,
	&IMAGE_TITLE_LOGO,
	&IMAGE_COMPANY_LOGO,
	&IMAGE_BLOCKS,
	&IMAGE_LOADER_BAR,
	&IMAGE_LOADER_LOADINGTXT,
	&IMAGE_BRANDING,
	&SOUND_TIMER,
	&IMAGE_YOU_LOSE,
	&IMAGE_PAK_COMPLETE_HOW_TO_PLAY,
	&IMAGE_PAK_COMPLETE_END_GAME,
	&IMAGE_PAK_COMPLETE_END_GAME_TEXT,
	&IMAGE_PAK_COMPLETE_END_GAME_2_TEXT,
	&IMAGE_PAK_COMPLETE_END_GAME_3_TEXT,
	&IMAGE_PAK_TEXT,
	&IMAGE_BOARD_BG,
	&IMAGE_SLIDER_BUTTON,
	&IMAGE_NUMBERS,
	&IMAGE_BACKGROUND_LAYER_3,
	&IMAGE_CHECKBOX,
	&IMAGE_DIALOG_BOX,
	&IMAGE_DIALOG_LARGE,
	&IMAGE_DIALOG_SMALL,
	&IMAGE_DIALOG_BUTTON,
	&IMAGE_DIALOG_BUTTON_SMALL,
	&IMAGE_SLIDER_TRACK,
	&IMAGE_SLIDER_THUMB,
	&IMAGE_ARROW,
	&IMAGE_lantern,
	&IMAGE_spyglass,
	&IMAGE_watch,
	&IMAGE_anchor,
	&IMAGE_map,
	&IMAGE_pipe,
	&IMAGE_jug,
	&IMAGE_wheel,
	&IMAGE_journal,
	&IMAGE_compass,
	&IMAGE_navcompass,
	&IMAGE_key,
	&IMAGE_unknown,
	&IMAGE_lock,
	&IMAGE_anchor_sm,
	&IMAGE_pipe_sm,
	&IMAGE_wheel_sm,
	&IMAGE_map_sm,
	&IMAGE_journal_sm,
	&IMAGE_compass_sm,
	&IMAGE_lantern_sm,
	&IMAGE_key_sm,
	&IMAGE_watch_sm,
	&IMAGE_spyglass_sm,
	&IMAGE_navcompass_sm,
	&IMAGE_jug_sm,
	&IMAGE_small_lock,
	&IMAGE_smile,
	&IMAGE_lantern_large,
	&IMAGE_spyglass_large,
	&IMAGE_watch_large,
	&IMAGE_anchor_large,
	&IMAGE_map_large,
	&IMAGE_pipe_large,
	&IMAGE_jug_large,
	&IMAGE_wheel_large,
	&IMAGE_journal_large,
	&IMAGE_compass_large,
	&IMAGE_navcompass_large,
	&IMAGE_key_large,
	&IMAGE_unknown_large,
	&IMAGE_gamepanel,
	&IMAGE_ground,
	&IMAGE_walls,
	&IMAGE_actors,
	&SOUND_BUTTON,
	&SOUND_LEVEL_UP,
	&SOUND_PAK_COMPLETE,
	&SOUND_BLOX_DISAPPEAR,
	&SOUND_BLOX_HIT,
	&SOUND_GAME_OVER_RESTART,
	&SOUND_footstep1,
	NULL
};

Image* Sexy::LoadImageById(ResourceManager *theManager, int theId)
{
	return (*((Image**)gResources[theId]) = theManager->LoadImage(GetStringIdById(theId)));
}

void Sexy::ReplaceImageById(ResourceManager *theManager, int theId, Image *theImage)
{
	theManager->ReplaceImage(GetStringIdById(theId),theImage);
	*(Image**)gResources[theId] = theImage;
}

Image* Sexy::GetImageById(int theId)
{
	return *(Image**)gResources[theId];
}

Font* Sexy::GetFontById(int theId)
{
	return *(Font**)gResources[theId];
}

int Sexy::GetSoundById(int theId)
{
	return *(int*)gResources[theId];
}

Image*& Sexy::GetImageRefById(int theId)
{
	return *(Image**)gResources[theId];
}

Font*& Sexy::GetFontRefById(int theId)
{
	return *(Font**)gResources[theId];
}

int& Sexy::GetSoundRefById(int theId)
{
	return *(int*)gResources[theId];
}

static Sexy::ResourceId GetIdByVariable(const void *theVariable)
{
	typedef std::map<int,int> MyMap;
	static MyMap aMap;
	if(gNeedRecalcVariableToIdMap)
	{
		gNeedRecalcVariableToIdMap = false;
		aMap.clear();
		for(int i=0; i<RESOURCE_ID_MAX; i++)
			aMap[*(int*)gResources[i]] = i;
	}

	MyMap::iterator anItr = aMap.find((int)theVariable);
	if (anItr == aMap.end())
		return RESOURCE_ID_MAX;
	else
		return (ResourceId) anItr->second;
}

Sexy::ResourceId Sexy::GetIdByImage(Image *theImage)
{
	return GetIdByVariable(theImage);
}

Sexy::ResourceId Sexy::GetIdByFont(Font *theFont)
{
	return GetIdByVariable(theFont);
}

Sexy::ResourceId Sexy::GetIdBySound(int theSound)
{
	return GetIdByVariable((void*)theSound);
}

const char* Sexy::GetStringIdById(int theId)
{
	switch(theId)
	{
		case FONT_DEFAULT_ID: return "FONT_DEFAULT";
		case FONT_BERLINSANS24_ID: return "FONT_BERLINSANS24";
		case FONT_BERLINSANS14_ID: return "FONT_BERLINSANS14";
		case IMAGE_CUSTOM_POINTER_ID: return "IMAGE_CUSTOM_POINTER";
		case IMAGE_CUSTOM_HAND_ID: return "IMAGE_CUSTOM_HAND";
		case IMAGE_CUSTOM_DRAGGING_ID: return "IMAGE_CUSTOM_DRAGGING";
		case IMAGE_CUSTOM_TEXT_ID: return "IMAGE_CUSTOM_TEXT";
		case IMAGE_TITLE_LOGO_ID: return "IMAGE_TITLE_LOGO";
		case IMAGE_COMPANY_LOGO_ID: return "IMAGE_COMPANY_LOGO";
		case IMAGE_BLOCKS_ID: return "IMAGE_BLOCKS";
		case IMAGE_LOADER_BAR_ID: return "IMAGE_LOADER_BAR";
		case IMAGE_LOADER_LOADINGTXT_ID: return "IMAGE_LOADER_LOADINGTXT";
		case IMAGE_BRANDING_ID: return "IMAGE_BRANDING";
		case SOUND_TIMER_ID: return "SOUND_TIMER";
		case IMAGE_YOU_LOSE_ID: return "IMAGE_YOU_LOSE";
		case IMAGE_PAK_COMPLETE_HOW_TO_PLAY_ID: return "IMAGE_PAK_COMPLETE_HOW_TO_PLAY";
		case IMAGE_PAK_COMPLETE_END_GAME_ID: return "IMAGE_PAK_COMPLETE_END_GAME";
		case IMAGE_PAK_COMPLETE_END_GAME_TEXT_ID: return "IMAGE_PAK_COMPLETE_END_GAME_TEXT";
		case IMAGE_PAK_COMPLETE_END_GAME_2_TEXT_ID: return "IMAGE_PAK_COMPLETE_END_GAME_2_TEXT";
		case IMAGE_PAK_COMPLETE_END_GAME_3_TEXT_ID: return "IMAGE_PAK_COMPLETE_END_GAME_3_TEXT";
		case IMAGE_PAK_TEXT_ID: return "IMAGE_PAK_TEXT";
		case IMAGE_BOARD_BG_ID: return "IMAGE_BOARD_BG";
		case IMAGE_SLIDER_BUTTON_ID: return "IMAGE_SLIDER_BUTTON";
		case IMAGE_NUMBERS_ID: return "IMAGE_NUMBERS";
		case IMAGE_BACKGROUND_LAYER_3_ID: return "IMAGE_BACKGROUND_LAYER_3";
		case IMAGE_CHECKBOX_ID: return "IMAGE_CHECKBOX";
		case IMAGE_DIALOG_BOX_ID: return "IMAGE_DIALOG_BOX";
		case IMAGE_DIALOG_LARGE_ID: return "IMAGE_DIALOG_LARGE";
		case IMAGE_DIALOG_SMALL_ID: return "IMAGE_DIALOG_SMALL";
		case IMAGE_DIALOG_BUTTON_ID: return "IMAGE_DIALOG_BUTTON";
		case IMAGE_DIALOG_BUTTON_SMALL_ID: return "IMAGE_DIALOG_BUTTON_SMALL";
		case IMAGE_SLIDER_TRACK_ID: return "IMAGE_SLIDER_TRACK";
		case IMAGE_SLIDER_THUMB_ID: return "IMAGE_SLIDER_THUMB";
		case IMAGE_ARROW_ID: return "IMAGE_ARROW";
		case IMAGE_lantern_ID: return "IMAGE_lantern";
		case IMAGE_spyglass_ID: return "IMAGE_spyglass";
		case IMAGE_watch_ID: return "IMAGE_watch";
		case IMAGE_anchor_ID: return "IMAGE_anchor";
		case IMAGE_map_ID: return "IMAGE_map";
		case IMAGE_pipe_ID: return "IMAGE_pipe";
		case IMAGE_jug_ID: return "IMAGE_jug";
		case IMAGE_wheel_ID: return "IMAGE_wheel";
		case IMAGE_journal_ID: return "IMAGE_journal";
		case IMAGE_compass_ID: return "IMAGE_compass";
		case IMAGE_navcompass_ID: return "IMAGE_navcompass";
		case IMAGE_key_ID: return "IMAGE_key";
		case IMAGE_unknown_ID: return "IMAGE_unknown";
		case IMAGE_lock_ID: return "IMAGE_lock";
		case IMAGE_anchor_sm_ID: return "IMAGE_anchor_sm";
		case IMAGE_pipe_sm_ID: return "IMAGE_pipe_sm";
		case IMAGE_wheel_sm_ID: return "IMAGE_wheel_sm";
		case IMAGE_map_sm_ID: return "IMAGE_map_sm";
		case IMAGE_journal_sm_ID: return "IMAGE_journal_sm";
		case IMAGE_compass_sm_ID: return "IMAGE_compass_sm";
		case IMAGE_lantern_sm_ID: return "IMAGE_lantern_sm";
		case IMAGE_key_sm_ID: return "IMAGE_key_sm";
		case IMAGE_watch_sm_ID: return "IMAGE_watch_sm";
		case IMAGE_spyglass_sm_ID: return "IMAGE_spyglass_sm";
		case IMAGE_navcompass_sm_ID: return "IMAGE_navcompass_sm";
		case IMAGE_jug_sm_ID: return "IMAGE_jug_sm";
		case IMAGE_small_lock_ID: return "IMAGE_small_lock";
		case IMAGE_smile_ID: return "IMAGE_smile";
		case IMAGE_lantern_large_ID: return "IMAGE_lantern_large";
		case IMAGE_spyglass_large_ID: return "IMAGE_spyglass_large";
		case IMAGE_watch_large_ID: return "IMAGE_watch_large";
		case IMAGE_anchor_large_ID: return "IMAGE_anchor_large";
		case IMAGE_map_large_ID: return "IMAGE_map_large";
		case IMAGE_pipe_large_ID: return "IMAGE_pipe_large";
		case IMAGE_jug_large_ID: return "IMAGE_jug_large";
		case IMAGE_wheel_large_ID: return "IMAGE_wheel_large";
		case IMAGE_journal_large_ID: return "IMAGE_journal_large";
		case IMAGE_compass_large_ID: return "IMAGE_compass_large";
		case IMAGE_navcompass_large_ID: return "IMAGE_navcompass_large";
		case IMAGE_key_large_ID: return "IMAGE_key_large";
		case IMAGE_unknown_large_ID: return "IMAGE_unknown_large";
		case IMAGE_gamepanel_ID: return "IMAGE_gamepanel";
		case IMAGE_ground_ID: return "IMAGE_ground";
		case IMAGE_walls_ID: return "IMAGE_walls";
		case IMAGE_actors_ID: return "IMAGE_actors";
		case SOUND_BUTTON_ID: return "SOUND_BUTTON";
		case SOUND_LEVEL_UP_ID: return "SOUND_LEVEL_UP";
		case SOUND_PAK_COMPLETE_ID: return "SOUND_PAK_COMPLETE";
		case SOUND_BLOX_DISAPPEAR_ID: return "SOUND_BLOX_DISAPPEAR";
		case SOUND_BLOX_HIT_ID: return "SOUND_BLOX_HIT";
		case SOUND_GAME_OVER_RESTART_ID: return "SOUND_GAME_OVER_RESTART";
		case SOUND_footstep1_ID: return "SOUND_footstep1";
		default: return "";
	}
}

