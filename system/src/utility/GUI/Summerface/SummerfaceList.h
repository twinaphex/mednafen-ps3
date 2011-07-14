#pragma once

class													SummerfaceList;

///Base class to provide input and output for SummerfaceList.
class													ListView
{
	public:
		///Create a new, empty, ListView object.
														ListView						()	{}

		///Create a new ListView object.
		///@param aList List the view should use for drawing.
														ListView						(SummerfaceList_WeakPtr aList) : WeakList(aList) {}

		///Empty virtual destructor for ListView.
		virtual											~ListView						() {}
	
		///Handle input for the list. This implementation does nothing.
		///@param aButton System button that was pressed.
		///@return True to end processing of the parent interface.
		virtual bool									Input							(uint32_t aButton)
		{
			return false;
		}

		///Handle drawing for the list. This implementation does nothing.
		///@param aButton System button that was pressed.
		///@return True to end processing of the parent interface.
		virtual bool									Draw							()
		{
			return false;
		}

		///Return a shared_ptr to the processed list, asserting if the list is invalid.
		///@return A shared_ptr to the processed list.
		SummerfaceList_Ptr								GetList							()
		{
			assert(!WeakList.expired());
			return WeakList.lock();
		}

		///Determine if the processed list is valid, and get a shared_ptr to it.
		///@param aResult [out] A reference that is set to the processed list. This value is not modified if the list is not valid.
		///@return True if the processed list is valid.
		bool											TryGetList						(SummerfaceList_Ptr& aResult)
		{
			if(!WeakList.expired())
			{
				aResult = WeakList.lock();
				return true;
			}

			return false;
		}

		///Determine if the processed list is valid.
		///@return True if the processed list is valid.
		bool											ListValid						()
		{
			return !WeakList.expired();
		}

	protected:
		SummerfaceList_WeakPtr							WeakList;						///<A weak_ptr to the processed list.
};

class													AnchoredListView : public ListView
{
	public:
														AnchoredListView				(SummerfaceList_WeakPtr aList, bool aAnchored = true, bool aWrap = false); //External
														~AnchoredListView				() {};
														
		virtual bool									DrawItem						(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected); //External
														
		virtual bool									Input							(uint32_t aButton); //External
		virtual bool									Draw							(); //External

	protected:
		int32_t											FirstLine;
		uint32_t										LinesDrawn;

		bool											Anchored;
		bool											Wrap;
};

class													GridListView : public ListView
{
	public:
														GridListView					(SummerfaceList_WeakPtr aList, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false); //External
		virtual											~GridListView					() {};
		
		virtual bool									DrawItem						(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected); //External
		
		virtual bool									Input							(uint32_t aButton); //External
		virtual bool									Draw							(); //External
		
	protected:
		uint32_t										Width;
		uint32_t										Height;
		
		bool											DrawHeader;
		bool											RefreshHeader;
		bool											DrawLabels;
};

class													SummerfaceList : public SummerfaceWindow, public SummerfaceCancelable
{
	public:
														SummerfaceList					(const Area& aRegion); //External
		virtual											~SummerfaceList					() {};

		virtual bool									Draw							() {return View->Draw();};
		virtual bool									Input							(uint32_t aButton) {return View->Input(aButton);};


		uint32_t										GetSelection					() const {return SelectedIndex;};
		void											SetSelection					(uint32_t aIndex); //External
		void											SetSelection					(const std::string& aText); //External

		SummerfaceItem_Ptr								GetSelected						() const {return (SelectedIndex < Items.size()) ? Items[SelectedIndex] : SummerfaceItem_Ptr();};
		SummerfaceItem_Ptr								GetItem							(uint32_t aIndex) {return (aIndex < Items.size()) ? Items[aIndex] : SummerfaceItem_Ptr();};

		virtual void									AddItem							(SummerfaceItem_Ptr aItem); //External
		virtual void									ClearItems						(); //External

		uint32_t										GetItemCount					() const {return Items.size();}

		void											SetFont							(Font* aFont); //External
		Font*											GetFont							() const {return LabelFont;}

		void											SetView							(ListView_Ptr aView); //External
		ListView_Ptr									GetView							() const {return View;}

		void											Sort							(bool (*aCallback)(SummerfaceItem_Ptr, SummerfaceItem_Ptr) = 0); //External

	private:
		uint32_t										SelectedIndex;

		Font*											LabelFont;
		
		ListView_Ptr									View;
		std::vector<SummerfaceItem_Ptr>					Items;
};

