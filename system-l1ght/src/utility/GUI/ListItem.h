#ifndef SYSTEM__LISTITEM_H
#define SYSTEM__LISTITEM_H

class													ListItem
{
	public:
														ListItem						(const char* aText, Font* aFont = 0, const char* aImage = "");
		virtual											~ListItem						();

		virtual bool									Input							();
		virtual void									Draw							(uint32_t aX, uint32_t aY, bool aSelected);

		virtual uint32_t								GetHeight						();
		virtual const char*								GetText							();
		
		virtual void									SetImage						(const char* aImage);
		virtual const char*								GetImage						();
		
	protected:
		Font*											LabelFont;
		uint32_t										TextColor;
		uint32_t										SelectedTextColor;

	private:
		char*											Text;
		char*											LabelImage;
};

#endif
