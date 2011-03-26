#ifndef SYSTEM__SUMMERFACE_LIST_H
#define SYSTEM__SUMMERFACE_LIST_H

class													ListView
{
	public:
		virtual											~ListView						(){}
	
		virtual bool									Input							() = 0;
		virtual bool									Draw							() = 0;
};

class													SummerfaceList;
class													AnchoredListView : public ListView
{
	public:
														AnchoredListView				(SummerfaceList* aList); //External
														~AnchoredListView				() {};
														
		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected); //External
														
		virtual bool									Input							(); //External
		virtual bool									Draw							(); //External

	protected:
		SummerfaceList*									List;
		uint32_t										FirstLine;
		uint32_t										LinesDrawn;
};

class													GridListView : public ListView
{
	public:
														GridListView					(SummerfaceList* aList, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false); //External
		virtual											~GridListView					() {};
		
		virtual bool									DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected); //External
		
		virtual bool									Input							(); //External
		virtual bool									Draw							(); //External
		
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
	public:
														SummerfaceList					(const Area& aRegion); //External
		virtual											~SummerfaceList					(); //External

		virtual bool									Draw							() {return View->Draw();};
		virtual bool									Input							() {return View->Draw();};


		uint32_t										GetSelection					() const {return SelectedIndex;};
		void											SetSelection					(uint32_t aIndex); //External
		void											SetSelection					(const std::string& aText); //External

		SummerfaceItem*									GetSelected						() const {return (SelectedIndex < Items.size()) ? Items[SelectedIndex] : 0;};
		SummerfaceItem*									GetItem							(uint32_t aIndex) {return (aIndex < Items.size()) ? Items[aIndex] : 0;};

		virtual void									AddItem							(SummerfaceItem* aItem); //External
		virtual void									ClearItems						(); //External

		uint32_t										GetItemCount					() const {return Items.size();}
		bool											WasCanceled						() const {return Canceled;};
		void											SetCanceled						(bool aCanceled) {Canceled = aCanceled;};

		void											SetFont							(Font* aFont); //External
		Font*											GetFont							() const {return LabelFont;}

		void											SetView							(ListView* aView); //External
		ListView*										GetView							() const {return View;}

		void											Sort							(bool (*aCallback)(SummerfaceItem*, SummerfaceItem*) = 0); //External

	private:
		uint32_t										SelectedIndex;
		bool											Canceled;

		Font*											LabelFont;
		
		ListView*										View;
		std::vector<SummerfaceItem*>					Items;
};

#endif

