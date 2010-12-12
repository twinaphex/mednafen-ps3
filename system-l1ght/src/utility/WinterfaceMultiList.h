#ifndef SYSTEM__WINTERFACEMULTILIST_H
#define SYSTEM__WINTERFACEMULTILIST_H

class													WinterfaceMultiList : public WinterfaceList
{
	public:
														WinterfaceMultiList				(std::string aHeader, bool aCanPage = true, bool aKillItems = true, MenuHook* aHook = 0);
		virtual											~WinterfaceMultiList			();

		virtual bool									Input							();
		
		void											SetCategory						(std::string aCategory);

	protected:
		bool											MultiKillItems;
	
		std::map<std::string, std::vector<ListItem*> >	Categories;
		std::string										CurrentCategory;
};

#endif