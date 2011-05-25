#pragma once

class								GLTexture : public Texture
{
	public:
									GLTexture				(uint32_t aWidth, uint32_t aHeight);
		virtual						~GLTexture				();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		uint32_t					GetFlags				() const {return 0;};
		uint32_t					GetRedShift				() const {return 16;};
		uint32_t					GetGreenShift			() const {return 8;};
		uint32_t					GetBlueShift			() const {return 0;};
		uint32_t					GetAlphaShift			() const {return 24;};
		
	public: //Helper, do not call directly
		void						Apply					();
		uint32_t					GetID					() const {return ID;}

	private:
		uint32_t*					Pixels;
		uint32_t					ID;
};

