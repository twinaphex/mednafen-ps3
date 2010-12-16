#ifndef SYSTEM__LISTITEM_H
#define SYSTEM__LISTITEM_H

class													ListItem
{
	public:
														ListItem						(const std::string& aText, Font* aFont = 0, std::string aImage = "");
		virtual											~ListItem						();

		virtual bool									Input							();
		virtual void									Draw							(uint32_t aX, uint32_t aY, bool aSelected);

		virtual uint32_t								GetHeight						();
		virtual const std::string&						GetText							();
		
	protected:
		Font*											LabelFont;
		std::string										LabelImage;
	
		std::string										Text;
		uint32_t										TextColor;
		uint32_t										SelectedTextColor;
};

#endif
