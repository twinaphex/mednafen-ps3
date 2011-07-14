#include <es_system.h>
#include "SummerfaceList.h"

namespace
{
	bool									AlphaSort											(SummerfaceItem_Ptr a, SummerfaceItem_Ptr b)
	{
		return a->GetText() < b->GetText();
	}
}


											SummerfaceList::SummerfaceList						(const Area& aRegion) : 
	SummerfaceWindow(aRegion),
	SelectedIndex(0),
	LabelFont(FontManager::GetBigFont()),
	View(smartptr::make_shared<ListView>())
{
}

void										SummerfaceList::SetSelection						(uint32_t aIndex)
{
	ErrorCheck(aIndex < Items.size(), "SummerfaceList::SetSelection: Item index out of range [Item %d, Total %d]", aIndex, Items.size());
	SelectedIndex = aIndex;
}

void										SummerfaceList::SetSelection						(const std::string& aText)
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

void										SummerfaceList::AddItem								(SummerfaceItem_Ptr aItem)
{
	ErrorCheck(std::find(Items.begin(), Items.end(), aItem) == Items.end(), "SummerfaceList::AddItem: Can't add the same item to the list twice");

	Items.push_back(aItem);
}

void										SummerfaceList::ClearItems							()
{
	Items.clear();
	SelectedIndex = 0;
}

void										SummerfaceList::SetFont								(Font* aFont)
{
	ErrorCheck(aFont, "SummerfaceList::SetFont: Font must not be null");

	LabelFont = aFont;
}

void										SummerfaceList::SetView								(ListView_Ptr aView)
{
	ErrorCheck(!!aView, "SummerfaceList::SetView: View must not be null");
	View = aView;
}

void										SummerfaceList::Sort								(bool (*aCallback)(SummerfaceItem_Ptr, SummerfaceItem_Ptr))
{
	std::sort(Items.begin(), Items.end(), aCallback ? aCallback : AlphaSort);
}


											GridListView::GridListView							(SummerfaceList_WeakPtr aList, uint32_t aWidth, uint32_t aHeight, bool aHeader, bool aLabels) :
	ListView(aList),
	Width(aWidth),
	Height(aHeight),
	DrawHeader(aHeader),
	RefreshHeader(true),
	DrawLabels(aLabels)
{
	ErrorCheck(Width != 0 && Height != 0 && Width <= 16 && Height <= 16, "GridListView::GridListView: Grid dimensions out of range. [X: %d, Y: %d]", Width, Height);
}


bool										GridListView::Input									(uint32_t aButton)
{
	SummerfaceList_Ptr List;
	if(TryGetList(List))
	{
		uint32_t oldIndex = List->GetSelection();
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
			List->SetSelection(YSelection * Width + XSelection);
		}

		if(DrawHeader && (oldIndex != List->GetSelection() || RefreshHeader))
		{
			RefreshHeader = false;
			List->SetHeader(List->GetSelected()->GetText());
		}

		List->SetCanceled(aButton == ES_BUTTON_CANCEL);
	}

	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}

bool										GridListView::DrawItem								(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image && aWidth && aHeight)
	{
		Area ImageArea(0, 0, image->GetWidth(), image->GetHeight());
		aHeight -= DrawLabels ? aList->GetFont()->GetHeight() : 0;

		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, ImageArea.Width, ImageArea.Height);
	
		ESVideo::PlaceTexture(image, Area(x, y, w, h), ImageArea, 0xFFFFFFFF);
	}

	if(DrawLabels)
	{
		aList->GetFont()->PutString(aItem->GetText().c_str(), aX, aY + aHeight, aItem->GetNormalColor(), true);
	}

	if(aSelected)
	{
		ESVideo::FillRectangle(Area(aX, aY, aWidth - 2, aHeight - 2), Colors::SpecialBackGround);
	}

	return false;
}

bool										GridListView::Draw									()
{
	SummerfaceList_Ptr List;
	if(TryGetList(List))
	{
		SummerfaceList_Ptr List = WeakList.lock();

		uint32_t iconWidth = ESVideo::GetClip().Width / Width - 4;
		uint32_t iconHeight = ESVideo::GetClip().Height / Height - 4;	

		for(int i = 0; i != Width * Height; i ++)
		{
			if(i >= List->GetItemCount())
			{
				break;
			}
		
			DrawItem(List, List->GetItem(i), (i % Width) * iconWidth + 4, (i / Width) * iconHeight + 4, iconWidth, iconHeight, List->GetSelection() == i);
		}
	}
	
	return false;
}

											AnchoredListView::AnchoredListView					(SummerfaceList_WeakPtr aList, bool aAnchored, bool aWrap) :
	ListView(aList),
	FirstLine(0),
	LinesDrawn(0),
	Anchored(aAnchored),
	Wrap(aWrap)
{
}

bool										AnchoredListView::DrawItem							(SummerfaceList_Ptr aList, SummerfaceItem_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image)
	{
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(aList->GetFont()->GetHeight() - 4) / (double)image->GetHeight()));

		ESVideo::PlaceTexture(image, Area(aX, aY + 2, width, aList->GetFont()->GetHeight() - 4), Area(0, 0, image->GetWidth(), image->GetHeight()), 0xFFFFFFFF);
		aX += width;
	}

	aList->GetFont()->PutString(aItem->GetText().c_str(), aX, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor(), true);

	return false;
}

bool										AnchoredListView::Draw								()
{
	SummerfaceList_Ptr List;
	if(TryGetList(List))
	{
		if(List->GetItemCount() != 0)
		{
			uint32_t itemheight = List->GetFont()->GetHeight();
			LinesDrawn = ESVideo::GetClip().Height / itemheight;
		
			uint32_t online = 0;
			int onitem = Anchored ? List->GetSelection() - LinesDrawn / 2 : FirstLine;

			for(int i = 0; i != LinesDrawn + 2; i ++, onitem ++, online ++)
			{
				if(onitem < 0 || onitem >= List->GetItemCount())
				{
					continue;
				}
	
				DrawItem(List, List->GetItem(onitem), 16, (online * itemheight), onitem == List->GetSelection());
			}
		}
	}

	return false;
}

bool										AnchoredListView::Input								(uint32_t aButton)
{
	SummerfaceList_Ptr List;
	if(TryGetList(List))
	{
		int32_t oldIndex = List->GetSelection();
		if(List->GetItemCount() != 0)
		{
			oldIndex += (aButton == ES_BUTTON_DOWN) ? 1 : 0;
			oldIndex -= (aButton == ES_BUTTON_UP) ? 1 : 0;
			oldIndex += (aButton == ES_BUTTON_RIGHT) ? LinesDrawn : 0;
			oldIndex -= (aButton == ES_BUTTON_LEFT) ? LinesDrawn : 0;
	
			if(Wrap)
			{
				oldIndex = (oldIndex < 0) ? List->GetItemCount() - 1 : oldIndex;
				oldIndex = (oldIndex >= List->GetItemCount()) ? 0 : oldIndex;
			}

			oldIndex = Utility::Clamp(oldIndex, 0, List->GetItemCount() - 1);
			List->SetSelection(oldIndex);

			if(List->GetItemCount() != 1 && !Anchored)
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

		List->SetCanceled(aButton == ES_BUTTON_CANCEL);
	}

	return aButton == ES_BUTTON_ACCEPT || aButton == ES_BUTTON_CANCEL;
}


