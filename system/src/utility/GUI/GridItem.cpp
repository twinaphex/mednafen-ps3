#include <ps3_system.h>

							GridItem::GridItem							(const std::string& aText, const std::string& aImage) : ListItem(aText, FontManager::GetSmallFont(), aImage)
{

}

void						GridItem::Draw								(uint32_t aX, uint32_t aY, uint32_t aWidth, uint32_t aHeight, bool aSelected)
{
	Texture* image = ImageManager::GetImage(LabelImage);

	if(image && LabelFont && aWidth != 0 && aHeight > LabelFont->GetHeight() + 1)
	{
		aHeight -= LabelFont->GetHeight();
	
		//TODO: Make this a Uitlity:: function (along with ListItem)
		uint32_t x = aX, y = aY, w = aWidth, h = aHeight;
		Utility::CenterAndScale(x, y, w, h, image->GetWidth(), image->GetHeight());
		
//		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(aHeight) / (double)image->GetHeight()));

//		es_video->PlaceTexture(image, aX, aY, width, aHeight, 0xFFFFFFFF);
		es_video->PlaceTexture(image, x, y, w, h, 0xFFFFFFFF);
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
		return ImageManager::GetImage(LabelImage)->GetWidth();
	}
	
	return 0;
}

uint32_t					GridItem::GetHeight							()
{
	if(ImageManager::GetImage(LabelImage))
	{
		return ImageManager::GetImage(LabelImage)->GetHeight();
	}
	
	return 0;
}
