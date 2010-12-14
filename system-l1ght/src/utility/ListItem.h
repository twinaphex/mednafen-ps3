#ifndef SYSTEM__LISTITEM_H
#define SYSTEM__LISTITEM_H

class													ListItem
{
	public:
														ListItem						(std::string aText, Font* aFont = 0, std::string aImage = "");

		virtual void									Draw							(uint32_t aX, uint32_t aY, bool aSelected);

	public: //inlines
		virtual											~ListItem						()
		{
		
		}
		
		virtual uint32_t								GetHeight						()
		{
			return LabelFont->GetHeight();
		}
		
		virtual const std::string&						GetText							()
		{
			return Text;
		}

		virtual bool									Input							()
		{	
			return PS3Input::ButtonDown(0, PS3_BUTTON_CROSS);
		}
		
	protected:
		Font*											LabelFont;
		std::string										LabelImage;
	
		std::string										Text;
		uint32_t										TextColor;
		uint32_t										SelectedTextColor;
};

#endif