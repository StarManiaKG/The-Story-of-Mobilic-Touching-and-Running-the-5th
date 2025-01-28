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

#endif // __APK_MAIN__
