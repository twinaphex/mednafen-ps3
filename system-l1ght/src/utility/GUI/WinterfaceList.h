#ifndef SYSTEM__WINTERFACELIST_H
#define SYSTEM__WINTERFACELIST_H

class													WinterfaceList : public Winterface
{
	public:
														WinterfaceList					(const std::string& aHeader, bool aCanPage = true, bool aKillItems = true, MenuHook* aHook = 0);
		virtual bool									Input							();
		virtual bool									DrawLeft						();

	public: //Inlines
		virtual											~WinterfaceList					()
		{
			if(KillItems)
			{
				for(std::vector<ListItem*>::iterator i = Items.begin(); i != Items.end(); i ++)
				{
					delete *i;
				}
			}
		}

		bool											WasCanceled						()
		{
			return Canceled;
		}

		ListItem*										GetSelected						()
		{
			return Items[Selected];
		}
		
	protected:
		std::vector<ListItem*>							Items;
	
		int32_t											Selected;
		bool											Canceled;
		
		uint32_t										LinesDrawn;
		bool											CanPage;
};

#endif