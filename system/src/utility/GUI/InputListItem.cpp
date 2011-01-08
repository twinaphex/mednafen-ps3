#include <ps3_system.h>

											InputListItem::InputListItem						(const std::string& aText, uint32_t aInput) : ListItem(aText, FontManager::GetSmallFont())
{
	Input = aInput;

	std::string inputname = es_input->GetButtonName(aInput);

	Text = inputname + " " + Text;
}

