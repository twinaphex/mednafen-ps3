#include <ps3_system.h>

namespace
{
	char		Buffer[2048];

	int			MakeSocket							(const char* aIP, const char* aPort, const char* aPort2 = 0)
	{
		int portno = atoi(aPort);
		if(aPort2 != 0)
		{
			portno = portno * 256 + atoi(aPort2);
		}
		
		int socketFD = socket(AF_INET, SOCK_STREAM, 0);
		if(socketFD == -1)
		{
			throw FileException("FTP: Could not open socket");
			return -1;
		}
	
		//TODO: gethostby name is appently evil?
		struct hostent* server = gethostbyname(aIP);
		if(server == 0)
		{
			close(socketFD);		
			throw FileException("FTP: Host look up failed");
		}
	
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_port = htons(portno);	
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		
		if(-1 == connect(socketFD, (sockaddr*)&serv_addr, sizeof(serv_addr)))
		{
			close(socketFD);
			throw FileException("FTP: connect() failed");			
		}
		
		return socketFD;
	}

	uint32_t	DoCommand				(int aSocket, const std::string& aCommand, uint32_t aNeededResult = 0, bool aResult = true)
	{
		write(aSocket, aCommand.c_str(), aCommand.length());
		
		if(aResult)
		{
			memset(Buffer, 0, 2048);
			read(aSocket, Buffer, 2048);
			
			uint32_t code;
			sscanf(Buffer, "%d", &code);
			
			if(aNeededResult && code != aNeededResult)
			{
				throw FileException("FTP: Communication error");
			}
			
			return code;
		}
		
		return 0;
	}


	void		MakePassiveConnection	(int& aOutSocket, int& aInSocket, const std::string& aHost, const std::string& aPort, const std::string& aUserName, const std::string& aPassword, const std::string& aPath)
	{
		aOutSocket = -1;
		aInSocket = -1;
	
		try
		{
			aOutSocket = MakeSocket(aHost.c_str(), aPort.c_str());
		
			memset(Buffer, 0, 2048);
			read(aOutSocket, Buffer, 2048);
		
			sprintf(Buffer, "USER %s\n", aUserName.c_str());
			DoCommand(aOutSocket, Buffer, 331);
			
			sprintf(Buffer, "PASS %s\n", aPassword.c_str());
			DoCommand(aOutSocket, Buffer, 230);
		
			sprintf(Buffer, "CWD %s\n", aPath.c_str());
			DoCommand(aOutSocket, Buffer, 250);
		
			DoCommand(aOutSocket, "PASV\n", 227);
		
			//TODO: Make sure it's valid
			char* parse = strchr(Buffer, '(') + 1;
			uint32_t a, b, c, d, e, f;
			sscanf(parse, "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f);
			char newtarget[200];
			char newport[10];			
			sprintf(newtarget, "%d.%d.%d.%d", a,b,c,d);
			sprintf(newport, "%d", e * 256 + f);
		
			aInSocket = MakeSocket(newtarget, newport);
		}
		catch(FileException ex)
		{
			if(aOutSocket != -1)
			{
				close(aOutSocket);
			}
			
			if(aInSocket != -1)
			{
				close(aInSocket);
			}
			
			throw;
		}
	}

};

void			FTPEnumerator::ListPath					(const std::string& aPath, const std::vector<std::string>& aFilters, std::vector<ListItem*>& aItems)
{
	if(Enabled)
	{
		int OutSocket;
		int InSocket;
		
		std::string Path = Enumerators::CleanPath(aPath);
		
		MakePassiveConnection(OutSocket, InSocket, Host, Port, UserName, Password, Path);
		
		DoCommand(OutSocket, "LIST\n", 0, false);
		
		std::string data = "";
		uint32_t count;
	
		memset(Buffer, 0, 2048);
		while((count = read(InSocket, Buffer, 2046)))
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
					aItems.push_back(new FileListItem(std::string(pdata.name, pdata.namelen - 1), aPath + std::string(pdata.name, pdata.namelen - 1) + (pdata.flagtrycwd ? "/" : ""), pdata.flagtrycwd, false));
				}
				
				parbuffer = parend + 1;
				length = -1;
			}
	
			parend ++;
			length ++;
		}
	
		free(parsebuffer);
		close(InSocket);
		close(OutSocket);
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
		int OutSocket;
		int InSocket;
	
		MakePassiveConnection(OutSocket, InSocket, Host, Port, UserName, Password, "/");
	
		DoCommand(OutSocket, "TYPE I\n", 200);
		
		sprintf(Buffer, "RETR %s\n", Enumerators::CleanPath(aPath).c_str());
		DoCommand(OutSocket, Buffer, 0, false);
	
		FILE* outputFile = fopen(es_paths->Build("temp.ftp").c_str(), "wb");
		uint32_t count;
		
		while((count = read(InSocket, Buffer, 2048)))
		{
			fwrite(Buffer, count, 1, outputFile);
		}
		
		fclose(outputFile);
	
		close(InSocket);
		close(OutSocket);
	
		return es_paths->Build("temp.ftp");
	}
	else
	{
		throw FileException("FTP Access is not enabled");
	}
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