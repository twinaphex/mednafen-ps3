#pragma once

#include "../SummerfaceList.h"

template <typename T>
class										GridListView : public ListView<T>
{
	typedef T*								Item_Ptr;

	public:
											GridListView					(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader = true, bool aLabels = false); //External
		virtual								~GridListView					() {};
		
		virtual bool						DrawItem						(Item_Ptr aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected); //External
		
		virtual bool						Input							(uint32_t aButton); //External
		virtual bool						Draw							(); //External
		
	protected:
		uint32_t							Width;
		uint32_t							Height;
		
		bool								DrawHeader;
		bool								RefreshHeader;
		bool								DrawLabels;

		Color								SelectedBackColor;
};

///Implement
template <typename T>
											GridListView<T>::GridListView						(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader, bool aLabels) :
	ListView<T>(aRegion),
	Width(aWidth),
	Height(aHeight),
	DrawHeader(aHeader),
	RefreshHeader(true),
	DrawLabels(aLabels),
	SelectedBackColor("selectedbackgroud", Colors::Alpha(Colors::gray, 0x40))
{
	assert(Width != 0 && Height != 0 && Width <= 16 && Height <= 16);
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
		ESVideo::FillRectangle(Area(aX, aY, aWidth - 2, aHeight - 2), SelectedBackColor);
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

