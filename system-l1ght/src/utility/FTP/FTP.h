#ifndef SYSTEM__FTP_H
#define SYSTEM__FTP_H


extern "C"
{
	#include "ftpparse.h"
}



class									FTPItem : public ListItem
{
	public:
										FTPItem							(const char* aName, const char* aPath, struct ftpparse aData);
		virtual							~FTPItem						();
	
		const char*						GetPath							();

	public:
		char*							Path;
		bool							Directory;
		bool							File;
};

class									FTPFileList : public WinterfaceList
{
	public:
										FTPFileList						(const char* aHost, const char* aPort, const char* aPath);
		virtual							~FTPFileList					();

		const char*						GetFile							();

		uint32_t						DoCommand						(const char* aCommand, bool aResult = true);
		
	protected:
		int								OutSocket;
		int								InSocket;
		char							Buffer[2048];
};

class									FTPSelect
{
	public:
										FTPSelect						(const char* aHeader, const char* aHost, const char* aPort, MenuHook* aInputHook = 0);
										~FTPSelect						();
		const char*						GetFile							();

	protected:
		char*							Header;
		char*							Host;
		char*							Port;
	
		std::stack<FTPFileList*>		Lists;

		MenuHook*						InputHook;
};


#endif
