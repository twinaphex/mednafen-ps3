#include <ps3_system.h>

namespace
{
	char		Buffer[2048];

	ESSocket*	MakeSocket							(const char* aIP, const char* aPort, const char* aPort2 = 0)
	{
		int portno = atoi(aPort);
		if(aPort2 != 0)
		{
			portno = portno * 256 + atoi(aPort2);
		}

		ESSocket* socket = es_network->OpenSocket(aIP, portno);
		if(socket == 0)
		{
			throw FileException("FTP: Could not open socket");
		}
		
		return socket;
	}

	uint32_t	DoCommand				(ESSocket* aSocket, const std::string& aCommand, uint32_t aNeededResult = 0, bool aResult = true)
	{
		aSocket->Write(aCommand.c_str(), aCommand.length());
		
		if(aResult)
		{
			memset(Buffer, 0, 2048);
			aSocket->Read(Buffer, 2048);
			printf("BUFF=%s\n", Buffer);

			uint32_t code = 0;
			sscanf(Buffer, "%d", &code);
			
			if(aNeededResult && code != aNeededResult)
			{
				printf("%d - %d\n", aNeededResult, code);
				throw FileException("FTP: Communication error");
			}
			
			return code;
		}
		
		return 0;
	}


	void		MakePassiveConnection	(ESSocket*& aOutSocket, ESSocket*& aInSocket, const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword, const std::string& aPath)
	{
		aOutSocket = 0;
		aInSocket = 0;
	
		try
		{
			aOutSocket = MakeSocket(aHost.c_str(), aPort.c_str());
		
			memset(Buffer, 0, 2048);
			aOutSocket->Read(Buffer, 2048);
		
			sprintf(Buffer, "USER %s\n", aUserName.c_str());
			DoCommand(aOutSocket, Buffer, 331);
			
			sprintf(Buffer, "PASS %s\n", aPassword.c_str());
			DoCommand(aOutSocket, Buffer, 230);
		
			sprintf(Buffer, "CWD %s\n", aPath.c_str());
			DoCommand(aOutSocket, Buffer, 250);
		
			DoCommand(aOutSocket, "PASV\n", 227);
		
			//TODO: Make sure it's valid
			char* parse = strchr(Buffer, '(') + 1;
			uint32_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
			sscanf(parse, "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f);
			char newtarget[200];
			char newport[10];			
			sprintf(newtarget, "%d.%d.%d.%d", a,b,c,d);
			sprintf(newport, "%d", e * 256 + f);
		
			aInSocket = MakeSocket(newtarget, newport);
		}
		catch(FileException ex)
		{
			delete aOutSocket;
			delete aInSocket;
			aOutSocket = aInSocket = 0;
			throw;
		}
	}

};

void			FTPEnumerator::ListPath					(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<SummerfaceItem*>& aItems)
{
	if(Enabled)
	{
		ESSocket* OutSocket = 0;
		ESSocket* InSocket = 0;
		
		std::string Path = Enumerators::CleanPath(aPath);
		
		MakePassiveConnection(OutSocket, InSocket, Host, Port, UserName, Password, Path);
		
		DoCommand(OutSocket, "LIST\n", 0, false);
		
		std::string data = "";
		uint32_t count;
	
		memset(Buffer, 0, 2048);
		while((count = InSocket->Read(Buffer, 2046)))
		{
			data += Buffer;
			memset(Buffer, 0, 2048);
		}
		
		char* parsebuffer = strdup(data.c_str());
		char* parbuffer = parsebuffer;
		char* parend = parsebuffer;
		int32_t length = 0;
		
		while(*parend != 0)
		{
			if(*parend == '\n')
			{
				struct ftpparse pdata;
				if(ftpparse(&pdata, parbuffer, length))
				{
					SummerfaceItem* item = new SummerfaceItem(std::string(pdata.name, pdata.namelen - 1), pdata.flagtrycwd ? "FolderICON" : "FileICON");
					item->Properties["DIRECTORY"] = pdata.flagtrycwd ? "1" : "0";
					item->Properties["FILE"] = !pdata.flagtrycwd ? "0" : "1";
					item->Properties["PATH"] = aPath + std::string(pdata.name, pdata.namelen - 1) + (pdata.flagtrycwd ? "/" : "");
					aItems.push_back(item);
				}
				
				parbuffer = parend + 1;
				length = -1;
			}
	
			parend ++;
			length ++;
		}
	
		free(parsebuffer);
		delete InSocket;
		delete OutSocket;
	}
	else
	{
		throw FileException("FTP Access is not enabled");
	}
}

std::string		FTPEnumerator::ObtainFile				(const std::string& aPath)
{
	if(Enabled)
	{
		ESSocket* OutSocket = 0;
		ESSocket* InSocket = 0;
	
		MakePassiveConnection(OutSocket, InSocket, Host, Port, UserName, Password, "/");
	
		DoCommand(OutSocket, "TYPE I\n", 200);
		
		sprintf(Buffer, "RETR %s\n", Enumerators::CleanPath(aPath).c_str());
		DoCommand(OutSocket, Buffer, 0, false);
	
		FILE* outputFile = fopen(es_paths->Build("temp.ftp").c_str(), "wb");
		uint32_t count;
		
		while((count = InSocket->Read(Buffer, 2048)))
		{
			fwrite(Buffer, count, 1, outputFile);
		}
		
		fclose(outputFile);

		delete InSocket;
		delete OutSocket;	
	
		return es_paths->Build("temp.ftp");
	}
	else
	{
		throw FileException("FTP Access is not enabled");
	}
	return "";
}

void			FTPEnumerator::SetCredentials			(const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword)
{
	Host = aHost;
	Port = aPort;
	UserName = aUserName;
	Password = aPassword;
}

void			FTPEnumerator::SetEnabled				(bool aEnabled)
{
	Enabled = aEnabled;
}

bool			FTPEnumerator::GetEnabled				()
{
	return Enabled;
}

bool			FTPEnumerator::Enabled = false;

std::string		FTPEnumerator::Host;
std::string		FTPEnumerator::Port;
std::string		FTPEnumerator::UserName;
std::string		FTPEnumerator::Password;

