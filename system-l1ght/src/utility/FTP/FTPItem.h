#ifndef SYSTEM__FTPITEM_H
#define SYSTEM__FTPITEM_H

extern "C"
{
	#include "ftpparse.h"
}

class									FTPItem : public ListItem
{
	public:
										FTPItem							(const char* aName, const char* aPath, struct ftpparse aData);
		virtual							~FTPItem						();

		const char*						GetFileName						();	
		const char*						GetPath							();
		
		bool							IsLink							();
		bool							IsDirectory						();		
		bool							IsFile							();		

	protected:
		char*							Path;
		
		bool							Directory;
		bool							File;
};

#endif
