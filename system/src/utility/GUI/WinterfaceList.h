#ifndef SYSTEM__WINTERFACELIST_H
#define SYSTEM__WINTERFACELIST_H

class													WinterfaceList : public Winterface
{
	public:
														WinterfaceList					(const std::string& aHeader, bool aCanPage = true, bool aKillItems = true, MenuHook* aHook = 0);
		virtual											~WinterfaceList					();

		virtual bool									Input							();
		virtual bool									DrawLeft						();

		uint32_t										GetItemCount					();
		void											SetSelection					(uint32_t aIndex);

		bool											WasCanceled						();
		ListItem*										GetSelected						();
		
	protected:
		std::vector<ListItem*>							Items;
	
		int32_t											Selected;
		bool											Canceled;
		
		uint32_t										LinesDrawn;
		bool											CanPage;
};

#endif

