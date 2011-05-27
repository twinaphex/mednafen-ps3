//ROBO: Much of this file stolen shamelessly from FCEUX

#include <mednafen_includes.h>

extern "C"
{
	#include <src/lua/lua.h>
	#include <src/lua/lauxlib.h>
	#include <src/lua/lualib.h>
}

namespace
{
	const char*					MD5ToString							(const uint8_t aBinary[16])
	{
		static char result[16 * 2 + 2];
		result[0] = 0;

		for(int i = 0; i != 16; i ++)
		{
			char buffer[4];
			snprintf(buffer, 4, "%02X", aBinary[i]);
			strcat(result, buffer);
		}

		return result;
	}

	uint32_t					TryGetColor							(lua_State *L, int aOffset, uint32_t aDefault)
	{
		if(lua_type(L, aOffset) == LUA_TNUMBER)
		{
			return lua_tointeger(L, aOffset);
		}

		return aDefault;
	}
}

/* EMU LIB */
int								emu_poweron							(lua_State *L)
{
	if(MednafenEmu::GetGameInfo())
	{
		MDFNI_Power();
	}

	return 0;
}

int								emu_softreset						(lua_State *L)
{
	if(MednafenEmu::GetGameInfo())
	{
		MDFNI_Reset();
	}

	return 0;
}
//SPEEDMODE

int								emu_frameadvance					(lua_State *L)
{
	// We're going to sleep for a frame-advance. Take notes.

//	if (frameAdvanceWaiting) 
//		return luaL_error(L, "can't call emu.frameadvance() from here");

//	frameAdvanceWaiting = TRUE;

	return lua_yield(L, 0);
}


int								emu_pause							(lua_State *L)
{
	MednafenEmu::SetPause(true);
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

int								emu_unpause							(lua_State *L)
{
	MednafenEmu::SetPause(false);
	return 0;

	//Ditto commented code from emu_pause
}

//Pass true to toggle layer at given index
//TODO: Make it possible to SET layer state, but mednafen only supports toggle with it's interface
int								emu_setrenderplanes					(lua_State *L)
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

int								emu_message							(lua_State *L)
{
	const char *msg = luaL_checkstring(L,1);
	MDFND_DispMessage((UTF8*)msg);
	return 0;
}


int								emu_framecount						(lua_State *L)
{
//TODO: What is the actual properties of FCEUMOV_GetFrame, does it work when not playing or recording?
	lua_pushinteger(L, MednafenEmu::GetFrameCount());
//	lua_pushinteger(L, FCEUMOV_GetFrame());
	return 1;
}

/*
int								emu_lagcount						(lua_State *L)
{
	lua_pushinteger(L, FCEUI_GetLagCount());
	return 1;
}

int								emu_lagged							(lua_State *L)
{
	bool Lag_Frame = FCEUI_GetLagged();
	lua_pushboolean(L, Lag_Frame);
	return 1;
}*/

int								emu_emulating						(lua_State *L)
{
	lua_pushboolean(L, 0 != MednafenEmu::GetGameInfo());
	return 1;
}

/*
int								emu_registerbefore					(lua_State *L)
{
	if (!lua_isnil(L,1))
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
	}

	lua_settop(L,1);
	lua_getfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFOREEMULATION]);
	lua_insert(L,1);
	lua_setfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFOREEMULATION]);
	//StopScriptIfFinished(luaStateToUIDMap[L]);
	return 1;
}

int								emu_registerafter					(lua_State *L)
{
	if (!lua_isnil(L,1))
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
	}

	lua_settop(L,1);
	lua_getfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_AFTEREMULATION]);
	lua_insert(L,1);
	lua_setfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_AFTEREMULATION]);
	//StopScriptIfFinished(luaStateToUIDMap[L]);
	return 1;
}

int								emu_registerexit					(lua_State *L)
{
	if (!lua_isnil(L,1))
	{
		luaL_checktype(L, 1, LUA_TFUNCTION);
	}
	lua_settop(L,1);
	lua_getfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFOREEXIT]);
	lua_insert(L,1);
	lua_setfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFOREEXIT]);
	//StopScriptIfFinished(luaStateToUIDMap[L]);
	return 1;
}

int								emu_addgamegenie					(lua_State *L)
{
	
	const char *msg = luaL_checkstring(L,1);

	// Add a Game Genie code if it hasn't already been added
	int GGaddr, GGcomp, GGval;
	int i=0;

	uint32 Caddr;
	uint8 Cval;
	int Ccompare, Ctype;
	
	if (!FCEUI_DecodeGG(msg, &GGaddr, &GGval, &GGcomp)) {
		luaL_error(L, "Failed to decode game genie code");
		lua_pushboolean(L, false);
		return 1;
	}

	while (FCEUI_GetCheat(i,NULL,&Caddr,&Cval,&Ccompare,NULL,&Ctype)) {

		if ((GGaddr == Caddr) && (GGval == Cval) && (GGcomp == Ccompare) && (Ctype == 1)) {
			// Already Added, so consider it a success
			lua_pushboolean(L, true);
			return 1;
		}

		i = i + 1;
	}
	
	if (FCEUI_AddCheat(msg,GGaddr,GGval,GGcomp,1)) {
		// Code was added
		// Can't manage the display update the way I want, so I won't bother with it
		// UpdateCheatsAdded();
		lua_pushboolean(L, true);
		return 1;
	} else {
		// Code didn't get added
		lua_pushboolean(L, false);
		return 1;
	}
}

int								emu_delgamegenie					(lua_State *L)
{
	const char *msg = luaL_checkstring(L,1);

	// Remove a Game Genie code. Very restrictive about deleted code.
	int GGaddr, GGcomp, GGval;
	uint32 i=0;

	char * Cname;
	uint32 Caddr;
	uint8 Cval;
	int Ccompare, Ctype;
	
	if (!FCEUI_DecodeGG(msg, &GGaddr, &GGval, &GGcomp)) {
		luaL_error(L, "Failed to decode game genie code");
		lua_pushboolean(L, false);
		return 1;
	}

	while (FCEUI_GetCheat(i,&Cname,&Caddr,&Cval,&Ccompare,NULL,&Ctype)) {

		if ((!strcmp(msg,Cname)) && (GGaddr == Caddr) && (GGval == Cval) && (GGcomp == Ccompare) && (Ctype == 1)) {
			// Delete cheat code
			if (FCEUI_DelCheat(i)) {
				lua_pushboolean(L, true);
				return 1;
			}
			else {
				lua_pushboolean(L, false);
				return 1;
			}
		}

		i = i + 1;
	}

	// Cheat didn't exist, so it's not an error
	lua_pushboolean(L, true);
	return 1;
}*/

int								emu_getscreenpixel					(lua_State *L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
//TODO: What does getemuscreen do?
//TODO: Support palette? Probably neccisary
//	bool getemuscreen = (lua_toboolean(L,3) == 1);

	int r, g, b;
//	int palette;

	if(!MednafenEmu::GetGameInfo())
	{
		lua_pushinteger(L, 0);
		lua_pushinteger(L, 0);
		lua_pushinteger(L, 0);
		lua_pushinteger(L, 254);
		return 4;
	}
	else
	{
//TODO: Colors locations are not right!
		uint32_t color = MednafenEmu::GetPixel(x, y);
		lua_pushinteger(L, (color >> 16) & 0xFF);
		lua_pushinteger(L, (color >> 8) & 0xFF);
		lua_pushinteger(L, (color >> 0) & 0xFF);
		lua_pushinteger(L, 0); //TODO: Palette
		return 4;
	}
}

/*
int								movie_getreadonly					(lua_State *L)
{
	lua_pushboolean(L, FCEUI_GetMovieToggleReadOnly());

	return 1;
}

//movie.setreadonly
//
//Sets readonly / read+write status
int								movie_setreadonly					(lua_State *L)
{
	bool which = (lua_toboolean( L, 1 ) == 1);
	FCEUI_SetMovieToggleReadOnly(which);
	
	return 0;
}

// replacement for luaB_print() that goes to the appropriate textbox instead of stdout
int								print								(lua_State *L)
{
	const char* str = toCString(L);

	int uid = info_uid;//luaStateToUIDMap[L->l_G->mainthread];
	//LuaContextInfo& info = GetCurrentInfo();

	if(info_print)
		info_print(uid, str);
	else
		puts(str);

	//worry(L, 100);
	return 0;
}
*/

luaL_reg emulib[] =
{
	{"poweron", emu_poweron},
	{"softreset", emu_softreset},
//	{"speedmode", emu_speedmode},
	{"frameadvance", emu_frameadvance},
	{"pause", emu_pause},
	{"unpause", emu_unpause},
//	{"exec_count", emu_exec_count},
//	{"exec_time", emu_exec_time},
	{"setrenderplanes", emu_setrenderplanes},
	{"message", emu_message},
	{"framecount", emu_framecount},
//	{"lagcount", emu_lagcount},
//	{"lagged", emu_lagged},
	{"emulating", emu_emulating},
//	{"registerbefore", emu_registerbefore},
//	{"registerafter", emu_registerafter},
//	{"registerexit", emu_registerexit},
//	{"addgamegenie", emu_addgamegenie},
//	{"delgamegenie", emu_delgamegenie},
	{"getscreenpixel", emu_getscreenpixel},
//	{"readonly", movie_getreadonly},
//	{"setreadonly", movie_setreadonly},
//	{"print", print}, // sure, why not
	{NULL,NULL}
};

/* ROM LIBRARY */
int								rom_readbyte						(lua_State *L)
{
	lua_pushinteger(L, MednafenEmu::ReadROM(luaL_checkinteger(L,1)));
	return 1;
}

int								rom_readbytesigned					(lua_State *L)
{
	lua_pushinteger(L, (int8_t)MednafenEmu::ReadROM(luaL_checkinteger(L,1)));
	return 1;
}

int								rom_gethash							(lua_State *L)
{
	if(MednafenEmu::GetGameInfo())
	{
		const char *type = luaL_checkstring(L, 1);
		if(!strcasecmp(type, "md5"))
		{
			lua_pushstring(L, MD5ToString(MednafenEmu::GetGameInfo()->MD5));
		}
		else
		{
			lua_pushstring(L, "");
		}

		return 1;
	}
	else
	{
		return 0;
	}
}

luaL_reg romlib[] =
{
	{"readbyte", rom_readbyte},
	{"readbytesigned", rom_readbytesigned},
	// alternate naming scheme for unsigned
	{"readbyteunsigned", rom_readbyte},

	{"gethash", rom_gethash},
	{NULL,NULL}
};

/* MEMORY FUNCTIONS */
int								memory_readbyte						(lua_State *L)
{
	const MDFNGI* info = MednafenEmu::GetGameInfo();
	if(info && info->Peek)
	{
		lua_pushinteger(L, info->Peek(luaL_checkinteger(L, 1)));
	}
	else
	{
		lua_pushinteger(L, 0);
	}

	return 1;
}

int								memory_readbytesigned				(lua_State *L)
{
	const MDFNGI* info = MednafenEmu::GetGameInfo();
	if(info && info->Peek)
	{
		int32_t value = (int8_t)info->Peek(luaL_checkinteger(L, 1));
		lua_pushinteger(L, value);
	}
	else
	{
		lua_pushinteger(L, 0);
	}

	return 1;
}

int								memory_writebyte					(lua_State *L)
{
	const MDFNGI* info = MednafenEmu::GetGameInfo();
	if(info && info->Poke)
	{
		info->Poke(luaL_checkinteger(L,1), luaL_checkinteger(L,2));
	}

	return 0;
}

int								memory_readbyterange				(lua_State *L)
{
	return 0;
/*	int range_start = luaL_checkinteger(L,1);
	int range_size = luaL_checkinteger(L,2);
	if(range_size < 0)
		return 0;

	char* buf = (char*)alloca(range_size);
	for(int i=0;i<range_size;i++) {
		buf[i] = FCEU_CheatGetByte(range_start+i);
	}

	lua_pushlstring(L,buf,range_size);
	
	return 1;*/
}

luaL_reg memorylib [] = {

	{"readbyte", memory_readbyte},
//	{"readbyterange", memory_readbyterange},
	{"readbytesigned", memory_readbytesigned},
	// alternate naming scheme for unsigned
	{"readbyteunsigned", memory_readbyte},
	{"writebyte", memory_writebyte},
//	{"getregister", memory_getregister},
//	{"setregister", memory_setregister},
	
	// memory hooks
//	{"registerwrite", memory_registerwrite},
	//{"registerread", memory_registerread}, TODO
//	{"registerexec", memory_registerexec},
	// alternate names
//	{"register", memory_registerwrite},
//	{"registerrun", memory_registerexec},
//	{"registerexecute", memory_registerexec},

	{NULL,NULL}
};


/* JOYPAD LIB */
static int						joy_get_internal					(lua_State *L, bool reportUp, bool reportDown)
{
	const InputHandler* inputs = MednafenEmu::GetInputs();

	if(inputs)
	{
		int which = luaL_checkinteger(L,1);
	
		if (which < 1 || which > inputs->GetPadCount() + 1)
		{
			luaL_error(L,"Invalid input port (valid range 1-4, specified %d)", which);
		}

		which --;
	
		lua_newtable(L);
	
		for(int i = 0; i < inputs->GetButtonCount(which); i++)
		{
			bool pressed = inputs->GetButtonState(which, i);

			if((pressed && reportDown) || (!pressed && reportUp))
			{
				lua_pushboolean(L, pressed ? 1 : 0);
				lua_setfield(L, -2, inputs->GetButtonName(which, i));
			}
		}
	
		return 1;
	}
//TODO: Handle this how ?
	else
	{
		lua_newtable(L);
		return 1;
	}
}

int								joypad_get							(lua_State *L)
{
	return joy_get_internal(L, true, true);
}

int								joypad_getdown						(lua_State *L)
{
	return joy_get_internal(L, false, true);
}

int								joypad_getup						(lua_State *L)
{
	return joy_get_internal(L, true, false);
}

/*static int joypad_set(lua_State *L) {

	// Which joypad we're tampering with
	int which = luaL_checkinteger(L,1);
	if (which < 1 || which > 4) {
		luaL_error(L,"Invalid output port (valid range 1-4, specified %d)", which);

	}

	// And the table of buttons.
	luaL_checktype(L,2,LUA_TTABLE);

	// Set up for taking control of the indicated controller
	luajoypads1[which-1] = 0xFF; // .1  Reset right bit
	luajoypads2[which-1] = 0x00; // 0.  Reset left bit

	int i;
	for (i=0; i < 8; i++) {
		lua_getfield(L, 2, button_mappings[i]);
		
		//Button is not nil, so find out if it is true/false
		if (!lua_isnil(L,-1))	
		{
			if (lua_toboolean(L,-1))							//True or string
				luajoypads2[which-1] |= 1 << i;
			if (lua_toboolean(L,-1) == 0 || lua_isstring(L,-1))	//False or string
				luajoypads1[which-1] &= ~(1 << i);
		}

		else
		{

		}
		lua_pop(L,1);
	}
	
	return 0;
}*/

luaL_reg joypadlib[] =
{
	{"get", joypad_get},
	{"getdown", joypad_getdown},
	{"getup", joypad_getup},
//	{"set", joypad_set},
	// alternative names
	{"read", joypad_get},
//	{"write", joypad_set},
	{"readdown", joypad_getdown},
	{"readup", joypad_getup},
	{NULL,NULL}
};

/* GUI LIB */
uint32_t gui_array[1024 * 768];

// gui.pixel(x,y,colour)
int								gui_pixel							(lua_State *L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	uint32 colour = luaL_checkinteger(L, 3);  //?

	//TODO: CHECK BOUNDS

	gui_array[y * 1024 + x] = colour;

	return 0;
}

int								gui_getpixel						(lua_State *L)
{
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);

	int r, g, b, a;

	//TODO: CHECK BOUNDS
	//TODO: Colors aren't correct
	uint32_t colour = gui_array[y * 1024 + x];
	lua_pushinteger(L, colour & 0xFF);
	lua_pushinteger(L, (colour >> 8) & 0xFF);
	lua_pushinteger(L, (colour >> 16) & 0xFF);
	lua_pushinteger(L, (colour >> 24) & 0xFF);
	return 4;
}

int								gui_line							(lua_State *L)
{
	//TODO: 
	return 0;
}

int								gui_box								(lua_State *L)
{
	int x1, y1, x2, y2;
	uint32 fillcolor;
	uint32 outlinecolor;

	x1 = luaL_checkinteger(L,1);
	y1 = luaL_checkinteger(L,2);
	x2 = luaL_checkinteger(L,3);
	y2 = luaL_checkinteger(L,4);
	fillcolor = TryGetColor(L, 5, 0xFF0000FF);
	outlinecolor = TryGetColor(L, 6, 0xFFFFFFFF);

	if (x1 > x2) 
	{
		std::swap(x1, x2);
	}

	if (y1 > y2) 
	{
		std::swap(y1, y2);
	}

	for(int i = y1; i != y2; i ++)
	{
		for(int j = x1; j != x2; j ++)
		{
			gui_array[i * 1024 + j] = (i == y1 || i == (y2 - 1) || j == x1 || j == (x2 - 1)) ? outlinecolor : fillcolor;
		}
	}

	return 0;
}

//TEXT HELPERS
static uint8 FCEUFont[792] =
{
	6,  0,  0,  0,  0,  0,  0,  0,
	3, 64, 64, 64, 64, 64,  0, 64,
	5, 80, 80, 80,  0,  0,  0,  0,
	6, 80, 80,248, 80,248, 80, 80,
	6, 32,120,160,112, 40,240, 32,
	6, 64,168, 80, 32, 80,168, 16,
	6, 96,144,160, 64,168,144,104,
	3, 64, 64,  0,  0,  0,  0,  0,
	4, 32, 64, 64, 64, 64, 64, 32,
	4, 64, 32, 32, 32, 32, 32, 64,
	6,  0, 80, 32,248, 32, 80,  0,
	6,  0, 32, 32,248, 32, 32,  0,
	3,  0,  0,  0,  0,  0, 64,128,
	5,  0,  0,  0,240,  0,  0,  0,
	3,  0,  0,  0,  0,  0,  0, 64,
	5, 16, 16, 32, 32, 32, 64, 64,
	6,112,136,136,136,136,136,112, //0
	6, 32, 96, 32, 32, 32, 32, 32,
	6,112,136,  8, 48, 64,128,248,
	6,112,136,  8, 48,  8,136,112,
	6, 16, 48, 80,144,248, 16, 16,
	6,248,128,128,240,  8,  8,240,
	6, 48, 64,128,240,136,136,112,
	6,248,  8, 16, 16, 32, 32, 32,
	6,112,136,136,112,136,136,112,
	6,112,136,136,120,  8, 16, 96,
	3,  0,  0, 64,  0,  0, 64,  0,
	3,  0,  0, 64,  0,  0, 64,128,
	4,  0, 32, 64,128, 64, 32,  0,
	5,  0,  0,240,  0,240,  0,  0,
	4,  0,128, 64, 32, 64,128,  0,
	5,112,136,  8, 16, 32,  0, 32,
	6,112,136,136,184,176,128,112,
	6,112,136,136,248,136,136,136, //A
	6,240,136,136,240,136,136,240,
	6,112,136,128,128,128,136,112,
	6,224,144,136,136,136,144,224,
	6,248,128,128,240,128,128,248,
	6,248,128,128,240,128,128,128,
	6,112,136,128,184,136,136,120,
	6,136,136,136,248,136,136,136,
	4,224, 64, 64, 64, 64, 64,224,
	6,  8,  8,  8,  8,  8,136,112,
	6,136,144,160,192,160,144,136,
	6,128,128,128,128,128,128,248,
	6,136,216,168,168,136,136,136,
	6,136,136,200,168,152,136,136,
	7, 48, 72,132,132,132, 72, 48,
	6,240,136,136,240,128,128,128,
	6,112,136,136,136,168,144,104,
	6,240,136,136,240,144,136,136,
	6,112,136,128,112,  8,136,112,
	6,248, 32, 32, 32, 32, 32, 32,
	6,136,136,136,136,136,136,112,
	6,136,136,136, 80, 80, 32, 32,
	6,136,136,136,136,168,168, 80,
	6,136,136, 80, 32, 80,136,136,
	6,136,136, 80, 32, 32, 32, 32,
	6,248,  8, 16, 32, 64,128,248,
	3,192,128,128,128,128,128,192,
	5, 64, 64, 32, 32, 32, 16, 16,
	3,192, 64, 64, 64, 64, 64,192,
	4, 64,160,  0,  0,  0,  0,  0,
	6,  0,  0,  0,  0,  0,  0,248,
	3,128, 64,  0,  0,  0,  0,  0,
	5,  0,  0, 96, 16,112,144,112, //a
	5,128,128,224,144,144,144,224,
	5,  0,  0,112,128,128,128,112,
	5, 16, 16,112,144,144,144,112,
	5,  0,  0, 96,144,240,128,112,
	5, 48, 64,224, 64, 64, 64, 64,
	5,  0,112,144,144,112, 16,224,
	5,128,128,224,144,144,144,144,
	2,128,  0,128,128,128,128,128,
	4, 32,  0, 32, 32, 32, 32,192,
	5,128,128,144,160,192,160,144,
	2,128,128,128,128,128,128,128,
	6,  0,  0,208,168,168,168,168,
	5,  0,  0,224,144,144,144,144,
	5,  0,  0, 96,144,144,144, 96,
	5,  0,  0,224,144,144,224,128,
	5,  0,  0,112,144,144,112, 16,
	5,  0,  0,176,192,128,128,128,
	5,  0,  0,112,128, 96, 16,224,
	4, 64, 64,224, 64, 64, 64, 32,
	5,  0,  0,144,144,144,144,112,
	5,  0,  0,144,144,144,160,192,
	6,  0,  0,136,136,168,168, 80,
	5,  0,  0,144,144, 96,144,144,
	5,  0,144,144,144,112, 16, 96,
	5,  0,  0,240, 32, 64,128,240,
	4, 32, 64, 64,128, 64, 64, 32,
	3, 64, 64, 64, 64, 64, 64, 64,
	4,128, 64, 64, 32, 64, 64,128,
	6,  0,104,176,  0,  0,  0,  0
};

static int FixJoedChar(uint8 ch)
{
	int c = ch; c -= 32;
	return (c < 0 || c > 98) ? 0 : c;
}
static int JoedCharWidth(uint8 ch)
{
	return FCEUFont[FixJoedChar(ch)*8];
}

#define LUA_SCREEN_WIDTH 1024
#define LUA_SCREEN_HEIGHT 768
void LuaDrawTextTransWH(const char *str, size_t l, int &x, int y, uint32 color, uint32 backcolor)
{
	int Opac = (color >> 24) & 0xFF;
	int backOpac = (backcolor >> 24) & 0xFF;
	int origX = x;

	if(!Opac && !backOpac)
		return;

	size_t len = l;
//	int defaultAlpha = std::max(0, std::min(transparencyModifier, 255));
	int defaultAlpha = 255;
	int diffx;
	int diffy = std::max(0, std::min(7, LUA_SCREEN_HEIGHT - y));

	while(*str && len && y < LUA_SCREEN_HEIGHT)
	{
		int c = *str++;
		while (x >= LUA_SCREEN_WIDTH && c != '\n') {
			c = *str;
			if (c == '\0')
				break;
			str++;
			if (!(--len))
				break;
		}
		if(c == '\n')
		{
			x = origX;
			y += 8;
			diffy = std::max(0, std::min(7, LUA_SCREEN_HEIGHT - y));
			continue;
		}
		else if(c == '\t') // just in case
		{
			const int tabSpace = 8;
			x += (tabSpace-(((x-origX)/8)%tabSpace))*8;
			continue;
		}

		diffx = std::max(0, std::min(7, LUA_SCREEN_WIDTH - x));
		int ch  = FixJoedChar(c);
		int wid = std::min(diffx, JoedCharWidth(c));

		for(int y2 = 0; y2 < diffy; y2++)
		{
			uint8 d = FCEUFont[ch*8 + 1+y2];
			for(int x2 = 0; x2 < wid; x2++)
			{
				int c = (d >> (7-x2)) & 1;
				if(c)
					gui_array[(y+y2) * 1024 + x+x2] = color;
				else
					gui_array[(y+y2) * 1024 + x+x2] = backcolor;
			}
		}
		// shadows :P
//		if (diffy >= 7) for(int x2 = 0; x2 < wid; x2++)
//			gui_drawpixel_internal(x+x2, y+7, LUA_BUILD_PIXEL(defaultAlpha, 0, 0, 0));
//		if (*str == '\0' || *str == '\n') for(int y2 = 0; y2 < diffy; y2++)
//			gui_drawpixel_internal(x+wid, y+y2, LUA_BUILD_PIXEL(defaultAlpha, 0, 0, 0));

		x += wid;
		len--;
	}
}

//END TEXT HELPERS

int								gui_text							(lua_State *L)
{
	extern int font_height;
	const char *msg;
	int x, y;
	size_t l;

	x = luaL_checkinteger(L,1);
	y = luaL_checkinteger(L,2);
	msg = luaL_checklstring(L,3,&l);

	uint32 color = TryGetColor(L, 4, 0xFFFFFFFF);
	uint32 bgcolor = TryGetColor(L, 5, 0xFF2020FF);

	LuaDrawTextTransWH(msg, l, x, y, color, bgcolor);

    lua_pushinteger(L, x);
	return 1;
}


luaL_reg guilib[] =
{
	{"pixel", gui_pixel},
	{"getpixel", gui_getpixel},
	{"line", gui_line},
	{"box", gui_box},
	{"text", gui_text},

//	{"savescreenshot",   gui_savescreenshot},
//	{"savescreenshotas", gui_savescreenshotas},
//	{"gdscreenshot", gui_gdscreenshot},
//	{"gdoverlay", gui_gdoverlay},
//	{"opacity", gui_setopacity},
//	{"transparency", gui_transparency},

//	{"register", gui_register},

//	{"popup", gui_popup},
	// alternative names
	{"drawtext", gui_text},
	{"drawbox", gui_box},
	{"drawline", gui_line},
	{"drawpixel", gui_pixel},
	{"setpixel", gui_pixel},
	{"writepixel", gui_pixel},
	{"rect", gui_box},
	{"drawrect", gui_box},
//	{"drawimage", gui_gdoverlay},
//	{"image", gui_gdoverlay},
	{NULL,NULL}
};







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





#if 0
static inline bool isalphaorunderscore(char c)
{
	return isalpha(c) || c == '_';
}

#define APPENDPRINT { int _n = snprintf(ptr, remaining,
#define END ); if(_n >= 0) { ptr += _n; remaining -= _n; } else { remaining = 0; } }
static void toCStringConverter(lua_State* L, int i, char*& ptr, int& remaining)
{
	if(remaining <= 0)
		return;

	const char* str = ptr; // for debugging

	// if there is a __tostring metamethod then call it
	int usedMeta = luaL_callmeta(L, i, "__tostring");
	if(usedMeta)
	{
		std::vector<const void*>::const_iterator foundCycleIter = std::find(s_metacallStack.begin(), s_metacallStack.end(), lua_topointer(L,i));
		if(foundCycleIter != s_metacallStack.end())
		{
			lua_pop(L, 1);
			usedMeta = false;
		}
		else
		{
			s_metacallStack.push_back(lua_topointer(L,i));
			i = lua_gettop(L);
		}
	}

	switch(lua_type(L, i))
	{
		case LUA_TNONE: break;
		case LUA_TNIL: APPENDPRINT "nil" END break;
		case LUA_TBOOLEAN: APPENDPRINT lua_toboolean(L,i) ? "true" : "false" END break;
		case LUA_TSTRING: APPENDPRINT "%s",lua_tostring(L,i) END break;
		case LUA_TNUMBER: APPENDPRINT "%.12Lg",lua_tonumber(L,i) END break;
		case LUA_TFUNCTION: 
			/*if((L->base + i-1)->value.gc->cl.c.isC)
			{
				//lua_CFunction func = lua_tocfunction(L, i);
				//std::map<lua_CFunction, const char*>::iterator iter = s_cFuncInfoMap.find(func);
				//if(iter == s_cFuncInfoMap.end())
					goto defcase;
				//APPENDPRINT "function(%s)", iter->second END 
			}
			else
			{
				APPENDPRINT "function(" END 
				Proto* p = (L->base + i-1)->value.gc->cl.l.p;
				int numParams = p->numparams + (p->is_vararg?1:0);
				for (int n=0; n<p->numparams; n++)
				{
					APPENDPRINT "%s", getstr(p->locvars[n].varname) END 
					if(n != numParams-1)
						APPENDPRINT "," END
				}
				if(p->is_vararg)
					APPENDPRINT "..." END
				APPENDPRINT ")" END
			}*/
			goto defcase;
			break;
defcase:default: APPENDPRINT "%s:%p",luaL_typename(L,i),lua_topointer(L,i) END break;
		case LUA_TTABLE:
		{
			// first make sure there's enough stack space
			if(!lua_checkstack(L, 4))
			{
				// note that even if lua_checkstack never returns false,
				// that doesn't mean we didn't need to call it,
				// because calling it retrieves stack space past LUA_MINSTACK
				goto defcase;
			}

			std::vector<const void*>::const_iterator foundCycleIter = std::find(s_tableAddressStack.begin(), s_tableAddressStack.end(), lua_topointer(L,i));
			if(foundCycleIter != s_tableAddressStack.end())
			{
				int parentNum = s_tableAddressStack.end() - foundCycleIter;
				if(parentNum > 1)
					APPENDPRINT "%s:parent^%d",luaL_typename(L,i),parentNum END
				else
					APPENDPRINT "%s:parent",luaL_typename(L,i) END
			}
			else
			{
				s_tableAddressStack.push_back(lua_topointer(L,i));
				struct Scope { ~Scope(){ s_tableAddressStack.pop_back(); } } scope;

				APPENDPRINT "{" END

				lua_pushnil(L); // first key
				int keyIndex = lua_gettop(L);
				int valueIndex = keyIndex + 1;
				bool first = true;
				bool skipKey = true; // true if we're still in the "array part" of the table
				lua_Number arrayIndex = (lua_Number)0;
				while(lua_next(L, i))
				{
					if(first)
						first = false;
					else
						APPENDPRINT ", " END
					if(skipKey)
					{
						arrayIndex += (lua_Number)1;
						bool keyIsNumber = (lua_type(L, keyIndex) == LUA_TNUMBER);
						skipKey = keyIsNumber && (lua_tonumber(L, keyIndex) == arrayIndex);
					}
					if(!skipKey)
					{
						bool keyIsString = (lua_type(L, keyIndex) == LUA_TSTRING);
						bool invalidLuaIdentifier = (!keyIsString || !isalphaorunderscore(*lua_tostring(L, keyIndex)));
						if(invalidLuaIdentifier)
							if(keyIsString)
								APPENDPRINT "['" END
							else
								APPENDPRINT "[" END

						toCStringConverter(L, keyIndex, ptr, remaining); // key

						if(invalidLuaIdentifier)
							if(keyIsString)
								APPENDPRINT "']=" END
							else
								APPENDPRINT "]=" END
						else
							APPENDPRINT "=" END
					}

					bool valueIsString = (lua_type(L, valueIndex) == LUA_TSTRING);
					if(valueIsString)
						APPENDPRINT "'" END

					toCStringConverter(L, valueIndex, ptr, remaining); // value

					if(valueIsString)
						APPENDPRINT "'" END

					lua_pop(L, 1);

					if(remaining <= 0)
					{
						lua_settop(L, keyIndex-1); // stack might not be clean yet if we're breaking early
						break;
					}
				}
				APPENDPRINT "}" END
			}
		}	break;
	}

	if(usedMeta)
	{
		s_metacallStack.pop_back();
		lua_pop(L, 1);
	}
}

static const int s_tempStrMaxLen = 64 * 1024;
static char s_tempStr [s_tempStrMaxLen];

static char* rawToCString(lua_State* L, int idx)
{
	int a = idx>0 ? idx : 1;
	int n = idx>0 ? idx : lua_gettop(L);

	char* ptr = s_tempStr;
	*ptr = 0;

	int remaining = s_tempStrMaxLen;
	for(int i = a; i <= n; i++)
	{
		toCStringConverter(L, i, ptr, remaining);
		if(i != n)
			APPENDPRINT " " END
	}

	if(remaining < 3)
	{
		while(remaining < 6)
			remaining++, ptr--;
		APPENDPRINT "..." END
	}
	APPENDPRINT "\r\n" END
	// the trailing newline is so print() can avoid having to do wasteful things to print its newline
	// (string copying would be wasteful and calling info.print() twice can be extremely slow)
	// at the cost of functions that don't want the newline needing to trim off the last two characters
	// (which is a very fast operation and thus acceptable in this case)

	return s_tempStr;
}
#undef APPENDPRINT
#undef END


// replacement for luaB_tostring() that is able to show the contents of tables (and formats numbers better, and show function prototypes)
// can be called directly from lua via tostring(), assuming tostring hasn't been reassigned
static int tostring(lua_State *L)
{
	char* str = rawToCString(L);
	str[strlen(str)-2] = 0; // hack: trim off the \r\n (which is there to simplify the print function's task)
	lua_pushstring(L, str);
	return 1;
}

// like rawToCString, but will check if the global Lua function tostring()
// has been replaced with a custom function, and call that instead if so
static const char* toCString(lua_State* L, int idx)
{
	int a = idx>0 ? idx : 1;
	int n = idx>0 ? idx : lua_gettop(L);
	lua_getglobal(L, "tostring");
	lua_CFunction cf = lua_tocfunction(L,-1);
	if(cf == tostring) // optimization: if using our own C tostring function, we can bypass the call through Lua and all the string object allocation that would entail
	{
		lua_pop(L,1);
		return rawToCString(L, idx);
	}
	else // if the user overrided the tostring function, we have to actually call it and store the temporarily allocated string it returns
	{
		lua_pushstring(L, "");
		for (int i=a; i<=n; i++) {
			lua_pushvalue(L, -2);  // function to be called
			lua_pushvalue(L, i);   // value to print
			lua_call(L, 1, 1);
			if(lua_tostring(L, -1) == NULL)
				luaL_error(L, LUA_QL("tostring") " must return a string to " LUA_QL("print"));
			lua_pushstring(L, (i<n) ? " " : "\r\n");
			lua_concat(L, 3);
		}
		const char* str = lua_tostring(L, -1);
		strncpy(s_tempStr, str, s_tempStrMaxLen);
		s_tempStr[s_tempStrMaxLen-1] = 0;
		lua_pop(L, 2);
		return s_tempStr;
	}
}

// replacement for luaB_print() that goes to the appropriate textbox instead of stdout
static int print(lua_State *L)
{
	const char* str = toCString(L);

	int uid = info_uid;//luaStateToUIDMap[L->l_G->mainthread];
	//LuaContextInfo& info = GetCurrentInfo();

	if(info_print)
		info_print(uid, str);
	else
		puts(str);

	//worry(L, 100);
	return 0;
}

#if 0 //Save state stuff ?
// can't remember what the best way of doing this is...
#if defined(i386) || defined(__i386) || defined(__i386__) || defined(M_I86) || defined(_M_IX86) || defined(WIN32)
	#define IS_LITTLE_ENDIAN
#endif

// push a value's bytes onto the output stack
template<typename T>
void PushBinaryItem(T item, std::vector<unsigned char>& output)
{
	unsigned char* buf = (unsigned char*)&item;
#ifdef IS_LITTLE_ENDIAN
	for(int i = sizeof(T); i; i--)
		output.push_back(*buf++);
#else
	int vecsize = output.size();
	for(int i = sizeof(T); i; i--)
		output.insert(output.begin() + vecsize, *buf++);
#endif
}
// read a value from the byte stream and advance the stream by its size
template<typename T>
T AdvanceByteStream(const unsigned char*& data, unsigned int& remaining)
{
#ifdef IS_LITTLE_ENDIAN
	T rv = *(T*)data;
	data += sizeof(T);
#else
	T rv; unsigned char* rvptr = (unsigned char*)&rv;
	for(int i = sizeof(T)-1; i>=0; i--)
		rvptr[i] = *data++;
#endif
	remaining -= sizeof(T);
	return rv;
}
// advance the byte stream by a certain size without reading a value
void AdvanceByteStream(const unsigned char*& data, unsigned int& remaining, int amount)
{
	data += amount;
	remaining -= amount;
}

#define LUAEXT_TLONG		30 // 0x1E // 4-byte signed integer
#define LUAEXT_TUSHORT		31 // 0x1F // 2-byte unsigned integer
#define LUAEXT_TSHORT		32 // 0x20 // 2-byte signed integer
#define LUAEXT_TBYTE		33 // 0x21 // 1-byte unsigned integer
#define LUAEXT_TNILS		34 // 0x22 // multiple nils represented by a 4-byte integer (warning: becomes multiple stack entities)
#define LUAEXT_TTABLE		0x40 // 0x40 through 0x4F // tables of different sizes:
#define LUAEXT_BITS_1A		0x01 // size of array part fits in a 1-byte unsigned integer
#define LUAEXT_BITS_2A		0x02 // size of array part fits in a 2-byte unsigned integer
#define LUAEXT_BITS_4A		0x03 // size of array part fits in a 4-byte unsigned integer
#define LUAEXT_BITS_1H		0x04 // size of hash part fits in a 1-byte unsigned integer
#define LUAEXT_BITS_2H		0x08 // size of hash part fits in a 2-byte unsigned integer
#define LUAEXT_BITS_4H		0x0C // size of hash part fits in a 4-byte unsigned integer
#define BITMATCH(x,y) (((x) & (y)) == (y))

static void PushNils(std::vector<unsigned char>& output, int& nilcount)
{
	int count = nilcount;
	nilcount = 0;

	static const int minNilsWorthEncoding = 6; // because a LUAEXT_TNILS entry is 5 bytes

	if(count < minNilsWorthEncoding)
	{
		for(int i = 0; i < count; i++)
			output.push_back(LUA_TNIL);
	}
	else
	{
		output.push_back(LUAEXT_TNILS);
		PushBinaryItem<uint32>(count, output);
	}
}

static std::vector<const void*> s_tableAddressStack; // prevents infinite recursion of a table within a table (when cycle is found, print something like table:parent)
static std::vector<const void*> s_metacallStack; // prevents infinite recursion if something's __tostring returns another table that contains that something (when cycle is found, print the inner result without using __tostring)

static void LuaStackToBinaryConverter(lua_State* L, int i, std::vector<unsigned char>& output)
{
	int type = lua_type(L, i);

	// the first byte of every serialized item says what Lua type it is
	output.push_back(type & 0xFF);

	switch(type)
	{
//ROBO: TODO: Info print
/*		default:
			{
				char errmsg [1024];
				sprintf(errmsg, "values of type \"%s\" are not allowed to be returned from registered save functions.\r\n", luaL_typename(L,i));
				if(info_print)
					info_print(info_uid, errmsg);
				else
					puts(errmsg);
			}
			break;*/
		case LUA_TNIL:
			// no information necessary beyond the type
			break;
		case LUA_TBOOLEAN:
			// serialize as 0 or 1
			output.push_back(lua_toboolean(L,i));
			break;
		case LUA_TSTRING:
			// serialize as a 0-terminated string of characters
			{
				const char* str = lua_tostring(L,i);
				while(*str)
					output.push_back(*str++);
				output.push_back('\0');
			}
			break;
		case LUA_TNUMBER:
			{
				double num = (double)lua_tonumber(L,i);
				int32 inum = (int32)lua_tointeger(L,i);
				if(num != inum)
				{
					PushBinaryItem(num, output);
				}
				else
				{
					if((inum & ~0xFF) == 0)
						type = LUAEXT_TBYTE;
					else if((uint16)(inum & 0xFFFF) == inum)
						type = LUAEXT_TUSHORT;
					else if((int16)(inum & 0xFFFF) == inum)
						type = LUAEXT_TSHORT;
					else
						type = LUAEXT_TLONG;
					output.back() = type;
					switch(type)
					{
					case LUAEXT_TLONG:
						PushBinaryItem<int32>(static_cast<int32>(inum), output);
						break;
					case LUAEXT_TUSHORT:
						PushBinaryItem<uint16>(static_cast<uint16>(inum), output);
						break;
					case LUAEXT_TSHORT:
						PushBinaryItem<int16>(static_cast<int16>(inum), output);
						break;
					case LUAEXT_TBYTE:
						output.push_back(static_cast<uint8>(inum));
						break;
					}
				}
			}
			break;
		case LUA_TTABLE:
			// serialize as a type that describes how many bytes are used for storing the counts,
			// followed by the number of array entries if any, then the number of hash entries if any,
			// then a Lua value per array entry, then a (key,value) pair of Lua values per hashed entry
			// note that the structure of table references are not faithfully serialized (yet)
		{
			int outputTypeIndex = output.size() - 1;
			int arraySize = 0;
			int hashSize = 0;

			if(lua_checkstack(L, 4) && std::find(s_tableAddressStack.begin(), s_tableAddressStack.end(), lua_topointer(L,i)) == s_tableAddressStack.end())
			{
				s_tableAddressStack.push_back(lua_topointer(L,i));
				struct Scope { ~Scope(){ s_tableAddressStack.pop_back(); } } scope;

				bool wasnil = false;
				int nilcount = 0;
				arraySize = lua_objlen(L, i);
				int arrayValIndex = lua_gettop(L) + 1;
				for(int j = 1; j <= arraySize; j++)
				{
			        lua_rawgeti(L, i, j);
					bool isnil = lua_isnil(L, arrayValIndex);
					if(isnil)
						nilcount++;
					else
					{
						if(wasnil)
							PushNils(output, nilcount);
						LuaStackToBinaryConverter(L, arrayValIndex, output);
					}
					lua_pop(L, 1);
					wasnil = isnil;
				}
				if(wasnil)
					PushNils(output, nilcount);

				if(arraySize)
					lua_pushinteger(L, arraySize); // before first key
				else
					lua_pushnil(L); // before first key

				int keyIndex = lua_gettop(L);
				int valueIndex = keyIndex + 1;
				while(lua_next(L, i))
				{
//					assert(lua_type(L, keyIndex) && "nil key in Lua table, impossible");
//					assert(lua_type(L, valueIndex) && "nil value in Lua table, impossible");
					LuaStackToBinaryConverter(L, keyIndex, output);
					LuaStackToBinaryConverter(L, valueIndex, output);
					lua_pop(L, 1);
					hashSize++;
				}
			}

			int outputType = LUAEXT_TTABLE;
			if(arraySize & 0xFFFF0000)
				outputType |= LUAEXT_BITS_4A;
			else if(arraySize & 0xFF00)
				outputType |= LUAEXT_BITS_2A;
			else if(arraySize & 0xFF)
				outputType |= LUAEXT_BITS_1A;
			if(hashSize & 0xFFFF0000)
				outputType |= LUAEXT_BITS_4H;
			else if(hashSize & 0xFF00)
				outputType |= LUAEXT_BITS_2H;
			else if(hashSize & 0xFF)
				outputType |= LUAEXT_BITS_1H;
			output[outputTypeIndex] = outputType;

			int insertIndex = outputTypeIndex;
			if(BITMATCH(outputType,LUAEXT_BITS_4A) || BITMATCH(outputType,LUAEXT_BITS_2A) || BITMATCH(outputType,LUAEXT_BITS_1A))
				output.insert(output.begin() + (++insertIndex), arraySize & 0xFF);
			if(BITMATCH(outputType,LUAEXT_BITS_4A) || BITMATCH(outputType,LUAEXT_BITS_2A))
				output.insert(output.begin() + (++insertIndex), (arraySize & 0xFF00) >> 8);
			if(BITMATCH(outputType,LUAEXT_BITS_4A))
				output.insert(output.begin() + (++insertIndex), (arraySize & 0x00FF0000) >> 16),
				output.insert(output.begin() + (++insertIndex), (arraySize & 0xFF000000) >> 24);
			if(BITMATCH(outputType,LUAEXT_BITS_4H) || BITMATCH(outputType,LUAEXT_BITS_2H) || BITMATCH(outputType,LUAEXT_BITS_1H))
				output.insert(output.begin() + (++insertIndex), hashSize & 0xFF);
			if(BITMATCH(outputType,LUAEXT_BITS_4H) || BITMATCH(outputType,LUAEXT_BITS_2H))
				output.insert(output.begin() + (++insertIndex), (hashSize & 0xFF00) >> 8);
			if(BITMATCH(outputType,LUAEXT_BITS_4H))
				output.insert(output.begin() + (++insertIndex), (hashSize & 0x00FF0000) >> 16),
				output.insert(output.begin() + (++insertIndex), (hashSize & 0xFF000000) >> 24);

		}	break;
	}
}


// complements LuaStackToBinaryConverter
void BinaryToLuaStackConverter(lua_State* L, const unsigned char*& data, unsigned int& remaining)
{
//	assert(s_dbg_dataSize - (data - s_dbg_dataStart) == remaining);

	unsigned char type = AdvanceByteStream<unsigned char>(data, remaining);

	switch(type)
	{
//ROBO: TODO: info_print?
/*		default:
			{
				char errmsg [1024];
				if(type <= 10 && type != LUA_TTABLE)
					sprintf(errmsg, "values of type \"%s\" are not allowed to be loaded into registered load functions. The save state's Lua save data file might be corrupted.\r\n", lua_typename(L,type));
				else
					sprintf(errmsg, "The save state's Lua save data file seems to be corrupted.\r\n");
				if(info_print)
					info_print(info_uid, errmsg);
				else
					puts(errmsg);
			}
			break;*/
		case LUA_TNIL:
			lua_pushnil(L);
			break;
		case LUA_TBOOLEAN:
			lua_pushboolean(L, AdvanceByteStream<uint8>(data, remaining));
			break;
		case LUA_TSTRING:
			lua_pushstring(L, (const char*)data);
			AdvanceByteStream(data, remaining, strlen((const char*)data) + 1);
			break;
		case LUA_TNUMBER:
			lua_pushnumber(L, AdvanceByteStream<double>(data, remaining));
			break;
		case LUAEXT_TLONG:
			lua_pushinteger(L, AdvanceByteStream<int32>(data, remaining));
			break;
		case LUAEXT_TUSHORT:
			lua_pushinteger(L, AdvanceByteStream<uint16>(data, remaining));
			break;
		case LUAEXT_TSHORT:
			lua_pushinteger(L, AdvanceByteStream<int16>(data, remaining));
			break;
		case LUAEXT_TBYTE:
			lua_pushinteger(L, AdvanceByteStream<uint8>(data, remaining));
			break;
		case LUAEXT_TTABLE:
		case LUAEXT_TTABLE | LUAEXT_BITS_1A:
		case LUAEXT_TTABLE | LUAEXT_BITS_2A:
		case LUAEXT_TTABLE | LUAEXT_BITS_4A:
		case LUAEXT_TTABLE | LUAEXT_BITS_1H:
		case LUAEXT_TTABLE | LUAEXT_BITS_2H:
		case LUAEXT_TTABLE | LUAEXT_BITS_4H:
		case LUAEXT_TTABLE | LUAEXT_BITS_1A | LUAEXT_BITS_1H:
		case LUAEXT_TTABLE | LUAEXT_BITS_2A | LUAEXT_BITS_1H:
		case LUAEXT_TTABLE | LUAEXT_BITS_4A | LUAEXT_BITS_1H:
		case LUAEXT_TTABLE | LUAEXT_BITS_1A | LUAEXT_BITS_2H:
		case LUAEXT_TTABLE | LUAEXT_BITS_2A | LUAEXT_BITS_2H:
		case LUAEXT_TTABLE | LUAEXT_BITS_4A | LUAEXT_BITS_2H:
		case LUAEXT_TTABLE | LUAEXT_BITS_1A | LUAEXT_BITS_4H:
		case LUAEXT_TTABLE | LUAEXT_BITS_2A | LUAEXT_BITS_4H:
		case LUAEXT_TTABLE | LUAEXT_BITS_4A | LUAEXT_BITS_4H:
			{
				unsigned int arraySize = 0;
				if(BITMATCH(type,LUAEXT_BITS_4A) || BITMATCH(type,LUAEXT_BITS_2A) || BITMATCH(type,LUAEXT_BITS_1A))
					arraySize |= AdvanceByteStream<uint8>(data, remaining);
				if(BITMATCH(type,LUAEXT_BITS_4A) || BITMATCH(type,LUAEXT_BITS_2A))
					arraySize |= ((uint16)AdvanceByteStream<uint8>(data, remaining)) << 8;
				if(BITMATCH(type,LUAEXT_BITS_4A))
					arraySize |= ((uint32)AdvanceByteStream<uint8>(data, remaining)) << 16,
					arraySize |= ((uint32)AdvanceByteStream<uint8>(data, remaining)) << 24;

				unsigned int hashSize = 0;
				if(BITMATCH(type,LUAEXT_BITS_4H) || BITMATCH(type,LUAEXT_BITS_2H) || BITMATCH(type,LUAEXT_BITS_1H))
					hashSize |= AdvanceByteStream<uint8>(data, remaining);
				if(BITMATCH(type,LUAEXT_BITS_4H) || BITMATCH(type,LUAEXT_BITS_2H))
					hashSize |= ((uint16)AdvanceByteStream<uint8>(data, remaining)) << 8;
				if(BITMATCH(type,LUAEXT_BITS_4H))
					hashSize |= ((uint32)AdvanceByteStream<uint8>(data, remaining)) << 16,
					hashSize |= ((uint32)AdvanceByteStream<uint8>(data, remaining)) << 24;

				lua_checkstack(L, 8);

				lua_createtable(L, arraySize, hashSize);

				unsigned int n = 1;
				while(n <= arraySize)
				{
					if(*data == LUAEXT_TNILS)
					{
						AdvanceByteStream(data, remaining, 1);
						n += AdvanceByteStream<uint32>(data, remaining);
					}
					else
					{
						BinaryToLuaStackConverter(L, data, remaining); // push value
						lua_rawseti(L, -2, n); // table[n] = value
						n++;
					}
				}

				for(unsigned int h = 1; h <= hashSize; h++)
				{
					BinaryToLuaStackConverter(L, data, remaining); // push key
					BinaryToLuaStackConverter(L, data, remaining); // push value
					lua_rawset(L, -3); // table[key] = value
				}
			}
			break;
	}
}

static const unsigned char luaBinaryMajorVersion = 9;
static const unsigned char luaBinaryMinorVersion = 1;

unsigned char* LuaStackToBinary(lua_State* L, unsigned int& size)
{
	int n = lua_gettop(L);
	if(n == 0)
		return NULL;

	std::vector<unsigned char> output;
	output.push_back(luaBinaryMajorVersion);
	output.push_back(luaBinaryMinorVersion);

	for(int i = 1; i <= n; i++)
		LuaStackToBinaryConverter(L, i, output);

	unsigned char* rv = new unsigned char [output.size()];
	memcpy(rv, &output.front(), output.size());
	size = output.size();
	return rv;
}

void BinaryToLuaStack(lua_State* L, const unsigned char* data, unsigned int size, unsigned int itemsToLoad)
{
	unsigned char major = *data++;
	unsigned char minor = *data++;
	size -= 2;
	if(luaBinaryMajorVersion != major || luaBinaryMinorVersion != minor)
		return;

	while(size > 0 && itemsToLoad > 0)
	{
		BinaryToLuaStackConverter(L, data, size);
		itemsToLoad--;
	}
}

// saves Lua stack into a record and pops it
void LuaSaveData::SaveRecord(lua_State* L, unsigned int key)
{
	if(!L)
		return;

	Record* cur = new Record();
	cur->key = key;
	cur->data = LuaStackToBinary(L, cur->size);
	cur->next = NULL;

	lua_settop(L,0);

	if(cur->size <= 0)
	{
		delete cur;
		return;
	}

	Record* last = recordList;
	while(last && last->next)
		last = last->next;
	if(last)
		last->next = cur;
	else
		recordList = cur;
}

// pushes a record's data onto the Lua stack
void LuaSaveData::LoadRecord(struct lua_State* L, unsigned int key, unsigned int itemsToLoad) const
{
	if(!L)
		return;

	Record* cur = recordList;
	while(cur)
	{
		if(cur->key == key)
		{
//			s_dbg_dataStart = cur->data;
//			s_dbg_dataSize = cur->size;
			BinaryToLuaStack(L, cur->data, cur->size, itemsToLoad);
			return;
		}
		cur = cur->next;
	}
}

// saves part of the Lua stack (at the given index) into a record and does NOT pop anything
void LuaSaveData::SaveRecordPartial(struct lua_State* L, unsigned int key, int idx)
{
	if(!L)
		return;

	if(idx < 0)
		idx += lua_gettop(L)+1;

	Record* cur = new Record();
	cur->key = key;
	cur->next = NULL;

	if(idx <= lua_gettop(L))
	{
		std::vector<unsigned char> output;
		output.push_back(luaBinaryMajorVersion);
		output.push_back(luaBinaryMinorVersion);

		LuaStackToBinaryConverter(L, idx, output);

		unsigned char* rv = new unsigned char [output.size()];
		memcpy(rv, &output.front(), output.size());
		cur->size = output.size();
		cur->data = rv;
	}

	if(cur->size <= 0)
	{
		delete cur;
		return;
	}

	Record* last = recordList;
	while(last && last->next)
		last = last->next;
	if(last)
		last->next = cur;
	else
		recordList = cur;
}

void fwriteint(unsigned int value, FILE* file)
{
	for(int i=0;i<4;i++)
	{
		int w = value & 0xFF;
		fwrite(&w, 1, 1, file);
		value >>= 8;
	}
}
void freadint(unsigned int& value, FILE* file)
{
	int rv = 0;
	for(int i=0;i<4;i++)
	{
		int r = 0;
		fread(&r, 1, 1, file);
		rv |= r << (i*8);
	}
	value = rv;
}

// writes all records to an already-open file
void LuaSaveData::ExportRecords(void* fileV) const
{
	FILE* file = (FILE*)fileV;
	if(!file)
		return;

	Record* cur = recordList;
	while(cur)
	{
		fwriteint(cur->key, file);
		fwriteint(cur->size, file);
		fwrite(cur->data, cur->size, 1, file);
		cur = cur->next;
	}
}

// reads records from an already-open file
void LuaSaveData::ImportRecords(void* fileV)
{
	FILE* file = (FILE*)fileV;
	if(!file)
		return;

	ClearRecords();

	Record rec;
	Record* cur = &rec;
	Record* last = NULL;
	while(1)
	{
		freadint(cur->key, file);
		freadint(cur->size, file);

		if(feof(file) || ferror(file))
			break;

		cur->data = new unsigned char [cur->size];
		fread(cur->data, cur->size, 1, file);

		Record* next = new Record();
		memcpy(next, cur, sizeof(Record));
		next->next = NULL;

		if(last)
			last->next = next;
		else
			recordList = next;
		last = next;
	}
}

void LuaSaveData::ClearRecords()
{
	Record* cur = recordList;
	while(cur)
	{
		Record* del = cur;
		cur = cur->next;

		delete[] del->data;
		delete del;
	}

	recordList = NULL;
}






void CallRegisteredLuaSaveFunctions(int savestateNumber, LuaSaveData& saveData)
{
	//lua_State* L = FCEU_GetLuaState();
	if(L)
	{
		lua_settop(L, 0);
		lua_getfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFORESAVE]);
		
		if (lua_isfunction(L, -1))
		{
			lua_pushinteger(L, savestateNumber);
			int ret = lua_pcall(L, 1, LUA_MULTRET, 0);
			if (ret != 0) {
				// This is grounds for trashing the function
				lua_pushnil(L);
				lua_setfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_BEFORESAVE]);
#ifdef WIN32
				MessageBox(hAppWnd, lua_tostring(L, -1), "Lua Error in SAVE function", MB_OK);
#else
				fprintf(stderr, "Lua error in registersave function: %s\n", lua_tostring(L, -1));
#endif
			}
			saveData.SaveRecord(L, LUA_DATARECORDKEY);
		}
		else
		{
			lua_pop(L, 1);
		}
	}
}


void CallRegisteredLuaLoadFunctions(int savestateNumber, const LuaSaveData& saveData)
{
	//lua_State* L = FCEU_GetLuaState();
	if(L)
	{
		lua_settop(L, 0);
		lua_getfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_AFTERLOAD]);
		
		if (lua_isfunction(L, -1))
		{
			// since the scriptdata can be very expensive to load
			// (e.g. the registered save function returned some huge tables)
			// check the number of parameters the registered load function expects
			// and don't bother loading the parameters it wouldn't receive anyway
			int numParamsExpected = (L->top - 1)->value.gc->cl.l.p->numparams; // NOTE: if this line crashes, that means your Lua headers are out of sync with your Lua lib
			if(numParamsExpected) numParamsExpected--; // minus one for the savestate number we always pass in

			int prevGarbage = lua_gc(L, LUA_GCCOUNT, 0);

			lua_pushinteger(L, savestateNumber);
			saveData.LoadRecord(L, LUA_DATARECORDKEY, numParamsExpected);
			int n = lua_gettop(L) - 1;

			int ret = lua_pcall(L, n, 0, 0);
			if (ret != 0) {
				// This is grounds for trashing the function
				lua_pushnil(L);
				lua_setfield(L, LUA_REGISTRYINDEX, luaCallIDStrings[LUACALL_AFTERLOAD]);
#ifdef WIN32
				MessageBox(hAppWnd, lua_tostring(L, -1), "Lua Error in LOAD function", MB_OK);
#else
				fprintf(stderr, "Lua error in registerload function: %s\n", lua_tostring(L, -1));
#endif
			}
			else
			{
				int newGarbage = lua_gc(L, LUA_GCCOUNT, 0);
				if(newGarbage - prevGarbage > 50)
				{
					// now seems to be a very good time to run the garbage collector
					// it might take a while now but that's better than taking 10 whiles 9 loads from now
					lua_gc(L, LUA_GCCOLLECT, 0);
				}
			}
		}
		else
		{
			lua_pop(L, 1);
		}
	}
}
#endif

#endif
