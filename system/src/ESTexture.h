#ifndef ESTEXTURE_H
#define ESTEXTURE_H

class								Texture
{
	public:
		virtual						~Texture				() {};

		virtual void				Clear					(uint32_t aColor) = 0; //Pure Virtual
		virtual uint32_t*			GetPixels				() = 0; //Pure Virtual
		
		virtual uint32_t			GetWidth				() = 0; //Pure Virtual
		virtual uint32_t			GetHeight				() = 0; //Pure Virtual
		virtual uint32_t			GetPitch				() = 0; //Pure Virtual

		virtual void				SetFilter				(uint32_t aOn) = 0; //Pure Virtual

		virtual uint32_t			GetRedShift				() const = 0; //Pure Virtual
		virtual uint32_t			GetGreenShift			() const = 0; //Pure Virtual
		virtual uint32_t			GetBlueShift			() const = 0; //Pure Virtual
		virtual uint32_t			GetAlphaShift			() const = 0; //Pure Virtual

		inline uint32_t				ConvertPixel			(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha) const; //Not Virtual, defined below
};

uint32_t							Texture::ConvertPixel	(uint8_t aRed, uint8_t aGreen, uint8_t aBlue, uint8_t aAlpha) const
{
	return (aRed << GetRedShift()) | (aBlue << GetBlueShift()) | (aGreen << GetGreenShift()) | (aAlpha << GetAlphaShift());
}

#endif
