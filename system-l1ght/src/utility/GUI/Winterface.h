#ifndef SYSTEM__WINTERFACE_H
#define SYSTEM__WINTERFACE_H

class													Winterface : public Menu
{
	public:
														Winterface						(const std::string& aHeader, bool aKillItems = true, MenuHook* aHook = 0);
		virtual											~Winterface						();
		virtual bool									DrawRight						();
		virtual bool									Input							();
		virtual bool									Draw							();
		virtual bool									DrawLeft						();
		virtual std::string								GetHeader						();
		
	protected:
		bool											KillItems;
		std::vector<ListItem*>							SideItems;

		char*											Header;
		
		static bool										HideRight;
};


#endif