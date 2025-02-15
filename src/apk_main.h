// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 2024-2025 by StarManiaKG.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  apk_main.c
/// \brief Android Operating System

#ifndef __APK_MAIN__
#define __APK_MAIN__

#include "doomdef.h"
#include "command.h"
#include "g_input.h"
#include "st_stuff.h"
#include "w_wad.h"

#ifdef SPLASH_SCREEN
#include <SDL.h>
#include "sdl/sdlmain.h"
#endif

#define APK_ST_WEAPONS_X ((BASEVIDWIDTH / 2) - (NUM_WEAPONS * 10) - 6)
#define APK_ST_WEAPONS_Y 176 // HUD_LIVES
#define APK_ST_WEAPONS_W 20
#define APK_ST_WEAPONS_H 20

extern struct android_data_s
{
	INT32 cam1_toggledelay;
	INT32 cam2_toggledelay;
	INT32 demo_inputdrawn;
	boolean gamedata_inpath;
	boolean prompt_blockcontrols;
	fixed_t prompt_hidehudbound;
} android_data;

#ifdef SPLASH_SCREEN
struct SDLSplashScreen
{
	SDL_bool displaying;
	UINT32   *image;
};
#endif

extern consvar_t cv_android_liveshudpos; // lives HUD position
extern consvar_t cv_android_thinkless;

extern joystickvector2_t android_joystickmovevectors[2];
extern joystickvector2_t android_joysticklookvectors[2];
#ifdef TOUCHINPUTS
extern joystickvector2_t android_touchmovevector;
#endif
#ifdef ACCELEROMETER
extern joystickvector2_t android_accelmovevector;
#endif

// Returns the longest PossibleValue string for this CVar
const char *APK_CV_LongestPossibleValue(consvar_t *var);

size_t APK_G_ReadSaveGameSlot(char *savename, UINT8 **buffer, UINT32 slot);
char *APK_G_LiveEventHasBackup(void);

boolean APK_G_CanViewpointSwitchToPlayer(player_t *player);
boolean APK_G_CanViewpointSwitch(boolean luahook);

boolean APK_G_ToggleChaseCam(UINT8 player, boolean set_chasecam);

void APK_P_MainTicker(boolean run);
boolean APK_P_ReduceMobjThinking(mobj_t *mobj);

boolean APK_ST_UseAltLivesHUD(void);
hudinfo_t *APK_ST_GetLivesHUDInfo(void);
boolean APK_ST_AltLivesHUDEnabled(void);

char *APK_M_FindFile(const char *filename);

// Loads a wadfile, but doesn't add it to the active wad files.
wadfile_t *APK_W_LoadResourceFile(const char *filename, fhandletype_t handletype);
// Deletes a wadfile.
void APK_W_DeleteResourceFile(wadfile_t *wad);

UINT16 APK_Resource_CheckNumForName(wadfile_t *wad, const char *name);
void *APK_Resource_CacheLumpNum(wadfile_t *wad, UINT16 lump, INT32 tag);
void *APK_Resource_CacheLumpName(wadfile_t *wad, const char *name, INT32 tag);
boolean APK_Resource_LumpExists(wadfile_t *wad, const char *name);
size_t APK_Resource_LumpLength(wadfile_t *wad, UINT16 lump);
size_t APK_Resource_ReadLumpHeader(wadfile_t *wad, UINT16 lump, void *dest, size_t size, size_t offset);

/**	\brief Show the splash screen
*/
void APK_I_ShowSplashScreen(void);

/**	\brief Hide the splash screen
*/
void APK_I_HideSplashScreen(void);

#endif // __APK_MAIN__
