#ifndef SYSTEM__WINTERFACE_H
#define SYSTEM__WINTERFACE_H

class													Winterface : public Menu
{
	public:
														Winterface						(std::string aHeader, bool aKillItems = true, MenuHook* aHook = 0);
		virtual bool									Draw							();
		virtual bool									DrawRight						();
		
	public:		//Inlines
		virtual											~Winterface						()
		{
			if(KillItems)
			{
				for(std::vector<ListItem*>::iterator iter = SideItems.begin(); iter != SideItems.end(); iter ++)
				{
					delete (*iter);
				}
			}
		}
	
		virtual bool									DrawRight						()
		{
			//TODO: Assume all items are the same size as item[0]
			for(int i = 0; i != SideItems.size(); i ++)
			{
				SideItems[i]->Draw(16, i * SideItems[0]->GetHeight(), false);
			}
		
			return false;
		}
	
		virtual bool									Input							()
		{
			if(PS3Input::ButtonDown(0, PS3_BUTTON_SELECT))
			{
				HideRight = !HideRight;
			}
			
			return false;
		}
	
		virtual bool									DrawLeft						()
		{
			return false;
		}
		
		virtual std::string								GetHeader						()
		{
			return Header;
		}
	
		
	protected:
		bool											KillItems;
		std::vector<ListItem*>							SideItems;

		std::string										Header;
		
		static bool										HideRight;
};


#endif