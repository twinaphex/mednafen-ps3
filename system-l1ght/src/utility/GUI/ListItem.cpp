#include <ps3_system.h>

											ListItem::ListItem									(const std::string& aText, Font* aFont, const std::string& aImage)
{
	Text = aText;
	LabelImage = aImage;

	LabelFont = aFont == 0 ? FontManager::GetBigFont() : aFont;
	TextColor = Colors::Normal;
	SelectedTextColor = Colors::HighLight;
}

											ListItem::~ListItem									()
{
}

bool										ListItem::Input										()
{	
	return PS3Input::ButtonDown(0, PS3_BUTTON_CROSS);
}

void										ListItem::Draw										(uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(LabelImage);

	if(image)
	{
		//TODO: Make this a Uitlity:: function (along with GridItem)
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(GetHeight() - 4) / (double)image->GetHeight()));
	
		PS3Video::PlaceTexture(image, aX, aY + 2, width, GetHeight() - 4);
		aX += width;
	}

	LabelFont->PutString(Text, aX, aY, aSelected ? SelectedTextColor : TextColor);
}
		
uint32_t									ListItem::GetHeight									()
{
	return LabelFont->GetHeight();
}

std::string									ListItem::GetText									()
{
	return Text;
}
