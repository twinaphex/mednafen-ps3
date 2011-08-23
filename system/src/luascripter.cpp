#ifdef USE_LUA

#include <es_system.h>

#include <random/mersennetwister.h>

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

namespace
{
	int						luaDisplayMessage							(lua_State* L)
	{
		ErrorCheck(lua_gettop(L) == 1 && lua_isstring(L, 1), "Lua: luaDisplayMessage argument error");

		ESSUB_Error(lua_tostring(L, 1));
		return 0;
	}

	int						luaLogMessage								(lua_State* L)
	{
		ErrorCheck(lua_gettop(L) == 1 && lua_isstring(L, 1), "Lua: luaLogMessage argument error");

		es_log->AppendLine(lua_tostring(L, 1));
		return 0;
	}

	int						luaRandom									(lua_State* L)
	{
		static MTRand mtrng(time(0));

		ErrorCheck(lua_gettop(L) == 2 && lua_isnumber(L, 1) && lua_isnumber(L, 2), "Lua: luaRandom argument error");

		int result = mtrng.randInt(lua_tonumber(L, 2));
		result += lua_tonumber(L, 1);
		lua_pushnumber(L, result);

		return 1;
	}
}

							LuaScripter::LuaScripter					()
{
	LuaState = lua_open();
	luaL_openlibs(LuaState);
	lua_settop(LuaState, 0);

	lua_register(LuaState, "Message", luaDisplayMessage);
	lua_register(LuaState, "Log", luaLogMessage);
	lua_register(LuaState, "Random", luaRandom);
}

							LuaScripter::~LuaScripter					()
{
	lua_close(LuaState);
}

void						LuaScripter::RegisterLibrary				(const std::string& aName, const luaL_Reg* aLibrary)
{
	luaL_register(LuaState, aName.c_str(), aLibrary);
	lua_pop(LuaState, 1);
}

void						LuaScripter::RegisterFunction				(const std::string& aName, LuaFunction aFunction)
{
	lua_register(LuaState, aName.c_str(), aFunction);
}

void						LuaScripter::LoadScript						(const std::string& aFileName)
{
	ErrorCheck(0 == luaL_loadfile(LuaState, aFileName.c_str()), "Lua Scripter: Failed to load script file [File: %s, Error: %s]", aFileName.c_str(), lua_tostring(LuaState, -1));
}

void						LuaScripter::DoScript						(const std::string& aFileName)
{
	ErrorCheck(0 == luaL_dofile(LuaState, aFileName.c_str()), "Lua Scripter: Failed to load script file [File: %s, Error: %s]", aFileName.c_str(), lua_tostring(LuaState, -1));
}

bool						LuaScripter::IsFunction						(const std::string& aFunction)
{
	lua_getglobal(LuaState, aFunction.c_str());
	bool result = lua_isfunction(LuaState, -1);
	lua_pop(LuaState, 1);
	return result;

}

uint32_t					LuaScripter::Call							(const std::string& aFunction)
{
	lua_getglobal(LuaState, aFunction.c_str());
	lua_call(LuaState, 0, 1);

	uint32_t result = lua_tonumber(LuaState, -1);
	lua_pop(LuaState, 1);

	return result;
}

uint32_t					LuaScripter::Call							(const std::string& aFunction, uint32_t aA)
{
	lua_getglobal(LuaState, aFunction.c_str());
	lua_pushnumber(LuaState, aA);

	lua_call(LuaState, 1, 0);

	return 0;
}

uint32_t					LuaScripter::Call							(const std::string& aFunction, uint32_t aA, uint32_t aB)
{
	lua_getglobal(LuaState, aFunction.c_str());
	lua_pushnumber(LuaState, aA);
	lua_pushnumber(LuaState, aB);

	lua_call(LuaState, 2, 0);

	return 0;
}


#endif //USE_LUA
