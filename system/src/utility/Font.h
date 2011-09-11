#ifndef FONT_H_
#define FONT_H_

//Freetype declarations
struct										FT_FaceRec_;
typedef struct FT_FaceRec_*					FT_Face;
class										FontCharacter;

class										Font
{
	public:
											Font						(uint32_t aPixelSize, bool aFixed);
											~Font						();

		void								ClearCache					();
		void								Resize						(uint32_t aPixelSize);
								
		uint32_t							PutString					(const char* aString, uint32_t aX, uint32_t aY, uint32_t aColor, bool aDropShadow = false, uint32_t aShadowColor = 0x00000080, int32_t aShadowXOffset = 1, int32_t aShadowYOffset = 1);
		uint32_t							PutString					(const char* aString, uint32_t aMaxCharacters, uint32_t aX, uint32_t aY, uint32_t aColor, bool aDropShadow = false, uint32_t aShadowColor = 0x00000080, int32_t aShadowXOffset = 1, int32_t aShadowYOffset = 1);
		void								PutStringCenter				(const char* aString, const Area& aRegion, uint32_t aColor, bool aDropShadow = false, uint32_t aShadowColor = 0x00000080, int32_t aShadowXOffset = 1, int32_t aShadowYOffset = 1);
		uint32_t							MeasureString				(const char* aString);

		uint32_t							GetWidth					() const {return Width;};
		uint32_t							GetHeight					() const {return Height;};
		
	protected:
		FontCharacter*						CacheCharacter				(uint32_t aCharacter);
	
		FT_Face								FontFace;
		uint32_t							Width;
		uint32_t							Height;
		
		std::map<uint32_t, FontCharacter*>	Cache;
};

namespace									FontManager
{
	void									InitFonts					();
	void									QuitFonts					();
	
	Font*									GetBigFont					();
	Font*									GetSmallFont				();
	Font*									GetFixedFont				();
};

#endif
