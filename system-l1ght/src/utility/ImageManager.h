#ifndef SYSTEM__IMAGEMANAGER_H
#define SYSTEM__IMAGEMANAGER_H

class													ImageManager
{
	public:
		static void										Purge							();
		static void										LoadDirectory					(std::string aPath);
		static Texture*									LoadImage						(std::string aName, std::string aPath);

	public://INLINES
		static Texture*									GetImage						(std::string aName)
		{
			return Images[aName];
		}

	protected:
		static std::map<std::string, Texture*>			Images;
};

#endif