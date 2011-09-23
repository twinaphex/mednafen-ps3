#pragma once

#include "Anchored.h"

template <typename T>
class											GroupListView : public AnchoredListView<T>
{
	typedef T*									Item_Ptr;

	public:
												GroupListView					(const Area& aRegion); //External
		virtual									~GroupListView					() {};
		
		virtual uint32_t						DrawHeader						(const std::string& aHeader, uint32_t aX, uint32_t aY);
		
		virtual bool							Input							(uint32_t aButton); //External
		virtual bool							Draw							(); //External

	protected:
		uint32_t								LinesDrawn;
		Color									HeaderColor;
};


template <typename T>
											GroupListView<T>::GroupListView				(const Area& aRegion) :
	AnchoredListView<T>(aRegion, true, false),
	HeaderColor("text", Colors::black)
{
}

template <typename T>
uint32_t									GroupListView<T>::DrawHeader				(const std::string& aHeader, uint32_t aX, uint32_t aY)
{
	//TODO: Make and use color
	this->GetFont()->PutString(aHeader.c_str(), aX, aY, HeaderColor, true);
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


