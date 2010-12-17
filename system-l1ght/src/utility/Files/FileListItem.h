#ifndef SYSTEM__FTPITEM_H
#define SYSTEM__FTPITEM_H

extern "C"
{
	#include "ftpparse.h"
}

class									FileListItem : public ListItem
{
	public:
										FileListItem					(const std::string& aName, const std::string& aPath, bool aDirectory, bool aBookMark);
										FileListItem					(const std::string& aName, const std::string& aPath, struct ftpparse aData);

		virtual							~FileListItem					();

		std::string						GetPath							();
		
		bool							IsLink							();
		bool							IsDirectory						();		
		bool							IsFile							();		

		void							SetBookMark						(bool aSet);
		bool							GetBookMark						();

	protected:
		std::string						Path;
		
		bool							Directory;
		bool							File;
		bool							BookMark;
};

#endif
