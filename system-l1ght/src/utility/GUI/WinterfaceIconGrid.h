#ifndef SYSTEM__ICONGRID_H
#define SYSTEM__ICONGRID_H

class								WinterfaceIconGrid : public Winterface
{
	public:
									WinterfaceIconGrid						(const std::string& aHeader, uint32_t aWidth, uint32_t aHeight, bool aKillItems);
		virtual						~WinterfaceIconGrid						();

		bool						Input									();
		bool						DrawLeft								();
	
		bool						WasCanceled								();
	
	protected:
		std::vector<GridItem*>		Items;
		
		uint32_t					Width;
		uint32_t					Height;

		bool						Canceled;

		int32_t						XSelection;
		int32_t						YSelection;
		int32_t						SelectedIndex;
};

#endif
