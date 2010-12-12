#ifndef SYSTEM__WINTERFACE_H
#define SYSTEM__WINTERFACE_H

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