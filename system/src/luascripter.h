#pragma once

struct lua_State;
struct luaL_Reg;

typedef int (*LuaFunction)(lua_State* L);
class								LuaScripter
{
	public:
									LuaScripter						();
									~LuaScripter					();

		void						RegisterLibrary					(const std::string& aName, const luaL_Reg* aLibrary);
		void						RegisterFunction				(const std::string& aName, LuaFunction aFunction);

		void						LoadScript						(const std::string& aFileName);

		bool						IsFunction						(const std::string& aFunction);
		uint32_t					Call							(const std::string& aFunction);
		uint32_t					Call							(const std::string& aFunction, uint32_t aA);
		uint32_t					Call							(const std::string& aFunction, uint32_t aA, uint32_t aB);

//		void						InsertDataSetItem				(const TiXmlNode* aDescription, const TiXmlNode* aData);
//		void						InsertDataSet					(const std::string& aFileName);

//		const TiXmlNode*			InsertData						(uint32_t aID, const std::string& aFileName, const std::string& aName);
//		void						DeleteData						(uint32_t aID);

	public: //HACK

		lua_State*					LuaState;
};

