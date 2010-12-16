#include <ps3_system.h>

							GridItem::GridItem							(const char* aText, const char* aImage) : ListItem(aText, FontManager::GetSmallFont(), aImage)
{

}

void						GridItem::Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(GetImage());

	if(image)
	{
		aHeight -= LabelFont->GetHeight();
	
		//TODO: Make this a Uitlity:: function (along with ListItem)
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(aHeight) / (double)image->GetHeight()));

	
		PS3Video::PlaceTexture(image, aX, aY, width, aHeight, 0xFFFFFFFF);
		LabelFont->PutString(GetText(), aX, aY + aHeight, TextColor);
	}
	
	if(aSelected)
	{
		//TODO: Make a color in Utility::Colors
		PS3Video::FillRectangle(Area(aX, aY, aWidth, aHeight + LabelFont->GetHeight()), 0x40404040);
	}
}

uint32_t					GridItem::GetWidth							()
{
	if(ImageManager::GetImage(GetImage()))
	{
		return ImageManager::GetImage(GetImage())->GetWidth();
	}
	
	return 0;
}

uint32_t					GridItem::GetHeight							()
{
	if(ImageManager::GetImage(GetImage()))
	{
		return ImageManager::GetImage(GetImage())->GetHeight();
	}
	
	return 0;
}
