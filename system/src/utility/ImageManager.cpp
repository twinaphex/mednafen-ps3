#include <es_system.h>

#ifndef ES_HAVE_LOADPNG
#include <png.h>
#include "Logger.h"

class										PingPNG
{
	public:
											PingPNG												(const std::string& aPath) :
			File(0),
			PngPtr(0),
			InfoPtr(0),
			RowPointers(0),
			Width(0),
			Height(0),
			Valid(false)
		{
			File = fopen(aPath.c_str(), "rb");

			if(File)
			{
				fread(Header, 1, 8, File);
				if(!png_sig_cmp(Header, 0, 8))
				{
					PngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
					if(PngPtr)
					{
						InfoPtr = png_create_info_struct(PngPtr);
						if(InfoPtr)
						{
							if(setjmp(png_jmpbuf(PngPtr)))
							{
								es_log->Log("PNG Reader: Error during init_io. [File: %s]", aPath.c_str());
								return;
							}

							png_init_io(PngPtr, File);
							png_set_sig_bytes(PngPtr, 8);

	
							if(setjmp(png_jmpbuf(PngPtr)))
							{
								es_log->Log("PNG Reader: Error during read_png. [File: %s]", aPath.c_str());
								return;
							}
							png_read_png(PngPtr, InfoPtr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_SWAP_ALPHA, 0);

							RowPointers = png_get_rows(PngPtr, InfoPtr);
							if(!RowPointers)
							{
								es_log->Log("PNG Reader: Failed to get pixels. [File: %s]", aPath.c_str());
								return;
							}

							Width = png_get_image_width(PngPtr, InfoPtr);
							Height = png_get_image_height(PngPtr, InfoPtr);
							Valid = true;
							return;
						}
						else
						{
							es_log->Log("PNG Reader: png_create_info_struct failed. [File: %s]", aPath.c_str());
							return;
						}
					}
					else
					{
						es_log->Log("PNG Reader: png_create_read_struct failed. [File: %s]", aPath.c_str());
						return;
					}
				}
				else
				{
					es_log->Log("PNG Reader: File not recognized as a PNG file. [File: %s]", aPath.c_str());
					return;
				}
			}
			else
			{
				es_log->Log("PNG Reader: File could not be opened for reading. [File: %s]", aPath.c_str());
				return;
			}
		}

											~PingPNG											()
		{
			if(File)
			{
				fclose(File);
			}

			if(InfoPtr)
			{
				png_destroy_info_struct(PngPtr, &InfoPtr);
			}

			if(PngPtr)
			{
				png_destroy_read_struct(&PngPtr, 0, 0);
			}
		}

		Texture*							GetTexture											()
		{
			Texture* output = 0;
			if(Valid)
			{
				output = ESVideo::CreateTexture(Width, Height, true);
				output->Clear(0);

				uint32_t copyWidth = std::min(Width, output->GetWidth());
				uint32_t copyHeight = std::min(Height, output->GetHeight());
				uint32_t* texPixels = output->Map();

				for(int i = 0; i != copyHeight; i ++)
				{
					uint32_t* dest = texPixels + (output->GetPitch() * i);
					uint8_t* source = RowPointers[i];

					for(int j = 0; j != copyWidth; j ++)
					{
						uint32_t a = (InfoPtr->color_type == PNG_COLOR_TYPE_RGB_ALPHA) ? *source++ : 0xFF;
						uint32_t r = *source ++;
						uint32_t g = *source ++;
						uint32_t b = *source ++;
						*dest++ = output->ConvertPixel(r, g, b, a);
					}
				}

				output->Unmap();
			}

			return output;
		}

	private:
		FILE*								File;
		uint8_t								Header[8];
		png_structp							PngPtr;
		png_infop							InfoPtr;
		png_bytep*							RowPointers;
		uint32_t							Width;
		uint32_t							Height;
		bool								Valid;
};

void										ESSUB_LoadPNG										(const std::string& aPath, Texture** aTexture)
{
	assert(aTexture);

	PingPNG p(aPath);
	*aTexture = p.GetTexture();
}
#endif

void										ImageManager::Purge									()
{
	for(std::map<std::string, Texture*>::iterator i = Images.begin(); i != Images.end(); i ++)
	{
		delete i->second;
	}

	Images.clear();
}
	
Texture*									ImageManager::LoadImage								(const std::string& aName, const std::string& aPath)
{
	if(!Utility::FileExists(aPath))
	{
		return 0;
	}

//HACK: Don't allow more than 100 images, make a less nuclear pruning routine!
	if(Images.size() >= 100)
	{
		Purge();
	}

	if(Images.find(aName) == Images.end())
	{
		Texture* output;

		ESSUB_LoadPNG(aPath, &output);

		if(output)
		{
			output->SetFilter(true);
			Images[aName] = output;
		}
	}
	
	return Images[aName];
}

Texture*									ImageManager::GetImage								(const std::string& aName)
{
	if(Images.find(aName) == Images.end())
	{
		std::string filename = Directory + "/" + aName;

		if(Utility::FileExists(filename))
		{
			return LoadImage(aName, filename);
		}
		else if(Utility::FileExists(filename + ".png"))
		{
			return LoadImage(aName, filename + ".png");
		}
		else
		{
			Images[aName] = 0; //Don't check every time for a non-existant image
		}
	}
	
	return Images[aName];
}

std::string									ImageManager::Directory;
std::map<std::string, Texture*>				ImageManager::Images;

