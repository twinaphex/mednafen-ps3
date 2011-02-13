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
	
	Model = new LineListModel(this);
}

											SummerfaceList::~SummerfaceList						()
{
	for(std::vector<SummerfaceItem*>::iterator iter = Items.begin(); iter != Items.end(); iter ++)
	{
		delete (*iter);
	}
	
	delete Model;
}

bool										SummerfaceList::Draw								()
{
	return Model->Draw();
}

bool										SummerfaceList::Input								()
{
	return Model->Input();
}

void										SummerfaceList::AddItem								(SummerfaceItem* aItem)
{
	for(int i = 0; i != Items.size(); i ++)
	{
		if(Items[i] == aItem)
		{
			throw ESException("SummerfaceList::AddItem: Can't add the same item to the list twice");
		}
	}

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
		throw ESException("SummerfaceList::SetSelection: Item index out of range [Item %d, Total %d]", aIndex, Items.size());
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
	if(!aFont)
	{
		throw ESException("SummerfaceList::SetFont: Font must not be null");
	}

	LabelFont = aFont;
}

void										SummerfaceList::SetModel							(ListModel* aModel)
{
	if(!aModel)
	{
		throw ESException("SummerfaceList::SetModel: Model must not be null");
	}

	delete Model;
	Model = aModel;
}

void										SummerfaceList::Sort								(bool (*aCallback)(SummerfaceItem*, SummerfaceItem*))
{
	std::sort(Items.begin(), Items.end(), aCallback ? aCallback : AlphaSort);
}


											GridListModel::GridListModel						(SummerfaceList* aList, uint32_t aWidth, uint32_t aHeight, bool aHeader, bool aLabels)
{
	List = aList;

	if(aWidth == 0 || aHeight == 0 || aWidth > 16 || aHeight > 16)
	{
		throw ESException("GridListModel::GridListModel: Grid dimensions out of range. [X: %d, Y: %d]", aWidth, aHeight);
	}

	Width = aWidth;
	Height = aHeight;
	FirstItem = 0;
	
	DrawLabels = aLabels;
	DrawHeader = aHeader;
	RefreshHeader = true;
}

											GridListModel::~GridListModel						()
{
}

bool										GridListModel::Input								()
{
	uint32_t oldIndex = List->SelectedIndex;
	int32_t XSelection = List->SelectedIndex % Width;
	int32_t YSelection = (List->SelectedIndex - FirstItem) / Width;

	XSelection += es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? 1 : 0;
	XSelection -= es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? 1 : 0;
	XSelection = Utility::Clamp(XSelection, 0, (int32_t)Width - 1);

	YSelection += es_input->ButtonPressed(0, ES_BUTTON_DOWN) ? 1 : 0;
	YSelection -= es_input->ButtonPressed(0, ES_BUTTON_UP) ? 1 : 0;

	while(YSelection < 0)
	{
		YSelection ++;
		FirstItem -= Width;
	}	
		
	while(YSelection >= Height)
	{
		YSelection --;
		FirstItem += Width;
	}
	
	while(FirstItem >= 0 && (FirstItem + (Width * Height) >= List->Items.size() + Width))
	{
		FirstItem -= Width;
	}

	while(FirstItem < 0)
	{
		FirstItem += Width;
	}
	
	
	List->SelectedIndex = FirstItem + (YSelection * Width + XSelection);
	if(List->SelectedIndex >= List->Items.size())
	{
		List->SelectedIndex = oldIndex;
	}

	if(DrawHeader && (oldIndex != List->SelectedIndex || RefreshHeader))
	{
		RefreshHeader = false;
		List->SetHeader(List->GetSelected()->GetText());
	}

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		List->Canceled = true;
		return true;
	}

	if(List->GetInputConduit() && List->SelectedIndex < List->Items.size())
	{
		return List->GetInputConduit()->HandleInput(List->GetInterface(), List->GetName()); 
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		List->Canceled = false;
		return true;
	}

	return false;
}

bool										GridListModel::DrawItem								(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());
	
	//TODO: Make this work!
	if(Utility::FileExists(aItem->Properties["THUMB"]))
	{
		image = ImageManager::GetImage("SCRATCH%0");
		ImageManager::FillScratch(0, aItem->Properties["THUMB"]);
	}
	
	Area ImageArea(0, 0, 0, 0);

	if(image && aWidth && aHeight)
	{
		if(ImageArea.Width == 0)
		{
			ImageArea = Area(0, 0, image->GetWidth(), image->GetHeight());
		}

		if(DrawLabels)
		{
			aHeight -= List->LabelFont->GetHeight();
		}

		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, ImageArea.Width, ImageArea.Height);
		
		es_video->PlaceTexture(image, x, y, w, h, 0xFFFFFFFF, &ImageArea);

		if(DrawLabels)
		{
			List->LabelFont->PutString(aItem->GetText().c_str(), aX, aY + aHeight, aItem->GetNormalColor());
		}
	}
	
	if(aSelected)
	{
		es_video->FillRectangle(Area(aX, aY, aWidth - 2, aHeight - 2), Colors::SpecialBackGround);
	}

	return false;
}

bool										GridListModel::Draw									()
{
	uint32_t iconWidth = es_video->GetClip().Width / Width - 4;
	uint32_t iconHeight = es_video->GetClip().Height / Height - 4;	

	uint32_t XSelection = List->SelectedIndex % Width;
	uint32_t YSelection = List->SelectedIndex / Width;

	for(int i = 0; i != Height; i ++)
	{
		for(int j = 0; j != Width; j ++)
		{
			if(FirstItem + (i * Width + j) >= List->Items.size())
			{
				break;
			}
		
			DrawItem(List->Items[FirstItem + (i * Width + j)], j * iconWidth + 4, i * iconHeight + 4, iconWidth, iconHeight, List->SelectedIndex == FirstItem + (i * Width + j));
		}
	}
	
	return false;
}

											LineListModel::LineListModel						(SummerfaceList* aList)
{
	List = aList;
	
	FirstLine = 0;
	LinesDrawn = 0;
}

											LineListModel::~LineListModel						()
{
}

bool										LineListModel::DrawItem								(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());

	if(image)
	{
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(List->LabelFont->GetHeight() - 4) / (double)image->GetHeight()));

		es_video->PlaceTexture(image, aX, aY + 2, width, List->LabelFont->GetHeight() - 4);
		aX += width;
	}

	List->LabelFont->PutString(aItem->GetText().c_str(), aX, aY, aSelected ? aItem->GetHighLightColor() : aItem->GetNormalColor());

	return false;
}

bool										LineListModel::Draw									()
{
	if(List->Items.size() != 0)
	{
		uint32_t itemheight = List->LabelFont->GetHeight();
		LinesDrawn = es_video->GetClip().Height / itemheight;
		
		//TODO: Fix it to draw one or two line lists!
		if(LinesDrawn < 3)
		{
			return true;
		}
		
		uint32_t online = 0;
	
		for(int i = List->SelectedIndex - LinesDrawn / 2; i != List->SelectedIndex + LinesDrawn / 2 + 2; i ++)
		{
			if(i < 0)
			{
				online ++;
				continue;
			}
		
			if(i >= List->Items.size())
			{
				break;
			}
			

			if(DrawItem(List->Items[i], 16, (online * itemheight), i == List->SelectedIndex))
			{
				return true;
			}	

			online ++;
		}
	}

	return false;
}

bool										LineListModel::Input								()
{
	if(List->Items.size() != 0)
	{
		List->SelectedIndex += (es_input->ButtonPressed(0, ES_BUTTON_DOWN) ? 1 : 0);
		List->SelectedIndex -= (es_input->ButtonPressed(0, ES_BUTTON_UP) ? 1 : 0);
		List->SelectedIndex += (es_input->ButtonPressed(0, ES_BUTTON_RIGHT) ? LinesDrawn : 0);
		List->SelectedIndex -= (es_input->ButtonPressed(0, ES_BUTTON_LEFT) ? LinesDrawn : 0);
	
		List->SelectedIndex = Utility::Clamp(List->SelectedIndex, 0, List->Items.size() - 1);
	}

	if(List->GetInputConduit())
	{
		return List->GetInputConduit()->HandleInput(List->GetInterface(), List->GetName()); 
	}
	else if(es_input->ButtonDown(0, ES_BUTTON_ACCEPT))
	{
		List->Canceled = false;
		return true;
	}

	if(es_input->ButtonDown(0, ES_BUTTON_CANCEL))
	{
		List->Canceled = true;
		return true;
	}
	
	return false;
}



