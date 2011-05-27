//ROBO: Much of this file stolen shamelessly from FCEUX

#include <mednafen_includes.h>

extern "C"
{
	#include <src/lua/lua.h>
	#include <src/lua/lauxlib.h>
	#include <src/lua/lualib.h>
}

//Pass true to toggle layer at given index
//TODO: Make it possible to SET layer state, but mednafen only supports toggle with it's interface
int		emu_setrenderplanes(lua_State *L)
{
	const MDFNGI* GameInfo = MednafenEmu::GetGameInfo();
	uint32_t count = lua_gettop(L);

	if(GameInfo && GameInfo->ToggleLayer && GameInfo->LayerNames && count)
	{
		const char* layerTag = GameInfo->LayerNames;

		for(int i = 0; i != count && strlen(layerTag); i ++)
		{
			if(lua_toboolean(L, i + 1))
			{
				MDFNI_ToggleLayer(i);
			}

			layerTag += strlen(layerTag) + 1;
		}
	}

	return 0;		
}

// emu.speedmode(string mode)
//
//   Takes control of the emulation speed
//   of the system. Normal is normal speed (60fps, 50 for PAL),
//   nothrottle disables speed control but renders every frame,
//   turbo renders only a few frames in order to speed up emulation,
//   maximum renders no frames
//   TODO: better enforcement, done in the same way as basicbot...
/*int			emu_speedmode(lua_State *L)
{
	const char *mode = luaL_checkstring(L, 1);
	
	if (strcasecmp(mode, "normal")==0) {
		speedmode = SPEED_NORMAL;
	} else if (strcasecmp(mode, "nothrottle")==0) {
		speedmode = SPEED_NOTHROTTLE;
	} else if (strcasecmp(mode, "turbo")==0) {
		speedmode = SPEED_TURBO;
	} else if (strcasecmp(mode, "maximum")==0) {
		speedmode = SPEED_MAXIMUM;
	} else
		luaL_error(L, "Invalid mode %s to emu.speedmode",mode);
	
	//printf("new speed mode:  %d\n", speedmode);
        if (speedmode == SPEED_NORMAL) 
		{
			FCEUD_SetEmulationSpeed(EMUSPEED_NORMAL);
			FCEUD_TurboOff();
		}
        else if (speedmode == SPEED_TURBO)				//adelikat: Making turbo actually use turbo.
			FCEUD_TurboOn();							//Turbo and max speed are two different results. Turbo employs frame skipping and sound bypassing if mute turbo option is enabled.
												//This makes it faster but with frame skipping. Therefore, maximum is still a useful feature, in case the user is recording an avi or making screenshots (or something else that needs all frames)
		else											
			FCEUD_SetEmulationSpeed(EMUSPEED_FASTEST);  //TODO: Make nothrottle turn off throttle, or remove the option
	return 0;
}*/

// emu.poweron()
//
// Executes a power cycle
int emu_poweron(lua_State *L) {
	const MDFNGI* GameInfo = MednafenEmu::GetGameInfo();
	if (GameInfo)
		MDFNI_Power();
	
	return 0;
}

// emu.softreset()
//
// Executes a power cycle
int emu_softreset(lua_State *L) {
	const MDFNGI* GameInfo = MednafenEmu::GetGameInfo();
	if (GameInfo)
		MDFNI_Reset();
	
	return 0;
}

/*
// emu.frameadvance()
//
//  Executes a frame advance. Occurs by yielding the coroutine, then re-running
//  when we break out.
int emu_frameadvance(lua_State *L) {
	// We're going to sleep for a frame-advance. Take notes.

	if (frameAdvanceWaiting) 
		return luaL_error(L, "can't call emu.frameadvance() from here");

	frameAdvanceWaiting = TRUE;

	// Now we can yield to the main 
	return lua_yield(L, 0);


	// It's actually rather disappointing...
}*/

// emu.pause()
//
//  Pauses the emulator, function "waits" until the user unpauses.
//  This function MAY be called from a non-frame boundary, but the frame
//  finishes executing anwyays. In this case, the function returns immediately.
int emu_pause(lua_State *L) {
	MednafenEmu::SetPause(true);
	return 0;
	
	// Return control if we're midway through a frame. We can't pause here.
/*	speedmode = SPEED_NORMAL;

	if (frameAdvanceWaiting) {
		return 0;
	}

	// If it's on a frame boundary, we also yield.	
	frameAdvanceWaiting = TRUE;
	return lua_yield(L, 0);*/
	
}

//emu.unpause()
//
//adelikat:  Why wasn't this added sooner?
//Gives the user a way to unpause the emulator via lua
int emu_unpause(lua_State *L) {
	MednafenEmu::SetPause(false);
	return 0;

/*	speedmode = SPEED_NORMAL;

	// Return control if we're midway through a frame. We can't pause here.
	if (frameAdvanceWaiting) {
		return 0;
	}

	// If it's on a frame boundary, we also yield.	
	frameAdvanceWaiting = TRUE;
	return lua_yield(L, 0);*/
}

// emu.message(string msg)
//
//  Displays the given message on the screen.
int emu_message(lua_State *L) {

	const char *msg = luaL_checkstring(L,1);
	MDFND_DispMessage((UTF8*)msg);
	
	return 0;

}

