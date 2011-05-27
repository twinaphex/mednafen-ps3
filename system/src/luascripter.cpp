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

		es_log->Log(lua_tostring(L, 1));
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
	ErrorCheck(0 == luaL_dofile(LuaState, aFileName.c_str()), "Lua Scripter: Failed to load script file [File: %s]", aFileName.c_str());
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

/*void						LuaScripter::InsertDataSetItem				(const TiXmlNode* aDescription, const TiXmlNode* aData)
{
	//Get the top of the lua stack
	int32_t top = lua_gettop(LuaState);

	//Insert only matching nodes
	ErrorCheck(aDescription->ValueStr() == aData->ValueStr(), "Lua Loader: Data and description node names don't match [Data: %s, Desc: %s]", aData->Value(), aDescription->Value());

	//Create a new lua table
	lua_newtable(LuaState);

	//Set the TYPE value
	lua_pushstring(LuaState, "TYPE");
	lua_pushstring(LuaState, aData->Value());
	lua_rawset(LuaState, top + 1);

	//Copy every attribute
	for(const TiXmlAttribute* attr = aDescription->ToElement()->FirstAttribute(); attr; attr = attr->Next())
	{
		ErrorCheck(attr->ValueStr() == "string" || attr->ValueStr() == "int", "Lua Loader: Data type is unknown, must be either string or int [Type: %s]", attr->Value());

		lua_pushstring(LuaState, attr->Name());
		if(attr->ValueStr() == "string")
		{
			lua_pushstring(LuaState, TryXmlGetStringAttribute(aData->ToElement(), attr->Name()));
		}
		else if(attr->ValueStr() == "int")
		{
			lua_pushnumber(LuaState, TryXmlGetIntAttribute(aData->ToElement(), attr->Name()));
		}

		lua_rawset(LuaState, top + 1);
	}

	//Copy child nodes
	for(const TiXmlNode* node = aDescription->ToElement()->FirstChild(); node; node = node->NextSibling())
	{
		if(aData->FirstChild(node->ValueStr()))
		{
			lua_pushstring(LuaState, TryXmlGetStringAttribute(aData->FirstChild(node->ValueStr())->ToElement(), "name", aData->FirstChild(node->ValueStr())->Value()));
			InsertDataSetItem(node, aData->FirstChild(node->ValueStr()));
		}
		else
		{
			throw ESException("Lua Loader: Child node missing [Node: %s]", node->Value());
		}
	}

	//Set the table to lua
	lua_rawset(LuaState, top - 1);
}

void						LuaScripter::InsertDataSet					(const std::string& aFileName)
{
	//Make the new table on the lua stack
	lua_newtable(LuaState);

	//Load the document
	const TiXmlNode* node = XMLCache::LoadDocument(aFileName, "dataset");

	//Get the description and data nodes
	const TiXmlNode* description = node->FirstChild("description");
	const TiXmlNode* data = node->FirstChild("data");

	ErrorCheck(description && data, "Lua Scripter: Dataset XML data or description node missing [File:%s]", aFileName.c_str());

	//Get the node name
	std::string nodename = TryXmlGetStringAttribute(description->ToElement(), "node");

	//Insert every child
	for(const TiXmlNode* child = data->FirstChild(); child; child = child->NextSibling())
	{
		if(child->ValueStr() == nodename && child->Type() == TiXmlNode::TINYXML_ELEMENT)
		{
			lua_pushstring(LuaState, TryXmlGetStringAttribute(child->ToElement(), "name", child->Value()));
			InsertDataSetItem(description->FirstChild(child->ValueStr()), child);
		}
	}

	//Set the table as a global
	lua_setglobal(LuaState, TryXmlGetStringAttribute(node->ToElement(), "name"));
}

const TiXmlNode*			LuaScripter::InsertData						(uint32_t aID, const std::string& aFileName, const std::string& aName)
{
	//Get the data table and it's ID on the stack
	lua_getglobal(LuaState, "objs");
	lua_pushnumber(LuaState, aID);

	//Load the document
	const TiXmlNode* node = XMLCache::LoadDocument(aFileName, "dataset");

	//Get the data and description nodes
	const TiXmlNode* description = node->FirstChild("description");
	const TiXmlNode* data = node->FirstChild("data");
	const TiXmlNode* output = 0;

	ErrorCheck(description && data, "Lua Scripter: Dataset XML data or description node missing [File:%s]", aFileName.c_str());

	//Get the name of our node type
	std::string nodename = TryXmlGetStringAttribute(description->ToElement(), "node");

	//Look for and insert the proper node
	for(const TiXmlNode* child = data->FirstChild(); child; child = child->NextSibling())
	{
		if(child->Type() == TiXmlNode::TINYXML_ELEMENT && child->ValueStr() == nodename && aName == TryXmlGetStringAttribute(child->ToElement(), "name"))
		{
			InsertDataSetItem(description->FirstChild(child->ValueStr()), child);
			output = child;
			break;
		}
	}

	//Check error
	ErrorCheck(output, "Lua Scripter: Dataset XML missing named item [Name: %s]", aName.c_str());

	//Clean up
	lua_pop(LuaState, 1);
	return output;
}

void						LuaScripter::DeleteData						(uint32_t aID)
{
	lua_getglobal(LuaState, "objs");

	lua_pushnumber(LuaState, aID);
	lua_pushnil(LuaState);
	lua_rawset(LuaState, -3);

	lua_pop(LuaState, 1);
}
*/

