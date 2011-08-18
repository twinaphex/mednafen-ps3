#pragma once

#include <algorithm>

class						FastCounter
{
	public:
							FastCounter					(uint32_t aSpeed = 4, uint32_t aButton = ES_BUTTON_AUXRIGHT2) :
			Button(aButton), NormalSpeed(1), FastSpeed(aSpeed), IsToggle(false), ToggleOn(false), WasButtonDown(true), LastFPS(0), LastSkip(0), LastFPSTime(0), FrameCount(0), SkipCount(0)
		{
		}

		uint32_t			GetSpeed					()
		{
			return Fast() ? FastSpeed : NormalSpeed;
		}

		uint32_t			GetMaxSpeed					()
		{
			return std::max(FastSpeed, NormalSpeed);
		}

		void				SetButton					(uint32_t aButton)
		{
			Button = aButton;
		}

		void				SetNormalSpeed				(uint32_t aSpeed)
		{
			assert(aSpeed && aSpeed <= 16);
			NormalSpeed = aSpeed;
		}

		void				SetFastSpeed				(uint32_t aSpeed)
		{
			assert(aSpeed && aSpeed <= 16);
			FastSpeed = aSpeed;
		}

		void				SetToggle					(bool aOn)
		{
			IsToggle = aOn;
			ToggleOn = false;
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

		uint32_t			NormalSpeed;
		uint32_t			FastSpeed;

		bool				IsToggle;
		bool				ToggleOn;
		bool				WasButtonDown;
		
		uint32_t			LastFPS;
		uint32_t			LastSkip;
		uint32_t			LastFPSTime;
		uint32_t			FrameCount;
		uint32_t			SkipCount;
};

