#include <ps3_system.h>
#include "SummerfaceList.h"

namespace
{
	bool									AlphaSort											(SummerfaceItem* a, SummerfaceItem* b)
	{
		return a->GetText() < b->GetText();
	}
}


											SummerfaceList::SummerfaceList						(const Area& aRegion) : SummerfaceWindow(aRegion)
{
	SelectedIndex = 0;

	Canceled = false;

	LabelFont = FontManager::GetBigFont();
}

											SummerfaceList::~SummerfaceList						()
{
	for(std::vector<SummerfaceItem*>::iterator iter = Items.begin(); iter != Items.end(); iter ++)
	{
		delete (*iter);
	}
}

void										SummerfaceList::AddItem								(SummerfaceItem* aItem)
{
	Items.push_back(aItem);
}

SummerfaceItem*								SummerfaceList::GetSelected							()
{
	if(SelectedIndex < Items.size())
	{
		return Items[SelectedIndex];
	}
	else
	{
		return 0;
	}
}

uint32_t									SummerfaceList::GetItemCount						()
{
	return Items.size();
}

void										SummerfaceList::SetSelection						(uint32_t aIndex)
{
	if(aIndex >= Items.size())
	{
		throw ESException("WinterfaceList: Item index out of range [Item %d, Total %d]", aIndex, Items.size());
	}

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

bool										SummerfaceList::WasCanceled							()
{
	return Canceled;
}

void										SummerfaceList::SetFont								(Font* aFont)
{
	LabelFont = aFont;
}

void										SummerfaceList::Sort								(bool (*aCallback)(SummerfaceItem*, SummerfaceItem*))
{
	std::sort(Items.begin(), Items.end(), aCallback ? aCallback : AlphaSort);
}


											SummerfaceGrid::SummerfaceGrid						(const Area& aRegion, uint32_t aWidth, uint32_t aHeight, bool aHeader, bool aLabels) : SummerfaceList(aRegion)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 16 || aHeight > 16)
	{
		throw ESException("Summerface grid dimensions out of range. [X: %d, Y: %d]", aWidth, aHeight);
	}

	Width = aWidth;
	Height = aHeight;
	
	SetDrawMode(aHeader, aLabels);
}

											SummerfaceGrid::~SummerfaceGrid						()
{
}

bool										SummerfaceGrid::Input								()
{
	uint32_t XSelection = SelectedIndex % Width;
	uint32_t YSelection = SelectedIndex / Width;

	XSelection += es_input->ButtonDown(0, ES_BUTTON_RIGHT) ? 1 : 0;
	XSelection -= es_input->ButtonDown(0, ES_BUTTON_LEFT) ? 1 : 0;
	XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

	YSelection += es_input->ButtonDown(0, ES_BUTTON_DOWN) ? 1 : 0;
	YSelection -= es_input->ButtonDown(0, ES_BUTTON_UP) ? 1 : 0;
	YSelection = Utility::Clamp(YSelection, 0, (int32_t)Height - 1);

	SelectedIndex = YSelection * Width + XSelection;

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		Canceled = true;
		return true;
	}

	if(GetInputConduit() && SelectedIndex < Items.size())
	{
		return GetInputConduit()->HandleInput(GetInterface(), GetName()); 
	}

	return false;
}

bool										SummerfaceGrid::DrawItem							(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());
	Area ImageArea(0, 0, 0, 0);

	if(image && aWidth && aHeight)
	{
		if(ImageArea.Width == 0)
		{
			ImageArea = Area(0, 0, image->GetWidth(), image->GetHeight());
		}

		if(DrawLabels)
		{
			aHeight -= LabelFont->GetHeight();
		}

		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, ImageArea.Width, ImageArea.Height);
		
		es_video->PlaceTexture(image, x, y, w, h, 0xFFFFFFFF, &ImageArea);

		if(DrawLabels)
		{
			LabelFont->PutString(aItem->GetText().c_str(), aX, aY + aHeight, aItem->GetNormalColor());
		}
	}
	
	if(aSelected)
	{
		es_video->FillRectangle(Area(aX, aY, aWidth - 2, aHeight - 2), Colors::SpecialBackGround);
	}

	return false;
}

bool										SummerfaceGrid::Draw								()
{
	uint32_t iconWidth = es_video->GetClip().Width / Width - 4;
	uint32_t iconHeight = es_video->GetClip().Height / Height - 4;	

	uint32_t XSelection = SelectedIndex % Width;
	uint32_t YSelection = SelectedIndex / Width;

	if(DrawHeader)
	{
		iconHeight = (es_video->GetClip().Height - LabelFont->GetHeight()) / Height - 4;
		if(SelectedIndex < Items.size())
		{
			LabelFont->PutString(Items[SelectedIndex]->GetText().c_str(), 0, 0, Colors::Normal);
		}
	}

	for(int i = 0; i != Height; i ++)
	{
		for(int j = 0; j != Width; j ++)
		{
			if(i * Width + j >= Items.size())
			{
				break;
			}
		
			DrawItem(Items[i * Width + j], j * iconWidth + 4, i * iconHeight + 4 + (DrawHeader ? LabelFont->GetHeight() : 0), iconWidth, iconHeight, j == XSelection && i == YSelection);
		}
	}
	
	return false;
}

void										SummerfaceGrid::SetDrawMode							(bool aHeader, bool aLabels)
{
	DrawHeader = aHeader;
	DrawLabels = aLabels;
}

											SummerfaceLineList::SummerfaceLineList				(const Area& aRegion) : SummerfaceList(aRegion)
{

}

											SummerfaceLineList::~SummerfaceLineList				()
{
}

bool										SummerfaceLineList::DrawItem						(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image)
	{
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(LabelFont->GetHeight() - 4) / (double)image->GetHeight()));

		es_video->PlaceTexture(image, aX, aY + 2, width, LabelFont->GetHeight() - 4);
		aX += width;
	}

	LabelFont->PutString(aItem->GetText().c_str(), aX, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());

	return false;
}

bool										SummerfaceLineList::Draw							()
{
	if(Items.size() != 0)
	{
		uint32_t itemheight = LabelFont->GetHeight();
		LinesDrawn = es_video->GetClip().Height / itemheight;
		
		//TODO: Fix it to draw one or two line lists!
		if(LinesDrawn < 3)
		{
			return true;
		}
		
		uint32_t online = 0;
	
		for(int i = SelectedIndex - LinesDrawn / 2; i != SelectedIndex + LinesDrawn / 2; i ++)
		{
			if(i < 0)
			{
				online ++;
				continue;
			}
		
			if(i >= Items.size())
			{
				break;
			}
			

			if(DrawItem(Items[i], 16, (online * itemheight), i == SelectedIndex))
			{
				return true;
			}	

			online ++;
		}
	}

	return false;
}

bool										SummerfaceLineList::Input							()
{
	if(Items.size() != 0)
	{
		SelectedIndex += (es_input->ButtonPressed(0, ES_BUTTON_DOWN) ? 1 : 0);
		SelectedIndex -= (es_input->ButtonPressed(0, ES_BUTTON_UP) ? 1 : 0);
		SelectedIndex += (es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? LinesDrawn : 0);
		SelectedIndex -= (es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? LinesDrawn : 0);
	
		SelectedIndex = Utility::Clamp(SelectedIndex, 0, Items.size() - 1);
	}

	if(GetInputConduit())
	{
		return GetInputConduit()->HandleInput(GetInterface(), GetName()); 
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		Canceled = false;
		return true;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		Canceled = true;
		return true;
	}
	
	return false;
}



