#ifndef SYSTEM__SUMMERFACE_ITEM_H
#define SYSTEM__SUMMERFACE_ITEM_H

class													SummerfaceItem
{
	public:
														SummerfaceItem					(const std::string& aText, const std::string& aImage);
		virtual											~SummerfaceItem					();

		virtual void									SetText							(const std::string& aText);
		virtual void									SetImage						(const std::string& aImage);

		virtual std::string								GetText							();
		virtual std::string								GetImage						();

	public:
		std::map<std::string, std::string>				Properties;

	private:
		std::string										Text;
		std::string										Image;
};


#endif

