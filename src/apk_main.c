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

#include "apk_main.h"
#include "ts_draw.h"

#include "doomstat.h"
#include "g_game.h"
#include "lua_hook.h"
#include "m_misc.h"
#include "r_main.h"
#include "z_zone.h"

static CV_PossibleValue_t liveshudpos_cons_t[] = {{0, "Bottom left"}, {1, "Top right"}, {2, "Automatic"}, {0, NULL}};
consvar_t cv_android_liveshudpos = CVAR_INIT ("liveshudpos", "Automatic", CV_SAVE, liveshudpos_cons_t, NULL);

consvar_t cv_android_thinkless = CVAR_INIT(
	"thinkless",
#ifdef MOBILE_PLATFORM
	"On",
#else
	"Off",
#endif
	CV_SAVE, CV_OnOff,
	NULL
);

joystickvector2_t android_joystickmovevectors[2];
joystickvector2_t android_joysticklookvectors[2];
#ifdef TOUCHINPUTS
joystickvector2_t android_touchmovevector;
#endif
#ifdef ACCELEROMETER
joystickvector2_t android_accelmovevector;
#endif

struct android_data_s android_data; // Misc. Android Stuff

//
// GAME CODE
//

static void GetSaveGameName(char *savename, UINT32 slot)
{
	if (marathonmode)
		strlcpy(savename, curliveeventbackup, SAVEGAMENAMELEN);
	else
		snprintf(savename, SAVEGAMENAMELEN, cursavegamename, slot);
}

size_t APK_G_ReadSaveGameSlot(char *savename, UINT8 **buffer, UINT32 slot)
{
	size_t length = 0;

	cursavegamename = savegamename[0];
	curliveeventbackup = liveeventbackup[0];

	GetSaveGameName(savename, slot);
	length = FIL_ReadFile(savename, buffer);

#ifdef USE_SAVEGAME_PATHS
	if (!length)
	{
		cursavegamename = savegamename[1];
		curliveeventbackup = liveeventbackup[1];

		GetSaveGameName(savename, slot);
		length = FIL_ReadFile(savename, buffer);
	}
#endif

	return length;
}

char *APK_G_LiveEventHasBackup(void)
{
	if (FIL_FileExists(liveeventbackup[0]))
		return liveeventbackup[0];
#ifdef USE_SAVEGAME_PATHS
	if (FIL_FileExists(liveeventbackup[1]))
		return liveeventbackup[1];
#endif
	return NULL;
}


// Returns true if you can switch your viewpoint to this player.
boolean APK_G_CanViewpointSwitchToPlayer(player_t *player)
{
	player_t *myself = &players[consoleplayer];

	if (player->spectator)
		return false;

	if (G_GametypeHasTeams())
	{
		if (myself->ctfteam && player->ctfteam != myself->ctfteam)
			return false;
	}
	else if (gametyperules & GTR_HIDEFROZEN)
	{
		if (myself->pflags & PF_TAGIT)
			return false;
	}
	// Other Tag-based gametypes?
	else if (G_TagGametype())
	{
		if (!myself->spectator && (myself->pflags & PF_TAGIT) != (player->pflags & PF_TAGIT))
			return false;
	}
	else if (G_GametypeHasSpectators() && G_RingSlingerGametype())
	{
		if (!myself->spectator)
			return false;
	}

	return true;
}

// Returns true if you can switch your viewpoint at all.
boolean APK_G_CanViewpointSwitch(boolean luahook)
{
	// ViewpointSwitch Lua hook.
	UINT8 canSwitchView = 0;
	INT32 checkdisplayplayer = displayplayer;

	if (splitscreen || !netgame)
		return false;

	if (D_NumPlayers() <= 1)
		return false;

	do
	{
		checkdisplayplayer++;
		if (checkdisplayplayer == MAXPLAYERS)
			checkdisplayplayer = 0;

		if (!playeringame[checkdisplayplayer])
			continue;

		// Call ViewpointSwitch hooks here.
		if (luahook)
		{
			canSwitchView = LUA_HookViewpointSwitch(&players[consoleplayer], &players[checkdisplayplayer], false);
			if (canSwitchView == 1) // Set viewpoint to this player
				break;
			else if (canSwitchView == 2) // Skip this player
				continue;
		}

		if (!APK_G_CanViewpointSwitchToPlayer(&players[checkdisplayplayer]))
			continue;

		break;
	} while (checkdisplayplayer != consoleplayer);

	// had any change??
	return (checkdisplayplayer != displayplayer);
}

void APK_P_MainTicker(boolean run)
{
	if (run)
	{
		android_data.cam1_toggledelay--;
		android_data.cam2_toggledelay--;
	}
}

// Handles the camera toggle key being pressed.
boolean APK_G_ToggleChaseCam(UINT8 player, boolean set_chasecam)
{
	if (!android_data.cam1_toggledelay && !player)
	{
		// Player 1
		android_data.cam1_toggledelay = NEWTICRATE / 7;
		if (set_chasecam)
			CV_SetValue(&cv_chasecam, cv_chasecam.value ? 0 : 1);
		return true;
	}
	if (!android_data.cam2_toggledelay && player)
	{
		// Player 2
		android_data.cam2_toggledelay = NEWTICRATE / 7;
		if (set_chasecam)
			CV_SetValue(&cv_chasecam2, cv_chasecam2.value ? 0 : 1);
		return true;
	}
	return false;
}

//
// OBJECT CODE
//

static inline boolean P_MobjDistanceCheck(mobj_t *mobj)
{
	fixed_t tx, ty, cx, cy;
	const fixed_t blocksize = 1024*FRACUNIT;
	const fixed_t range = 4;
	tx = mobj->x / blocksize;
	ty = mobj->y / blocksize;
	cx = viewx / blocksize;
	cy = viewy / blocksize;

	if (abs(tx-cx) > range || abs(ty-cy) > range)
		return false;
	return true;
}

boolean APK_P_ReduceMobjThinking(mobj_t *mobj)
{
	if (mobj->player)
		return false;

	if (!(cv_android_thinkless.value
		&& !(netgame || multiplayer)
		&& !(demoplayback || modeattacking || marathonmode || metalrecording))
	)
	{
		// No reduced thinking!
		return false;
	}

	if (!P_MobjDistanceCheck(mobj))
	{
		switch (mobj->type)
		{
			case MT_MACEPOINT:
			case MT_CHAINMACEPOINT:
			case MT_SPRINGBALLPOINT:
			case MT_CHAINPOINT:
			case MT_FIREBARPOINT:
			case MT_CUSTOMMACEPOINT:
			case MT_HIDDEN_SLING:
				// Always update movedir to prevent desyncing (in the traditional sense, not the netplay sense).
				mobj->movedir = (mobj->movedir + mobj->lastlook) & FINEMASK;
				/* FALLTHRU */
			default:
				return true;
		}
	}

	return false;
}

//
// STATUS BAR CODE
//

boolean APK_ST_UseAltLivesHUD(void)
{
#ifdef TOUCHINPUTS
	if (cv_android_liveshudpos.value == 2)
		return TS_CanDrawButtons();
#endif
	return (cv_android_liveshudpos.value == 1);
}

hudinfo_t *APK_ST_GetLivesHUDInfo(void)
{
	if (APK_ST_UseAltLivesHUD())
		return &hudinfo[ANDROID_HUD_LIVES];
	return &hudinfo[HUD_LIVES];
}

boolean APK_ST_AltLivesHUDEnabled(void)
{
	return (APK_ST_UseAltLivesHUD() && !modeattacking);
}
