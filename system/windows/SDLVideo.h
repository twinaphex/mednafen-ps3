#pragma once

class								ESVideoPlatform
{
	public:	
		static void					Initialize				(uint32_t& aWidth, uint32_t& aHeight); //External
		static void					Shutdown				(); //External
		static void					Flip					();
		
	protected:
		static SDL_Surface*			Screen;
};


