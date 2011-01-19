#include <ps3_system.h>

											InputListItem::InputListItem						(const std::string& aText, uint32_t aInput) : ListItem(aText, FontManager::GetSmallFont())
{
	Input = aInput;

	std::string inputimage = es_input->GetButtonImage(aInput);
	if(ImageManager::GetImage(inputimage))
	{
		LabelImage = inputimage;
	}
	else
	{
		std::string inputname = es_input->GetButtonName(aInput);

		Text = inputname + " " + Text;
	}
}

uint32_t									InputListItem::GetHeight							()
{
	return ListItem::GetHeight() + 10;
}
