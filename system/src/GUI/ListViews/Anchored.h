#pragma once

#include "../SummerfaceList.h"

template <typename T>
class										AnchoredListView : public ListView<T>
{
	typedef T*								Item_Ptr;

	public:
											AnchoredListView				(const Area& aRegion, bool aAnchored = true, bool aWrap = false); //External
											~AnchoredListView				() {};
														
		virtual bool						DrawItem						(Item_Ptr aItem, uint32_t aX, uint32_t aY, bool aSelected); //External
														
		virtual bool						Input							(uint32_t aButton); //External
		virtual bool						Draw							(); //External

	protected:
		int32_t								FirstLine;
		uint32_t							LinesDrawn;

		bool								Anchored;
		bool								Wrap;
};

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

