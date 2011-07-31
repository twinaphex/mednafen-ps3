#pragma once

class						FastCounter
{
	public:
							FastCounter					(uint32_t aSpeed = 4, uint32_t aButton = ES_BUTTON_AUXRIGHT2) :
			Button(aButton), Speed(aSpeed), LastFPS(0), LastSkip(0), LastFPSTime(0), FrameCount(0), SkipCount(0)
		{
		}

		void				SetButton					(uint32_t aButton)
		{
			Button = aButton;
		}

		uint32_t			GetSpeed					()
		{
			return Fast() ? Speed : 1;
		}

		uint32_t			GetMaxSpeed					()
		{
			return Speed;
		}

		void				SetSpeed					(uint32_t aSpeed)
		{
			assert(Speed && Speed <= 16);
			Speed = aSpeed;
		}

		void				Tick						(bool aSkip = false)
		{
			FrameCount ++;
			SkipCount += aSkip ? 1 : 0;
		}

		bool				Fast						(); //External
		uint32_t			GetFPS						(uint32_t* aSkip); //External
								
	private:
		uint32_t			Button;
		uint32_t			Speed;
		
		uint32_t			LastFPS;
		uint32_t			LastSkip;
		uint32_t			LastFPSTime;
		uint32_t			FrameCount;
		uint32_t			SkipCount;
};

