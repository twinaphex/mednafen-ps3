#pragma once

///Abstract class for displaying a list of items. Not complete until inherited by a view.
///@tparam T Type of item stored in the list.
template <typename T>
class													SummerfaceList : public SummerfaceWindow, public SummerfaceCancelable
{
	typedef smartptr::shared_ptr<T>						Item_Ptr;						///<The type of item in the list.

	public:
		///Create a new SummerfaceList.
		///@param aRegion Area in % of the screen covered by the list.
														SummerfaceList					(const Area& aRegion) :
			SummerfaceWindow(aRegion),
			SelectedIndex(0),
			LabelFont(FontManager::GetBigFont())
		{
		}

		///Empty virtual destructor.
		virtual											~SummerfaceList					()
		{
		}

		///Return the index of the currently selected item, it is safe to assume the returned item is always a valid item.
		//It is an error if the index is not currently valid, it is the responsibility of the caller to ensure the list is not empty before calling.
		///@return The index of the currently selected item. 
		uint32_t										GetSelection					() const
		{
			assert(Items.size() != 0 && SelectedIndex < Items.size());
			return SelectedIndex;
		}

		///Return a pointer to the currently selected item, it is safe to assume the returned item is always a valid item.
		///It is an error if the list selection is not currently valid, it is the responsibility of the caller to ensure the list is not empty before calling.
		Item_Ptr										GetSelected						() const
		{
			assert(Items.size() != 0 && SelectedIndex < Items.size());
			return Items[SelectedIndex];
		}

		///Return a pointer to the item at a given index, it is safe to assume the returned item is always a valid item.
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

template <typename T>
class													AnchoredListView : public ListView<T>
{
	typedef smartptr::shared_ptr<T>						Item_Ptr;

	public:
														AnchoredListView				(const Area& aRegion, bool aAnchored = true, bool aWrap = false); //External
														~AnchoredListView				() {};
														
		virtual bool									DrawItem						(Item_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected); //External
														
		virtual bool									Input							(uint32_t aButton); //External
		virtual bool									Draw							(); //External

	protected:
		int32_t											FirstLine;
		uint32_t										LinesDrawn;

		bool											Anchored;
		bool											Wrap;
};

template <typename T>
class													GridListView : public ListView<T>
{
	typedef smartptr::shared_ptr<T>						Item_Ptr;

	public:
														GridListView					(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false); //External
		virtual											~GridListView					() {};
		
		virtual bool									DrawItem						(Item_Ptr aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected); //External
		
		virtual bool									Input							(uint32_t aButton); //External
		virtual bool									Draw							(); //External
		
	protected:
		uint32_t										Width;
		uint32_t										Height;
		
		bool											DrawHeader;
		bool											RefreshHeader;
		bool											DrawLabels;
};

template <typename T>
class													GroupListView : public AnchoredListView<T>
{
	typedef smartptr::shared_ptr<T>						Item_Ptr;

	public:
														GroupListView					(const Area& aRegion); //External
		virtual											~GroupListView					() {};
		
		virtual uint32_t								DrawHeader						(const std::string& aHeader, uint32_t aX, uint32_t aY);
		
		virtual bool									Input							(uint32_t aButton); //External
		virtual bool									Draw							(); //External

	protected:
		uint32_t										LinesDrawn;
};


///Implement
template <typename T>
											GridListView<T>::GridListView						(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader, bool aLabels) :
	ListView<T>(aRegion),
	Width(aWidth),
	Height(aHeight),
	DrawHeader(aHeader),
	RefreshHeader(true),
	DrawLabels(aLabels)
{
	ErrorCheck(Width != 0 && Height != 0 && Width <= 16 && Height <= 16, "GridListView::GridListView: Grid dimensions out of range. [X: %d, Y: %d]", Width, Height);
}

template <typename T>
bool										GridListView<T>::Input								(uint32_t aButton)
{
	uint32_t oldIndex = this->GetSelection();
	int32_t XSelection = oldIndex % Width;
	int32_t YSelection = oldIndex / Width;

	XSelection += (aButton == ES_BUTTON_RIGHT) ? 1 : 0;
	XSelection -= (aButton == ES_BUTTON_LEFT) ? 1 : 0;
	XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

	YSelection += (aButton == ES_BUTTON_DOWN) ? 1 : 0;
	YSelection -= (aButton == ES_BUTTON_UP) ? 1 : 0;
	YSelection = Utility::Clamp(YSelection, 0, (int32_t)Height - 1);

	if(YSelection * Width + XSelection != oldIndex)
	{
		this->SetSelection(YSelection * Width + XSelection);
	}

	if(DrawHeader && (oldIndex != this->GetSelection() || RefreshHeader))
	{
		RefreshHeader = false;
		SetHeader(this->GetSelected()->GetText());
	}

	this->SetCanceled(aButton == ES_BUTTON_CANCEL);

	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}

template <typename T>
bool										GridListView<T>::DrawItem							(Item_Ptr aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image && aWidth && aHeight)
	{
		Area ImageArea(0, 0, image->GetWidth(), image->GetHeight());
		aHeight -= DrawLabels ? this->GetFont()->GetHeight() : 0;

		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, ImageArea.Width, ImageArea.Height);
	
		ESVideo::PlaceTexture(image, Area(x, y, w, h), ImageArea, 0xFFFFFFFF);
	}

	if(DrawLabels)
	{
		this->GetFont()->PutString(aItem->GetText().c_str(), aX, aY + aHeight, aItem->GetNormalColor(), true);
	}

	if(aSelected)
	{
		ESVideo::FillRectangle(Area(aX, aY, aWidth - 2, aHeight - 2), Colors::SpecialBackGround);
	}

	return false;
}

template <typename T>
bool										GridListView<T>::Draw								()
{
	uint32_t iconWidth = ESVideo::GetClip().Width / Width - 4;
	uint32_t iconHeight = ESVideo::GetClip().Height / Height - 4;	

	for(int i = 0; i != Width * Height; i ++)
	{
		if(i >= this->GetItemCount())
		{
			break;
		}
	
		DrawItem(this->GetItem(i), (i % Width) * iconWidth + 4, (i / Width) * iconHeight + 4, iconWidth, iconHeight, this->GetSelection() == i);
	}
	
	return false;
}

template <typename T>
											AnchoredListView<T>::AnchoredListView				(const Area& aRegion, bool aAnchored, bool aWrap) :
	ListView<T>(aRegion),
	FirstLine(0),
	LinesDrawn(0),
	Anchored(aAnchored),
	Wrap(aWrap)
{
}

template <typename T>
bool										AnchoredListView<T>::DrawItem						(Item_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image)
	{
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(this->GetFont()->GetHeight() - 4) / (double)image->GetHeight()));

		ESVideo::PlaceTexture(image, Area(aX, aY + 2, width, this->GetFont()->GetHeight() - 4), Area(0, 0, image->GetWidth(), image->GetHeight()), 0xFFFFFFFF);
		aX += width;
	}

	//Get the text
	const std::string text = aItem->GetText();

	//No tab, just draw
	if(aItem->GetText().find("\t") == std::string::npos)
	{
		this->GetFont()->PutString(aItem->GetText().c_str(), aX, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor(), true);
	}
	//Draw parts after tab right aligned
	else
	{
		//Put first part
		uint32_t count = this->GetFont()->PutString(text.c_str(), text.find("\t"), aX, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor(), true);

		//Put second part
		std::string second = text.substr(count + 1);
		int32_t x = ESVideo::GetClip().Width - this->GetFont()->MeasureString(second.c_str()) - 10;
		this->GetFont()->PutString(second.c_str(), x, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor(), true);
	}

	return false;
}

template <typename T>
bool										AnchoredListView<T>::Draw							()
{
	if(this->GetItemCount() != 0)
	{
		uint32_t itemheight = this->GetFont()->GetHeight();
		LinesDrawn = ESVideo::GetClip().Height / itemheight;
	
		uint32_t online = 0;
		int onitem = Anchored ? this->GetSelection() - LinesDrawn / 2 : FirstLine;

		for(int i = 0; i != LinesDrawn + 2; i ++, onitem ++, online ++)
		{
			if(onitem < 0 || onitem >= this->GetItemCount())
			{
				continue;
			}

			DrawItem(this->GetItem(onitem), 16, (online * itemheight), onitem == this->GetSelection());
		}
	}

	return false;
}

template <typename T>
bool										AnchoredListView<T>::Input							(uint32_t aButton)
{
	int32_t oldIndex = this->GetSelection();
	if(this->GetItemCount() != 0)
	{
		oldIndex += (aButton == ES_BUTTON_DOWN) ? 1 : 0;
		oldIndex -= (aButton == ES_BUTTON_UP) ? 1 : 0;
		oldIndex += (aButton == ES_BUTTON_RIGHT) ? LinesDrawn : 0;
		oldIndex -= (aButton == ES_BUTTON_LEFT) ? LinesDrawn : 0;

		if(Wrap)
		{
			oldIndex = (oldIndex < 0) ? this->GetItemCount() - 1 : oldIndex;
			oldIndex = (oldIndex >= this->GetItemCount()) ? 0 : oldIndex;
		}

		oldIndex = Utility::Clamp(oldIndex, 0, this->GetItemCount() - 1);
		this->SetSelection(oldIndex);

		if(this->GetItemCount() != 1 && !Anchored)
		{
			if(oldIndex >= FirstLine + LinesDrawn)
			{
				FirstLine = oldIndex - LinesDrawn;
			}

			if(oldIndex < FirstLine)
			{
				FirstLine = oldIndex;
			}
		}
	}

	this->SetCanceled(aButton == ES_BUTTON_CANCEL);
	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}

template <typename T>
											GroupListView<T>::GroupListView				(const Area& aRegion) :
	AnchoredListView<T>(aRegion, true, false)
{
}

template <typename T>
uint32_t									GroupListView<T>::DrawHeader				(const std::string& aHeader, uint32_t aX, uint32_t aY)
{
	//TODO: Make and use color
	this->GetFont()->PutString(aHeader.c_str(), aX, aY, Colors::Normal, true);
	ESVideo::FillRectangle(Area(aX, aY + this->GetFont()->GetHeight() + 2, ESVideo::GetClip().Width - (aX * 2), 1), 0xFFFFFFFF);

	return this->GetFont()->GetHeight() + 3;
}

template <typename T>
bool										GroupListView<T>::Draw						()
{
	if(this->GetItemCount() != 0)
	{
		uint32_t itemheight = this->GetFont()->GetHeight() + 3;
		LinesDrawn = ESVideo::GetClip().Height / itemheight;

		uint32_t online = 0;
		int onitem = this->GetSelection() - LinesDrawn / 2;
		std::string lastGroup = "";

		for(int i = 0; i != LinesDrawn + 2; i ++, online ++)
		{
			if(onitem < 0 || onitem >= this->GetItemCount())
			{
				onitem++;
				continue;
			}

			if(lastGroup != this->GetItem(onitem)->GetGroup())
			{
				lastGroup = this->GetItem(onitem)->GetGroup();
				DrawHeader(lastGroup, 16, online * itemheight);
			}
			else
			{
				DrawItem(this->GetItem(onitem), 16, online * itemheight, onitem == this->GetSelection());
				onitem ++;
			}
		}
	}

	return false;
}

template <typename T>
bool										GroupListView<T>::Input							(uint32_t aButton)
{
	int32_t oldIndex = this->GetSelection();
	if(this->GetItemCount() != 0)
	{
		oldIndex += (aButton == ES_BUTTON_DOWN) ? 1 : 0;
		oldIndex -= (aButton == ES_BUTTON_UP) ? 1 : 0;

		oldIndex = Utility::Clamp(oldIndex, 0, this->GetItemCount() - 1);
		this->SetSelection(oldIndex);
	}

	this->SetCanceled(aButton == ES_BUTTON_CANCEL);
	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}


