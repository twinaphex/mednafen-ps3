#ifndef SYSTEM__WINTERFACEMULTILIST_H
#define SYSTEM__WINTERFACEMULTILIST_H

class													WinterfaceMultiList : public WinterfaceList
{
	public:
														WinterfaceMultiList				(std::string aHeader, bool aCanPage = true, bool aKillItems = true, MenuHook* aHook = 0);
		virtual											~WinterfaceMultiList			();
		virtual bool									Input							();
		

	public:		//Inlines
		void											SetCategory						(std::string aCategory)
		{
			if(aCategory != CurrentCategory)
			{
				Selected = 0;
				CurrentCategory = aCategory;
				
				Items = Categories[CurrentCategory];
			}
		}


	protected:
		bool											MultiKillItems;
	
		std::map<std::string, std::vector<ListItem*> >	Categories;
		std::string										CurrentCategory;
};

#endif