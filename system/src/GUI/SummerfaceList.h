#pragma once

#include "Colors.h"

///Abstract class for displaying a list of items. Not complete until inherited by a view.
///@tparam T Type of item stored in the list.
template <typename T>
class													SummerfaceList : public SummerfaceWindow, public SummerfaceCancelable
{
	typedef T*											Item_Ptr;						///<The type of item in the list.

	public:
		///Create a new SummerfaceList.
		///@param aRegion Area in % of the screen covered by the list.
														SummerfaceList					(const Area& aRegion) :
			SummerfaceWindow(aRegion),
			SelectedIndex(0),
			LabelFont(FontManager::GetBigFont())
		{
		}

		///Destructor.
		virtual											~SummerfaceList					()
		{
			ClearItems();
		}

		///Return the index of the currently selected item.
		//It is an error if the index is not currently valid, it is the responsibility of the caller to ensure the list is not empty before calling.
		///@return The index of the currently selected item. 
		uint32_t										GetSelection					() const
		{
			assert(Items.size() != 0 && SelectedIndex < Items.size());
			return SelectedIndex;
		}

		///Return a pointer to the currently selected item.
		///It is an error if the list selection is not currently valid, it is the responsibility of the caller to ensure the list is not empty before calling.
		Item_Ptr										GetSelected						() const
		{
			assert(Items.size() != 0 && SelectedIndex < Items.size());
			return Items[SelectedIndex];
		}

		///Return a pointer to the item at a given index.
		//It is an error if aIndex is not currently valid.
		Item_Ptr										GetItem							(uint32_t aIndex)
		{
			assert(Items.size() != 0 && aIndex < Items.size());
			return Items[aIndex];
		}

		///Set the lists selection to the specified index. If aIndex is invalid the index is set to zero.
		///@param aIndex Item index in the list to select.
		void											SetSelection					(uint32_t aIndex)
		{
			SelectedIndex = (aIndex < Items.size()) ? aIndex : 0;
		}

		///Set the lists selection to the item with the specified name. If no item is invalid the index is set
		///is found the selection is set to zero. If more than one item shares the same name the first will be
		///selected.
		///@param aName Name of the item to select.
		void											SetSelection					(const std::string& aText)
		{
			for(int i = 0; i != Items.size(); i ++)
			{
				if(Items[i]->GetText() == aText)
				{
					SelectedIndex = i;
					return;
				}
			}

			SelectedIndex = 0;
		}

		///Add a new item to the list. It is an error if the item is null.
		///@param aItem Item to add.
		virtual void									AddItem							(Item_Ptr aItem)
		{
			assert(aItem);
			Items.push_back(aItem);
		}

		///Remove all items from the list.
		virtual void									ClearItems						()
		{
			for(int i = 0; i != Items.size(); i ++)
			{
				delete Items[i];
			}

			Items.clear();
			SelectedIndex = 0;
		}

		///Get the number of items currently in the list.
		///@return The number of items in the list.
		uint32_t										GetItemCount					() const
		{
			return Items.size();
		}

		void											SetFont							(Font* aFont)
		{
			assert(aFont);
			LabelFont = aFont;
		}

		Font*											GetFont							() const
		{
			return LabelFont;
		}

		static bool										AlphaSort						(Item_Ptr a, Item_Ptr b)
		{
			return a->GetText() < b->GetText();
		}

		void											Sort							(bool (*aCallback)(Item_Ptr, Item_Ptr) = 0)
		{
			std::sort(Items.begin(), Items.end(), aCallback ? aCallback : AlphaSort);
		}

	private:
		uint32_t										SelectedIndex;
		Font*											LabelFont;
		std::vector<Item_Ptr>							Items;
};


///Base class to provide input and output for SummerfaceList.
///@tparam The type of a ListItem.
template <typename T>
class													ListView : public SummerfaceList<T>
{
	public:
		///Create a new, empty, ListView object.
														ListView						(const Area& aRegion) : SummerfaceList<T>(aRegion)	{}

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
};

#include "ListViews/Anchored.h"
#include "ListViews/Grid.h"
#include "ListViews/Group.h"

