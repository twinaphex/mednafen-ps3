#ifndef SYSTEM__SUMMERFACE_LIST_H
#define SYSTEM__SUMMERFACE_LIST_H

class													ListModel
{
	public:
		virtual											~ListModel						(){}
	
		virtual bool									Input							() = 0;
		virtual bool									Draw							() = 0;
};

class													SummerfaceList;
class													LineListModel : public ListModel
{
	public:
														LineListModel					(SummerfaceList* aList);
														~LineListModel					();
														
		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected);
														
		virtual bool									Input							();
		virtual bool									Draw							();

	protected:
		SummerfaceList*									List;
		uint32_t										FirstLine;
		uint32_t										LinesDrawn;
};

class													GridListModel : public ListModel
{
	public:
														GridListModel					(SummerfaceList* aList, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false);
		virtual											~GridListModel					();
		
		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected);
		
		virtual bool									Input							();
		virtual bool									Draw							();
		
	protected:
		SummerfaceList*									List;

		uint32_t										Width;
		uint32_t										Height;
		
		int32_t											FirstItem;

		bool											DrawHeader;
		bool											RefreshHeader;
		bool											DrawLabels;
};

class													SummerfaceList : public SummerfaceWindow
{
	friend class										ListModel;
	friend class										LineListModel;
	friend class										GridListModel;

	public:
														SummerfaceList					(const Area& aRegion);
		virtual											~SummerfaceList					();

		virtual bool									Draw							();
		virtual bool									Input							();

		virtual void									AddItem							(SummerfaceItem* aItem);
		virtual void									ClearItems						();

		SummerfaceItem*									GetSelected						();

		uint32_t										GetItemCount					();
		void											SetSelection					(uint32_t aIndex);
		void											SetSelection					(const std::string& aText);

		bool											WasCanceled						();

		void											SetFont							(Font* aFont);
		void											SetModel						(ListModel* aModel);

		void											Sort							(bool (*aCallback)(SummerfaceItem*, SummerfaceItem*) = 0);

	protected:
		std::vector<SummerfaceItem*>					Items;
		uint32_t										SelectedIndex;
		bool											Canceled;

		Font*											LabelFont;
		
		ListModel*										Model;
};

#endif

