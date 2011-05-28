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

int								emu_emulating						(lua_State *L)
{
	lua_pushboolean(L, 0 != MednafenEmu::GetGameInfo());
	return 1;
}

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


int								memory_readbyterange				(lua_State *L)
{
	int range_start = luaL_checkinteger(L, 1);
	int range_size = luaL_checkinteger(L, 2);
	const MDFNGI* info = MednafenEmu::GetGameInfo();

	if(range_size && info && info->Peek)
	{
		char* buf = (char*)alloca(range_size);
		for(int i = 0; i != range_size; i ++)
		{
			buf[i] = info->Peek(range_start + i);
		}

		lua_pushlstring(L, buf, range_size);
		return 1;
	}
	else
	{
		return 0;
	}
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

luaL_reg memorylib [] = {

	{"readbyte", memory_readbyte},
	{"readbyterange", memory_readbyterange},
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

