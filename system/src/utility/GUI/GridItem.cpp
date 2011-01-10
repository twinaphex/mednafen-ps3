#include <ps3_system.h>

							GridItem::GridItem							(const std::string& aText, const std::string& aImage) : ListItem(aText, FontManager::GetSmallFont(), aImage)
{
	ImageArea = Area(0, 0, 0, 0);
}

void						GridItem::Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(LabelImage);

	if(image && LabelFont && aWidth != 0 && aHeight > LabelFont->GetHeight() + 1)
	{
		aHeight -= LabelFont->GetHeight();
	
		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, image->GetWidth(), image->GetHeight());
		
		es_video->PlaceTexture(image, x, y, w, h, 0xFFFFFFFF, ImageArea.Width == 0 ? 0 : &ImageArea);
		LabelFont->PutString(GetText().c_str(), aX, aY + aHeight, TextColor);
	}
	
	if(aSelected)
	{
		es_video->FillRectangle(Area(aX, aY, aWidth, aHeight + LabelFont->GetHeight()), Colors::SpecialBackGround);
	}
}

uint32_t					GridItem::GetWidth							()
{
	if(ImageManager::GetImage(LabelImage))
	{
		return ImageArea.Width ? ImageArea.Width : ImageManager::GetImage(LabelImage)->GetWidth();
	}
	
	return 0;
}

uint32_t					GridItem::GetHeight							()
{
	if(ImageManager::GetImage(LabelImage))
	{
		return ImageArea.Height ? ImageArea.Height : ImageManager::GetImage(LabelImage)->GetHeight();
	}
	
	return 0;
}

void						GridItem::SetImageArea						(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight)
{
	ImageArea = Area(aX, aY, aWidth, aHeight);
}

