#include <ps3_system.h>

											ListItem::ListItem									(const char* aText, Font* aFont, const char* aImage)
{
	LabelFont = aFont == 0 ? FontManager::GetBigFont() : aFont;
	LabelImage = strdup(aImage);

	Text = strdup(aText);

	TextColor = Colors::Normal;
	SelectedTextColor = Colors::HighLight;
}

											ListItem::~ListItem									()
{
	free(Text);
	free(LabelImage);
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

const char*									ListItem::GetText									()
{
	return Text;
}

void										ListItem::SetImage									(const char* aImage)
{
	if(strlen(aImage) < strlen(LabelImage))
	{
		strcpy(LabelImage, aImage);
	}
	else
	{
		free(LabelImage);
		LabelImage = strdup(aImage);
	}
}

const char*									ListItem::GetImage									()
{
	return LabelImage;
}