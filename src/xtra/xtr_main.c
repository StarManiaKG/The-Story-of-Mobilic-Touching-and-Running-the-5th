
#include "xtr_main.h"

angle_t XTRA_R_GetLocalViewRollAngle(player_t *player)
{
	angle_t ang = player->viewrollangle;
#if defined(ACCELEROMETER) && defined(ACCELEROMETER_TILT_VIEW)
	if (cv_useaccelerometer.value && gamestate == GS_LEVEL && player == &players[consoleplayer] && !splitscreen)
	{
		fixed_t accelangle = FixedDiv(acceltilt * FRACUNIT, 4096<<FRACBITS);
		ang += FixedAngle(FixedMul(accelangle, 90<<FRACBITS));
	}
#endif
	return ang;
}
