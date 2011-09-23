#pragma once

#include "Scroller.h"

class										Keyboard : public SummerfaceWindow, public SummerfaceCancelable
{
	public:
											Keyboard				(const Area& aRegion, const std::string& aHeader, const std::string& aText);
		virtual								~Keyboard				() {};

		bool								Draw					();
		bool								Input					(uint32_t aButton);
				
		std::string							GetText					()								{return Text;};
		void								SetText					(const std::string& aText)		{Text = aText;};

	protected:
		std::string							Text;
		
		Scroller							Column;
		Scroller							Row;
		uint32_t							Shift;

		Color								TextColor;
		Color								SelectedColor;
};


