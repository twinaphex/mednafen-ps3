#ifndef FASTCOUNTER_H__
#define FASTCOUNTER_H__

class						FastCounter
{
	public:
							FastCounter					(uint32_t aSpeed = 4, uint32_t aButton = ES_BUTTON_AUXRIGHT2);
							~FastCounter				();
								
		bool				Fast						();
		uint32_t			GetSpeed					();
		void				Tick						(bool aSkip = false);
		uint32_t			GetFPS						(uint32_t* aSkip);
								
	protected:
		uint32_t			Button;
		uint32_t			Speed;
		
		uint32_t			LastFPS;
		uint32_t			LastSkip;
		uint32_t			LastFPSTime;
		uint32_t			FrameCount;
		uint32_t			SkipCount;
};

#endif