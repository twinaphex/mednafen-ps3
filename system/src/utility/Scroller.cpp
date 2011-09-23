#include "Scroller.h"

			Scroller::Scroller				(int32_t aValue, uint32_t aNegative, uint32_t aPositive, uint32_t aNegativePage, uint32_t aPositivePage, uint32_t aHome, uint32_t aEnd) :
	Value(aValue),
	Max(aValue + 1),
	PageSize(0),
	Positive(aPositive),
	Negative(aNegative),
	PositivePage(aPositivePage),
	NegativePage(aNegativePage),
	Home(aHome),
	End(aEnd)
{

}

void		Scroller::SetValue				(int32_t aValue)
{
	Value = aValue;
	Clamp();
}

int32_t		Scroller::GetValue				()
{
	return Value;
}

int32_t		Scroller::Scroll				(uint32_t aButton, uint32_t aMax, uint32_t aPageSize)
{
	PageSize = aPageSize ? aPageSize : PageSize;
	Max = aMax ? aMax : Max;

	if(aButton != 0xFFFFFFFF)
	{
		Value += (aButton == Positive) ? 1 : 0;
		Value -= (aButton == Negative) ? 1 : 0;
		Value += (aButton == PositivePage) ? PageSize : 0;
		Value -= (aButton == NegativePage) ? PageSize : 0;
		Value = (aButton == End) ? Max - 1 : Value;
		Value = (aButton == Home) ? 0 : Value;
	}

	Clamp();
}

void		Scroller::Clamp					()
{
	Value = (Value < 0) ? 0 : Value;
	Value = (Value >= Max) ? Max - 1 : Value;
}

