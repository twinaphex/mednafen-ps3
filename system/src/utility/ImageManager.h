#ifndef SYSTEM__IMAGEMANAGER_H
#define SYSTEM__IMAGEMANAGER_H

class													ImageManager
{
	public:
		struct											PNGFile
		{
														PNGFile							(const std::string& aFileName);
														~PNGFile						();
			void										CopyToTexture					(Texture* aTexture);
			
			png_structp									png_ptr;
			png_infop									info_ptr;
			png_bytep*									row_pointers;
			uint32_t									Width;
			uint32_t									Height;
		};
	
	public:
		static void										Purge							();
		static void										SetDirectory					(const std::string& aPath);

		static Texture*									LoadImage						(const std::string& aName, const std::string& aPath);
		static Texture*									GetImage						(const std::string& aName);
	
	protected:
		static std::string								Directory;

		static std::map<std::string, Texture*>			Images;
};

#endif

