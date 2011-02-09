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
	return es_input->ButtonDown(0, ES_BUTTON_ACCEPT);
}

void										ListItem::Draw										(uint32_t aX, uint32_t aY, bool aSelected)
{
	Texture* image = ImageManager::GetImage(LabelImage);

	if(image)
	{
		uint32_t width = (uint32_t)((double)image->GetWidth() * ((double)(GetHeight() - 4) / (double)image->GetHeight()));
	
		es_video->PlaceTexture(image, aX, aY + 2, width, GetHeight() - 4);
		aX += width;
	}

	LabelFont->PutString(Text.c_str(), aX, aY, aSelected ? SelectedTextColor : TextColor);
}
		
uint32_t									ListItem::GetHeight									()
{
	return LabelFont->GetHeight();
}

std::string									ListItem::GetText									()
{
	return Text;
}
