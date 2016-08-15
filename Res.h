#ifndef __res_H__
#define __res_H__

namespace Sexy
{
	class ResourceManager;
	class Image;
	class Font;

	Image* LoadImageById(ResourceManager *theManager, int theId);
	void ReplaceImageById(ResourceManager *theManager, int theId, Image *theImage);
	bool ExtractResourcesByName(ResourceManager *theManager, const char *theName);

	// Game Resources
	bool ExtractGameResources(ResourceManager *theMgr);
	extern Image* IMAGE_ARROW;
	extern Image* IMAGE_BACKGROUND_LAYER_3;
	extern Image* IMAGE_BOARD_BG;
	extern Image* IMAGE_CHECKBOX;
	extern Image* IMAGE_DIALOG_BOX;
	extern Image* IMAGE_DIALOG_BUTTON;
	extern Image* IMAGE_DIALOG_BUTTON_SMALL;
	extern Image* IMAGE_DIALOG_LARGE;
	extern Image* IMAGE_DIALOG_SMALL;
	extern Image* IMAGE_NUMBERS;
	extern Image* IMAGE_PAK_COMPLETE_END_GAME;
	extern Image* IMAGE_PAK_COMPLETE_END_GAME_2_TEXT;
	extern Image* IMAGE_PAK_COMPLETE_END_GAME_3_TEXT;
	extern Image* IMAGE_PAK_COMPLETE_END_GAME_TEXT;
	extern Image* IMAGE_PAK_COMPLETE_HOW_TO_PLAY;
	extern Image* IMAGE_PAK_TEXT;
	extern Image* IMAGE_SLIDER_BUTTON;
	extern Image* IMAGE_SLIDER_THUMB;
	extern Image* IMAGE_SLIDER_TRACK;
	extern Image* IMAGE_YOU_LOSE;
	extern Image* IMAGE_actors;
	extern Image* IMAGE_anchor;
	extern Image* IMAGE_anchor_large;
	extern Image* IMAGE_anchor_sm;
	extern Image* IMAGE_compass;
	extern Image* IMAGE_compass_large;
	extern Image* IMAGE_compass_sm;
	extern Image* IMAGE_gamepanel;
	extern Image* IMAGE_ground;
	extern Image* IMAGE_journal;
	extern Image* IMAGE_journal_large;
	extern Image* IMAGE_journal_sm;
	extern Image* IMAGE_jug;
	extern Image* IMAGE_jug_large;
	extern Image* IMAGE_jug_sm;
	extern Image* IMAGE_key;
	extern Image* IMAGE_key_large;
	extern Image* IMAGE_key_sm;
	extern Image* IMAGE_lantern;
	extern Image* IMAGE_lantern_large;
	extern Image* IMAGE_lantern_sm;
	extern Image* IMAGE_lock;
	extern Image* IMAGE_map;
	extern Image* IMAGE_map_large;
	extern Image* IMAGE_map_sm;
	extern Image* IMAGE_navcompass;
	extern Image* IMAGE_navcompass_large;
	extern Image* IMAGE_navcompass_sm;
	extern Image* IMAGE_pipe;
	extern Image* IMAGE_pipe_large;
	extern Image* IMAGE_pipe_sm;
	extern Image* IMAGE_small_lock;
	extern Image* IMAGE_smile;
	extern Image* IMAGE_spyglass;
	extern Image* IMAGE_spyglass_large;
	extern Image* IMAGE_spyglass_sm;
	extern Image* IMAGE_unknown;
	extern Image* IMAGE_unknown_large;
	extern Image* IMAGE_walls;
	extern Image* IMAGE_watch;
	extern Image* IMAGE_watch_large;
	extern Image* IMAGE_watch_sm;
	extern Image* IMAGE_wheel;
	extern Image* IMAGE_wheel_large;
	extern Image* IMAGE_wheel_sm;
	extern int SOUND_BLOX_DISAPPEAR;
	extern int SOUND_BLOX_HIT;
	extern int SOUND_BUTTON;
	extern int SOUND_GAME_OVER_RESTART;
	extern int SOUND_LEVEL_UP;
	extern int SOUND_PAK_COMPLETE;
	extern int SOUND_TIMER;
	extern int SOUND_footstep1;

	// GameScreen Resources
	bool ExtractGameScreenResources(ResourceManager *theMgr);
	extern Image* IMAGE_BRANDING;
	extern Image* IMAGE_LOADER_BAR;
	extern Image* IMAGE_LOADER_LOADINGTXT;

	// Init Resources
	bool ExtractInitResources(ResourceManager *theMgr);
	extern Font* FONT_BERLINSANS14;
	extern Font* FONT_BERLINSANS24;
	extern Font* FONT_DEFAULT;
	extern Image* IMAGE_BLOCKS;
	extern Image* IMAGE_COMPANY_LOGO;
	extern Image* IMAGE_CUSTOM_DRAGGING;
	extern Image* IMAGE_CUSTOM_HAND;
	extern Image* IMAGE_CUSTOM_POINTER;
	extern Image* IMAGE_CUSTOM_TEXT;
	extern Image* IMAGE_TITLE_LOGO;

	enum ResourceId
	{
		FONT_DEFAULT_ID,
		FONT_BERLINSANS24_ID,
		FONT_BERLINSANS14_ID,
		IMAGE_CUSTOM_POINTER_ID,
		IMAGE_CUSTOM_HAND_ID,
		IMAGE_CUSTOM_DRAGGING_ID,
		IMAGE_CUSTOM_TEXT_ID,
		IMAGE_TITLE_LOGO_ID,
		IMAGE_COMPANY_LOGO_ID,
		IMAGE_BLOCKS_ID,
		IMAGE_LOADER_BAR_ID,
		IMAGE_LOADER_LOADINGTXT_ID,
		IMAGE_BRANDING_ID,
		SOUND_TIMER_ID,
		IMAGE_YOU_LOSE_ID,
		IMAGE_PAK_COMPLETE_HOW_TO_PLAY_ID,
		IMAGE_PAK_COMPLETE_END_GAME_ID,
		IMAGE_PAK_COMPLETE_END_GAME_TEXT_ID,
		IMAGE_PAK_COMPLETE_END_GAME_2_TEXT_ID,
		IMAGE_PAK_COMPLETE_END_GAME_3_TEXT_ID,
		IMAGE_PAK_TEXT_ID,
		IMAGE_BOARD_BG_ID,
		IMAGE_SLIDER_BUTTON_ID,
		IMAGE_NUMBERS_ID,
		IMAGE_BACKGROUND_LAYER_3_ID,
		IMAGE_CHECKBOX_ID,
		IMAGE_DIALOG_BOX_ID,
		IMAGE_DIALOG_LARGE_ID,
		IMAGE_DIALOG_SMALL_ID,
		IMAGE_DIALOG_BUTTON_ID,
		IMAGE_DIALOG_BUTTON_SMALL_ID,
		IMAGE_SLIDER_TRACK_ID,
		IMAGE_SLIDER_THUMB_ID,
		IMAGE_ARROW_ID,
		IMAGE_lantern_ID,
		IMAGE_spyglass_ID,
		IMAGE_watch_ID,
		IMAGE_anchor_ID,
		IMAGE_map_ID,
		IMAGE_pipe_ID,
		IMAGE_jug_ID,
		IMAGE_wheel_ID,
		IMAGE_journal_ID,
		IMAGE_compass_ID,
		IMAGE_navcompass_ID,
		IMAGE_key_ID,
		IMAGE_unknown_ID,
		IMAGE_lock_ID,
		IMAGE_anchor_sm_ID,
		IMAGE_pipe_sm_ID,
		IMAGE_wheel_sm_ID,
		IMAGE_map_sm_ID,
		IMAGE_journal_sm_ID,
		IMAGE_compass_sm_ID,
		IMAGE_lantern_sm_ID,
		IMAGE_key_sm_ID,
		IMAGE_watch_sm_ID,
		IMAGE_spyglass_sm_ID,
		IMAGE_navcompass_sm_ID,
		IMAGE_jug_sm_ID,
		IMAGE_small_lock_ID,
		IMAGE_smile_ID,
		IMAGE_lantern_large_ID,
		IMAGE_spyglass_large_ID,
		IMAGE_watch_large_ID,
		IMAGE_anchor_large_ID,
		IMAGE_map_large_ID,
		IMAGE_pipe_large_ID,
		IMAGE_jug_large_ID,
		IMAGE_wheel_large_ID,
		IMAGE_journal_large_ID,
		IMAGE_compass_large_ID,
		IMAGE_navcompass_large_ID,
		IMAGE_key_large_ID,
		IMAGE_unknown_large_ID,
		IMAGE_gamepanel_ID,
		IMAGE_ground_ID,
		IMAGE_walls_ID,
		IMAGE_actors_ID,
		SOUND_BUTTON_ID,
		SOUND_LEVEL_UP_ID,
		SOUND_PAK_COMPLETE_ID,
		SOUND_BLOX_DISAPPEAR_ID,
		SOUND_BLOX_HIT_ID,
		SOUND_GAME_OVER_RESTART_ID,
		SOUND_footstep1_ID,
		RESOURCE_ID_MAX
	};

	Image* GetImageById(int theId);
	Font* GetFontById(int theId);
	int GetSoundById(int theId);

	Image*& GetImageRefById(int theId);
	Font*& GetFontRefById(int theId);
	int& GetSoundRefById(int theId);

	ResourceId GetIdByImage(Image *theImage);
	ResourceId GetIdByFont(Font *theFont);
	ResourceId GetIdBySound(int theSound);
	const char* GetStringIdById(int theId);
	ResourceId GetIdByStringId(const char *theStringId);

} // namespace Sexy


#endif
