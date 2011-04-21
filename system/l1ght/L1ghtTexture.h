#pragma once

class								L1ghtVideo;
class								L1ghtTexture : public Texture
{
	friend class					L1ghtVideo;

	public:
									L1ghtTexture			(uint32_t aWidth, uint32_t aHeight);
		virtual						~L1ghtTexture			();
		
		void						Clear					(uint32_t aColor);
		uint32_t*					GetPixels				();
		
		virtual uint32_t			GetFlags				() const {return SLOW_READ;}

		virtual uint32_t			GetRedShift				() const {return 0;}
		virtual uint32_t			GetGreenShift			() const {return 8;}
		virtual uint32_t			GetBlueShift			() const {return 16;}
		virtual uint32_t			GetAlphaShift			() const {return 24;}
	
	protected:
		uint32_t*					Pixels;
		uint32_t					Offset;
};
