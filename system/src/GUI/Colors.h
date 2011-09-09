#pragma once

class										Color
{
	public:
											Color				();
											Color				(uint32_t aColor);
											Color				(const char* aName, uint32_t aDefault);

		uint32_t							GetValue			() {return Value;}
		void								SetValue			(uint32_t aValue) {Value = aValue;}

											operator uint32_t	() {return Value;}
		Color&								operator=			(const uint32_t &aValue) {Value = aValue;}

	private:
		uint32_t							Value;
};

class										Colors
{
	public:
		static void							Initialize					();

		static uint32_t						GetColor					(const char* aColorName, uint32_t aDefaultValue);
		static uint32_t						Alpha						(uint32_t aColor, uint8_t aAlpha) {return (aColor & 0xFFFFFF00) | aAlpha;}

	public:
		static const uint32_t				steelblue = 0x4682B4FF;
		static const uint32_t				royalblue = 0x041690FF;
		static const uint32_t				cornflowerblue = 0x6495EDFF;
		static const uint32_t				lightsteelblue = 0xB0C4DEFF;
		static const uint32_t				mediumslateblue = 0x7B68EEFF;
		static const uint32_t				slateblue = 0x6A5ACDFF;
		static const uint32_t				darkslateblue = 0x483D8BFF;
		static const uint32_t				midnightblue = 0x191970FF;
		static const uint32_t				navy = 0x000080FF;
		static const uint32_t				darkblue = 0x00008BFF;
		static const uint32_t				mediumblue = 0x0000CDFF;
		static const uint32_t				blue = 0x0000FFFF;
		static const uint32_t				dodgerblue = 0x1E90FFFF;
		static const uint32_t				deepskyblue = 0x00BFFFFF;
		static const uint32_t				lightskyblue = 0x87CEFAFF;
		static const uint32_t				skyblue = 0x87CEEBFF;
		static const uint32_t				lightblue = 0xADD8E6FF;
		static const uint32_t				powderblue = 0xB0E0E6FF;
		static const uint32_t				azure = 0xF0FFFFFF;
		static const uint32_t				lightcyan = 0xE0FFFFFF;
		static const uint32_t				paleturquoise = 0xAFEEEEFF;
		static const uint32_t				mediumturquoise = 0x48D1CCFF;
		static const uint32_t				lightseagreen = 0x20B2AAFF;
		static const uint32_t				darkcyan = 0x008B8BFF;
		static const uint32_t				teal = 0x008080FF;
		static const uint32_t				cadetblue = 0x5F9EA0FF;
		static const uint32_t				darkturquoise = 0x00CED1FF;
		static const uint32_t				aqua = 0x00FFFFFF;
		static const uint32_t				cyan = 0x00FFFFFF;
		static const uint32_t				turquoise = 0x40E0D0FF;
		static const uint32_t				aquamarine = 0x7FFFD4FF;
		static const uint32_t				mediumaquamarine = 0x66CDAAFF;
		static const uint32_t				darkseagreen = 0x8FBC8FFF;
		static const uint32_t				mediumseagreen = 0x3CB371FF;
		static const uint32_t				seagreen = 0x2E8B57FF;
		static const uint32_t				darkgreen = 0x006400FF;
		static const uint32_t				green = 0x008000FF;
		static const uint32_t				forestgreen = 0x228B22FF;
		static const uint32_t				limegreen = 0x32CD32FF;
		static const uint32_t				lime = 0x00FF00FF;
		static const uint32_t				chartreuse = 0x7FFF00FF;
		static const uint32_t				lawngreen = 0x7CFC00FF;
		static const uint32_t				greenyellow = 0xADFF2FFF;
		static const uint32_t				yellowgreen = 0x9ACD32FF;
		static const uint32_t				palegreen = 0x98FB98FF;
		static const uint32_t				lightgreen = 0x90EE90FF;
		static const uint32_t				springgreen = 0x00FF7FFF;
		static const uint32_t				mediumspringgreen = 0x00FA9AFF;
		static const uint32_t				darkolivegreen = 0x556B2FFF;
		static const uint32_t				olivedrab = 0x6B8E23FF;
		static const uint32_t				olive = 0x808000FF;
		static const uint32_t				darkkhaki = 0xBDB76BFF;
		static const uint32_t				darkgoldenrod = 0xB8860BFF;
		static const uint32_t				goldenrod = 0xDAA520FF;
		static const uint32_t				gold = 0xFFD700FF;
		static const uint32_t				yellow = 0xFFFF00FF;
		static const uint32_t				khaki = 0xF0E68CFF;
		static const uint32_t				palegoldenrod = 0xEEE8AAFF;
		static const uint32_t				blanchedalmond = 0xFFEBCDFF;
		static const uint32_t				moccasin = 0xFFE4B5FF;
		static const uint32_t				wheat = 0xF5DEB3FF;
		static const uint32_t				navajowhite = 0xFFDEADFF;
		static const uint32_t				burlywood = 0xDEB887FF;
		static const uint32_t				tan = 0xD2B48CFF;
		static const uint32_t				rosybrown = 0xBC8F8FFF;
		static const uint32_t				sienna = 0xA0522DFF;
		static const uint32_t				saddlebrown = 0x8B4513FF;
		static const uint32_t				chocolate = 0xD2691EFF;
		static const uint32_t				peru = 0xCD853FFF;
		static const uint32_t				sandybrown = 0xF4A460FF;
		static const uint32_t				darkred = 0x8B0000FF;
		static const uint32_t				maroon = 0x800000FF;
		static const uint32_t				brown = 0xA52A2AFF;
		static const uint32_t				firebrick = 0xB22222FF;
		static const uint32_t				indianred = 0xCD5C5CFF;
		static const uint32_t				lightcoral = 0xF08080FF;
		static const uint32_t				salmon = 0xFA8072FF;
		static const uint32_t				darksalmon = 0xE9967AFF;
		static const uint32_t				lightsalmon = 0xFFA07AFF;
		static const uint32_t				coral = 0xFF7F50FF;
		static const uint32_t				tomato = 0xFF6347FF;
		static const uint32_t				darkorange = 0xFF8C00FF;
		static const uint32_t				orange = 0xFFA500FF;
		static const uint32_t				orangered = 0xFF4500FF;
		static const uint32_t				crimson = 0xDC143CFF;
		static const uint32_t				red = 0xFF0000FF;
		static const uint32_t				deeppink = 0xFF1493FF;
		static const uint32_t				fuchsia = 0xFF00FFFF;
		static const uint32_t				magenta = 0xFF00FFFF;
		static const uint32_t				hotpink = 0xFF69B4FF;
		static const uint32_t				lightpink = 0xFFB6C1FF;
		static const uint32_t				pink = 0xFFC0CBFF;
		static const uint32_t				palevioletred = 0xDB7093FF;
		static const uint32_t				mediumvioletred = 0xC71585FF;
		static const uint32_t				purple = 0x800080FF;
		static const uint32_t				darkmagenta = 0x8B008BFF;
		static const uint32_t				mediumpurple = 0x9370DBFF;
		static const uint32_t				blueviolet = 0x8A2BE2FF;
		static const uint32_t				indigo = 0x4B0082FF;
		static const uint32_t				darkviolet = 0x9400D3FF;
		static const uint32_t				darkorchid = 0x9932CCFF;
		static const uint32_t				mediumorchid = 0xBA55D3FF;
		static const uint32_t				orchid = 0xDA70D6FF;
		static const uint32_t				violet = 0xEE82EEFF;
		static const uint32_t				plum = 0xDDA0DDFF;
		static const uint32_t				thistle = 0xD8BFD8FF;
		static const uint32_t				lavender = 0xE6E6FAFF;
		static const uint32_t				ghostwhite = 0xF8F8FFFF;
		static const uint32_t				aliceblue = 0xF0F8FFFF;
		static const uint32_t				mintcream = 0xF5FFFAFF;
		static const uint32_t				honeydew = 0xF0FFF0FF;
		static const uint32_t				lightgoldenrodyellow = 0xFAFAD2FF;
		static const uint32_t				lemonchiffon = 0xFFFACDFF;
		static const uint32_t				cornsilk = 0xFFF8DCFF;
		static const uint32_t				lightyellow = 0xFFFFE0FF;
		static const uint32_t				ivory = 0xFFFFF0FF;
		static const uint32_t				floralwhite = 0xFFFAF0FF;
		static const uint32_t				linen = 0xFAF0E6FF;
		static const uint32_t				oldlace = 0xFDF5E6FF;
		static const uint32_t				antiquewhite = 0xFAEBD7FF;
		static const uint32_t				bisque = 0xFFE4C4FF;
		static const uint32_t				peachpuff = 0xFFDAB9FF;
		static const uint32_t				papayawhip = 0xFFEFD5FF;
		static const uint32_t				beige = 0xF5F5DCFF;
		static const uint32_t				seashell = 0xFFF5EEFF;
		static const uint32_t				lavenderblush = 0xFFF0F5FF;
		static const uint32_t				mistyrose = 0xFFE4E1FF;
		static const uint32_t				snow = 0xFFFAFAFF;
		static const uint32_t				white = 0xFFFFFFFF;
		static const uint32_t				whitesmoke = 0xF5F5F5FF;
		static const uint32_t				gainsboro = 0xDCDCDCFF;
		static const uint32_t				lightgrey = 0xD3D3D3FF;
		static const uint32_t				silver = 0xC0C0C0FF;
		static const uint32_t				darkgray = 0xA9A9A9FF;
		static const uint32_t				gray = 0x808080FF;
		static const uint32_t				lightslategray = 0x778899FF;
		static const uint32_t				slategray = 0x708090FF;
		static const uint32_t				dimgray = 0x696969FF;
		static const uint32_t				darkslategray = 0x2F4F4FFF;
		static const uint32_t				black = 0x000000FF;
};

