#pragma once

struct lua_State;

class								LuaScripter
{
	public:
									LuaScripter						();
									~LuaScripter					();

		void						LoadScript						(const std::string& aFileName);

		bool						IsFunction						(const std::string& aFunction);
		uint32_t					Call							(const std::string& aFunction);
		uint32_t					Call							(const std::string& aFunction, uint32_t aA);
		uint32_t					Call							(const std::string& aFunction, uint32_t aA, uint32_t aB);

//		void						InsertDataSetItem				(const TiXmlNode* aDescription, const TiXmlNode* aData);
//		void						InsertDataSet					(const std::string& aFileName);

//		const TiXmlNode*			InsertData						(uint32_t aID, const std::string& aFileName, const std::string& aName);
//		void						DeleteData						(uint32_t aID);

	private:

		lua_State*					LuaState;
};

