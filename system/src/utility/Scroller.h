#pragma once
#include <stdint.h>

//Note that value ranges from 0 to (max - 1)
class				Scroller
{
	public:
					Scroller				(int32_t aValue, uint32_t aNegative, uint32_t aPositive, uint32_t aNegativePage = 0xFFFFFFFF, uint32_t aPositivePage = 0xFFFFFFFF, uint32_t aHome = 0xFFFFFFFF, uint32_t aEnd = 0xFFFFFFFF);

		void		SetValue				(int32_t aValue);
		int32_t		GetValue				();
		
		int32_t		Scroll					(uint32_t aButton, uint32_t aMax = 0, uint32_t aPageSize = 0);

		Scroller&	operator=				(uint32_t aValue) {SetValue(aValue); return *this;};
					operator int32_t		() {return GetValue();}

	private:
		void		Clamp					();

		int32_t		Value;
		int32_t		Max;
		uint32_t	PageSize;

		uint32_t	Positive;
		uint32_t	Negative;

		uint32_t	PositivePage;
		uint32_t	NegativePage;

		uint32_t	Home;
		uint32_t	End;
};


