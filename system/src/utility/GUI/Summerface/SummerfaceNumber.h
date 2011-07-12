#pragma once

#include "SummerfaceWindow.h"

class													SummerfaceNumber : public SummerfaceWindow, public SummerfaceCancelable
{
	public:
														SummerfaceNumber				(const Area& aRegion, int64_t aValue, uint32_t aDigits = 10) : SummerfaceWindow(aRegion), SelectedIndex(31), Digits(aDigits) {SetValue(aValue);};
		virtual											~SummerfaceNumber				() {};

		virtual bool									Draw							();
		virtual bool									Input							();

		int64_t											GetValue						();
		void											SetValue						(int64_t aValue);

	private:
		void											IncPosition						(uint32_t aPosition);
		void											DecPosition						(uint32_t aPosition);

		char											Value[33];
		int32_t											SelectedIndex;
		uint32_t										Digits;
};


