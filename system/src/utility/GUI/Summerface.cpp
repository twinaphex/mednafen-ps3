#include <ps3_system.h>

											SummerfaceWindow::SummerfaceWindow					(Summerface* aInterface, const Area& aRegion)
{
	Interface = aInterface;
	Region = aRegion;

	Client = Area(Region.X + BorderWidth, Region.Y + BorderWidth, Region.Width - BorderWidth * 2, Region.Height - BorderWidth * 2);
}

											SummerfaceWindow::~SummerfaceWindow					()
{

}

//Draw the border and background, leave clip set to the windows client area
bool										SummerfaceWindow::PrepareDraw						()
{
	es_video->SetClip(Area(0, 0, es_video->GetScreenWidth(), es_video->GetScreenHeight()));

	es_video->FillRectangle(Area(Region.X, Region.Y, Region.Width, BorderWidth), 0xFFFFFFFF);
	es_video->FillRectangle(Area(Region.X, Region.Bottom() - BorderWidth, Region.Width, BorderWidth), 0xFFFFFFFF);

	es_video->FillRectangle(Area(Region.X, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);
	es_video->FillRectangle(Area(Region.Right() - BorderWidth, Region.Y + BorderWidth, BorderWidth, Region.Height - BorderWidth * 2), 0xFFFFFFFF);

	es_video->FillRectangle(Client, Colors::BackGround);
	es_video->SetClip(Client);

	return Draw();
}

											SummerfaceItem::SummerfaceItem						(const std::string& aText, const std::string& aImage)
{
	SetText(aText);
	SetImage(aImage);
}

											SummerfaceItem::~SummerfaceItem						()
{

}

											SummerfaceGrid::SummerfaceGrid						(Summerface* aInterface, const Area& aRegion, uint32_t aWidth, uint32_t aHeight) : SummerfaceWindow(aInterface, aRegion)
{
	if(aWidth == 0 || aHeight == 0 || aWidth > 16 || aHeight > 16)
	{
		throw ESException("Summerface grid dimensions out of range. [X: %d, Y: %d]", aWidth, aHeight);
	}

	Width = aWidth;
	Height = aHeight;
	
	XSelection = 0;
	YSelection = 0;
	SelectedIndex = 0;
	Canceled = false;
}

											SummerfaceGrid::~SummerfaceGrid						()
{
	for(std::vector<SummerfaceItem*>::iterator iter = Items.begin(); iter != Items.end(); iter ++)
	{
		delete (*iter);
	}
}

bool										SummerfaceGrid::Input								()
{
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

	return false;
}

bool										SummerfaceGrid::DrawItem							(SummerfaceItem* aItem, uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(aItem->GetImage());
	uint32_t TextColor = 0xFFFFFFFF;
	Area ImageArea(0, 0, 0, 0);

	if(image && aWidth != 0 && aHeight)
	{
		if(ImageArea.Width == 0)
		{
			ImageArea = Area(0, 0, image->GetWidth(), image->GetHeight());
		}

		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, ImageArea.Width, ImageArea.Height);
		
		es_video->PlaceTexture(image, x, y, w, h, 0xFFFFFFFF, &ImageArea);
	}
	
	if(aSelected)
	{
		es_video->FillRectangle(Area(aX, aY, aWidth, aHeight), Colors::SpecialBackGround);
	}

	return false;
}

bool										SummerfaceGrid::Draw								()
{
	uint32_t iconWidth = es_video->GetClip().Width / Width - 4;
	uint32_t iconHeight = es_video->GetClip().Height / Height - 4;	

	for(int i = 0; i != Height; i ++)
	{
		for(int j = 0; j != Width; j ++)
		{
			if(i * Width + j >= Items.size())
			{
				break;
			}
		
			DrawItem(Items[i * Width + j], j * iconWidth + 4, i * iconHeight + 4, iconWidth, iconHeight, j == XSelection && i == YSelection);
		}
	}
	
	return false;
}

uint32_t									SummerfaceGrid::GetItemCount						()
{
	return Items.size();
}

void										SummerfaceGrid::SetSelection						(uint32_t aIndex)
{
	if(aIndex >= Items.size())
	{
		throw ESException("WinterfaceIconGrid: Item index out of range [Item %d, Total %d]", aIndex, Items.size());
	}

	SelectedIndex = aIndex;
	XSelection = aIndex % Width;
	YSelection = aIndex / Width;
}

bool										SummerfaceGrid::WasCanceled							()
{
	return Canceled;
}


void										SummerfaceItem::SetText								(const std::string& aText)
{
	Text = aText;
}

void										SummerfaceItem::SetImage							(const std::string& aImage)
{
	Image = aImage;
}

std::string									SummerfaceItem::GetText								()
{
	return Text;
}

std::string									SummerfaceItem::GetImage							()
{
	return Image;
}



											Summerface::Summerface								()
{
}

											Summerface::~Summerface								()
{
	//Delete all windows
	for(std::map<std::string, SummerfaceWindow*>::iterator iter = Windows.begin(); iter != Windows.end(); iter ++)
	{
		delete iter->second;
	}
}

bool										Summerface::Input									()
{
	if(Windows[ActiveWindow])
	{
		return Windows[ActiveWindow]->Input();
	}

	return false;
}

bool										Summerface::Draw									()
{
	uint32_t screenW = es_video->GetScreenWidth();
	uint32_t screenH = es_video->GetScreenHeight();

	es_video->SetClip(Area(0, 0, 0, 0));
	es_video->FillRectangle(Area(0, 0, screenW, screenH), 0x40404080);

	for(std::map<std::string, SummerfaceWindow*>::iterator i = Windows.begin(); i != Windows.end(); i ++)
	{
		if(i->second && i->second->PrepareDraw())
		{
			return true;
		}
	}

	return false;
}

void										Summerface::AddWindow								(const std::string& aName, SummerfaceWindow* aWindow)
{
	if(!Windows[aName] && aWindow)
	{
		Windows[aName] = aWindow;
		ActiveWindow = aName;
	}
	else if(!aWindow)
	{
		throw ESException("Window with name is null. [Name: %s]", aName.c_str());		
	}
	else
	{
		throw ESException("Window with name is already present. [Name: %s]", aName.c_str());
	}
}

void										Summerface::SetActiveWindow							(const std::string& aName)
{
	ActiveWindow = aName;
}

