#include <es_system.h>
#include "src/thirdparty/simpleini/SimpleIni.h"

extern "C"
{
	//TODO: utf8_decode isn't thread safe
	#include "src/thirdparty/utf8_decode/utf8_decode.h"

	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_BITMAP_H
	#include FT_GLYPH_H
}

namespace
{
	FT_Library			FreeType;	

	Font*				BigFont;
	uint32_t			BigFontSize;

	Font*				SmallFont;
	uint32_t			SmallFontSize;

	Font*				FixedFont;
	uint32_t			FixedFontSize;
}

struct					FontCharacter
{
	Texture*			CharTexture;
	Area				TextureArea;
	int32_t				Width;
	int32_t				Height;
	int32_t				Advance;
	int32_t				BaseX;
	int32_t				BaseY;
};

						Font::Font					(uint32_t aPixelSize, bool aFixed = false)
{
	if(0 != FT_New_Face(FreeType, aFixed ? ESSUB_BuildPath("assets/font/fixed.ttf").c_str() : ESSUB_BuildPath("assets/font/prop.ttf").c_str(), 0, &FontFace))
	{
		Abort("Font::Font: FT_New_Face failed");
	}

	Resize(aPixelSize);
}

						Font::~Font					()
{
	ClearCache();
	FT_Done_Face(FontFace);
}

void					Font::ClearCache			()
{
	for(std::map<uint32_t, FontCharacter*>::iterator i = Cache.begin(); i != Cache.end(); i ++)
	{
		delete i->second->CharTexture;
		delete i->second;
	}

	Cache.clear();
}

void					Font::Resize				(uint32_t aPixelSize)
{
	ClearCache();

	//Calculate new size
	FT_Set_Pixel_Sizes(FontFace, 0, aPixelSize);
	Width = FontFace->max_advance_width / 64;
	Height = FontFace->size->metrics.height / 64;
}

uint32_t				Font::PutString				(const char* aString, uint32_t aX, uint32_t aY, uint32_t aColor, bool aDropShadow, uint32_t aShadowColor, int32_t aShadowXOffset, int32_t aShadowYOffset)
{
	return PutString(aString, 0xFFFFFFFF, aX, aY, aColor, aDropShadow, aShadowColor, aShadowXOffset, aShadowYOffset);
}

uint32_t				Font::PutString				(const char* aString, uint32_t aMaxCharacters, uint32_t aX, uint32_t aY, uint32_t aColor, bool aDropShadow, uint32_t aShadowColor, int32_t aShadowXOffset, int32_t aShadowYOffset)
{
	//Draw any drop shadow
	if(aDropShadow)
	{
		PutString(aString, aMaxCharacters, aX + aShadowXOffset, aY + aShadowYOffset, aShadowColor, false);
	}

	//Begin decoding the string
	utf8_decode_init(aString, strlen(aString));

	//Draw until aMax or the end
	uint32_t drawnChars = 0;
	for(int thischar = utf8_decode_next(); thischar != UTF8_END && drawnChars != aMaxCharacters; thischar = utf8_decode_next(), drawnChars ++)
	{
		//Only draw those in range
		if(thischar >= 32 || thischar == 9)
		{
			//TODO: Support real tab stops
			FontCharacter* chara = CacheCharacter(thischar == 9 ? 32 : thischar);
			
			if(chara && chara->CharTexture)
			{
				ESVideo::PlaceTexture(chara->CharTexture, Area(aX + chara->BaseX, aY + (Height + FontFace->size->metrics.descender / 64) - chara->BaseY, chara->Width, chara->Height), chara->TextureArea, aColor);
			}
			
			if(chara)
			{
				aX += chara->Advance;
			}
		}
	}

	//Done
	return drawnChars;
}

void					Font::PutStringCenter		(const char* aString, const Area& aRegion, uint32_t aColor, bool aDropShadow, uint32_t aShadowColor, int32_t aShadowXOffset, int32_t aShadowYOffset)
{
	int32_t x = (aRegion.Width - MeasureString(aString)) / 2;
	PutString(aString, aRegion.X + x, aRegion.Y, aColor, aDropShadow, aShadowColor, aShadowXOffset, aShadowYOffset);
}


uint32_t				Font::MeasureString			(const char* aString)
{
	uint32_t width = 0;
	utf8_decode_init(aString, strlen(aString));

	for(int thischar = utf8_decode_next(); thischar != UTF8_END; thischar = utf8_decode_next())
	{
		if(thischar >= 32 || thischar == 9)
		{
			//TODO: Support real tab stops
			FontCharacter* chara = CacheCharacter(thischar == 9 ? 32 : thischar);
			
			if(chara)
			{
				width += chara->Advance;
			}
		}
	}

	return width;
}

FontCharacter*			Font::CacheCharacter		(uint32_t aCharacter)
{
	if(Cache[aCharacter] == 0)
	{
		FontCharacter* character = new FontCharacter();

		FT_UInt glyph_index = FT_Get_Char_Index(FontFace, aCharacter);

		if(0 != FT_Load_Glyph(FontFace, glyph_index, FT_LOAD_RENDER))
		{
			Abort("Font::CacheCharacter: FT_Load_Glyph failed");
		}

		character->Advance = FontFace->glyph->advance.x >> 6;
		character->Width = FontFace->glyph->bitmap.width;
		character->Height = FontFace->glyph->bitmap.rows;
		character->BaseX = FontFace->glyph->bitmap_left;
		character->BaseY = FontFace->glyph->bitmap_top;

		if(character->Width != 0 && character->Height != 0)
		{
			character->CharTexture = ESVideo::CreateTexture(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows, true);
			character->TextureArea = Area(0, 0, character->CharTexture->GetWidth(), character->CharTexture->GetHeight());
			character->CharTexture->Clear(0);
		
			uint32_t* pixels = character->CharTexture->Map();
			uint32_t pitch = character->CharTexture->GetPitch();
	
			for(int y = 0; y != FontFace->glyph->bitmap.rows; y ++)
			{
				uint8_t* row = &FontFace->glyph->bitmap.buffer[y * FontFace->glyph->bitmap.pitch];
			
				for(int x = 0; x != FontFace->glyph->bitmap.width; x ++)
				{
					pixels[y * pitch + x] = character->CharTexture->ConvertPixel(0xFF, 0xFF, 0xFF, row[x]);
				}
			}

			character->CharTexture->Unmap();
		}
		else
		{
			character->CharTexture = 0;
		}

		Cache[aCharacter] = character;
	}
	
	return Cache[aCharacter];
}

/////////////////////

void					FontManager::InitFonts		()
{
	if((BigFontSize == 0) || (SmallFontSize == 0) || (FixedFontSize == 0))
	{
		CSimpleIniA INI;
		INI.LoadFile(ESSUB_BuildPath(std::string("assets/colors.ini")).c_str());

		BigFontSize = Utility::Clamp(INI.GetLongValue("bigfont", "size", 30), 10, 100);
		SmallFontSize = Utility::Clamp(INI.GetLongValue("smallfont", "size", 40), 10, 100);
		FixedFontSize = Utility::Clamp(INI.GetLongValue("fixedfont", "size", 36), 10, 100);
	}

	assert(BigFontSize && SmallFontSize && FixedFontSize);

	if(FreeType)
	{
		BigFont->Resize(ESVideo::GetScreenHeight() / BigFontSize);
		SmallFont->Resize(ESVideo::GetScreenHeight() / SmallFontSize);
		FixedFont->Resize(ESVideo::GetScreenHeight() / FixedFontSize);
	}
	else
	{
		if(0 != FT_Init_FreeType(&FreeType))
		{
			Abort("FontManager::Init: Failed to initialize freetype");
		}

		BigFont = new Font(ESVideo::GetScreenHeight() / BigFontSize);
		SmallFont = new Font(ESVideo::GetScreenHeight() / SmallFontSize);
		FixedFont = new Font(ESVideo::GetScreenHeight() / FixedFontSize, true);
	}
}

void					FontManager::QuitFonts		()
{
	delete BigFont; BigFont = 0;
	delete SmallFont; SmallFont = 0;
	delete FixedFont; FixedFont = 0;

	if(FreeType)
	{		
		FT_Done_FreeType(FreeType);
	}
}

Font*					FontManager::GetBigFont		()
{
	assert(BigFont);
	return BigFont;
}

Font*					FontManager::GetSmallFont	()
{
	assert(SmallFont);
	return SmallFont;
}

Font*					FontManager::GetFixedFont	()
{
	assert(FixedFont);
	return FixedFont;
}

