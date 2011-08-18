#pragma once

template <typename T>
class													SummerfaceItemUser
{
	public:
														SummerfaceItemUser				(const std::string& aText, const std::string& aImage, uint32_t aNormalColor = Colors::Normal, uint32_t aHighLightColor = Colors::HighLight)
		{
			SetText(aText);
			SetImage(aImage);
			SetColors(aNormalColor, aHighLightColor);
		}

														SummerfaceItemUser				(const std::string& aText, const std::string& aImage, const T& aValue, uint32_t aNormalColor = Colors::Normal, uint32_t aHighLightColor = Colors::HighLight)
		{
			SetText(aText);
			SetImage(aImage);
			SetColors(aNormalColor, aHighLightColor);
			UserData = aValue;
		}

		virtual											~SummerfaceItemUser				() {}

		virtual void									SetText							(const std::string& aText) {Text = aText;}
		virtual void									SetImage						(const std::string& aImage) {Image = aImage;}

		virtual std::string								GetText							() {return Text;}
		virtual std::string								GetImage						() {return Image;}

		virtual void									SetColors						(uint32_t aNormalColor, uint32_t aSpecialColor) {NormalColor = aNormalColor; SpecialColor = aSpecialColor;}

		virtual uint32_t								GetNormalColor					() {return NormalColor;}
		virtual uint32_t								GetHighLightColor				() {return SpecialColor;}

		T												UserData;

	private:
		std::string										Text;
		std::string										Image;

		uint32_t										NormalColor;
		uint32_t										SpecialColor;
};

typedef SummerfaceItemUser<int>							SummerfaceItem;

