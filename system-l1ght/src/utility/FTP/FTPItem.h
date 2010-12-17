#ifndef SYSTEM__FTPITEM_H
#define SYSTEM__FTPITEM_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPItem : public ListItem
{
	public:
										FTPItem							(const std::string& aName, const std::string& aPath, struct ftpparse aData);
		virtual							~FTPItem						();

		std::string						GetPath							();
		
		bool							IsLink							();
		bool							IsDirectory						();		
		bool							IsFile							();		

	protected:
		std::string						Path;
		
		bool							Directory;
		bool							File;
};

#endif
