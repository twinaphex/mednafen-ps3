#ifndef SYSTEM__WINTERFACE_H
#define SYSTEM__WINTERFACE_H

class													ImageManager
{
	public:
		static void										Purge							();
		
		static Texture*									LoadImage						(std::string aName, const void* aPngData, uint32_t aPngSize);
		static Texture*									GetImage						(std::string aName);

	protected:
		static std::map<std::string, Texture*>			Images;
};


class													Winterface : public Menu
{
	public:
														Winterface						(std::string aHeader, bool aKillItems = true, MenuHook* aHook = 0);
		virtual											~Winterface						();

		virtual bool									Input							();													
		virtual bool									Draw							();

		virtual bool									DrawLeft						()		{return false;};
		virtual bool									DrawRight						();
		
		virtual std::string								GetHeader						()		{return Header;};
		
	protected:
		bool											KillItems;
		std::vector<ListItem*>							SideItems;

		std::string										Header;
		
		static bool										HideRight;
};


#endif