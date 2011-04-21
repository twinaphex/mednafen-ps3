#pragma once

class								SDLTexture : public Texture
{
	friend class					SDLVideo;

	public:
									SDLTexture				(uint32_t aWidth, uint32_t aHeight);
		virtual						~SDLTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		uint32_t					GetFlags				() const {return 0;};
		uint32_t					GetRedShift				() const {return 16;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 0;};
		uint32_t					GetAlphaShift			() const {return 24;};
		
	public: //Helper, do not call directly
		void						Apply					();

	private:
		uint32_t*					Pixels;
		uint32_t					ID;
};
