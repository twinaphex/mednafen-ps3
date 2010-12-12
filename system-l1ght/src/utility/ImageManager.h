#ifndef SYSTEM__IMAGEMANAGER_H
#define SYSTEM__IMAGEMANAGER_H

class													ImageManager
{
	public:
		static void										Purge							();
		static Texture*									LoadImage						(std::string aName, const void* aPngData, uint32_t aPngSize);

	public://INLINES
		static Texture*									GetImage						(std::string aName)
		{
			return Images[aName];
		}

	protected:
		static std::map<std::string, Texture*>			Images;
};

#endif