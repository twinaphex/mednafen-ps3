#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(((FileListItem*)a)->IsDirectory() && !((FileListItem*)b)->IsDirectory())	return true;
		if(((FileListItem*)b)->IsDirectory() && !((FileListItem*)a)->IsDirectory())	return false;
	
		return a->GetText() < b->GetText();
	}

	int		MakeSocket				(const char* aIP, const char* aPort, const char* aPort2 = 0)
	{
		int portno = atoi(aPort);
		if(aPort2 != 0)
		{
			portno = portno * 256 + atoi(aPort2);
		}
		
		int socketFD = socket(AF_INET, SOCK_STREAM, 0);
	
		struct hostent* server = gethostbyname(aIP);
	
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_port = htons(portno);	
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
		
		connect(socketFD, (sockaddr*)&serv_addr, sizeof(serv_addr));
		return socketFD;
	}
}


								FTPFileList::FTPFileList				(const std::string& aHeader, const std::string& aHost, const std::string& aPort, const std::string& aPath, const std::string& aUserName, const std::string& aPassword) : WinterfaceList(aHeader)
{
	Header = aHeader;
	Host = aHost;
	Port = aPort;
	Path = aPath;
	UserName = aUserName;
	Password = aPassword;

	MakePassiveConnection();
	
	DoCommand("LIST\n", 0, false);
	
	std::string data = "";
	uint32_t count;

	memset(Buffer, 0, 2048);
	while(count = read(InSocket, Buffer, 2046))
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
			ftpparse(&pdata, parbuffer, length);
			Items.push_back(new FileListItem(std::string(pdata.name, pdata.namelen - 1), Path + std::string(pdata.name, pdata.namelen - 1) + (pdata.flagtrycwd ? "/" : ""), pdata));
			
			parbuffer = parend + 1;
			length = -1;
		}

		parend ++;
		length ++;
	}

	std::sort(Items.begin(), Items.end(), AlphaSortC);

	free(parsebuffer);
	close(InSocket);
	close(OutSocket);
}

								FTPFileList::~FTPFileList				()
{
}

void							FTPFileList::DownloadFile				(const std::string& aDest)
{
	MakePassiveConnection();

	DoCommand("TYPE I\n", 200);
	
	sprintf(Buffer, "RETR %s\n", GetSelected()->GetText().c_str());
	DoCommand(Buffer, 0, false);

	sprintf(Buffer, "%s/%s", aDest.c_str(), GetSelected()->GetText().c_str());
	FILE* outputFile = fopen(Buffer, "wb");
	uint32_t count;
	
	while(count = read(InSocket, Buffer, 2048))
	{
		fwrite(Buffer, count, 1, outputFile);
	}
	
	fclose(outputFile);

	close(InSocket);
	close(OutSocket);
}

std::string						FTPFileList::GetChosenFile				()
{
	return WasCanceled() ? "" : ((FileListItem*)GetSelected())->GetPath();
}

void							FTPFileList::MakePassiveConnection		()
{
	OutSocket = MakeSocket(Host.c_str(), Port.c_str());

	memset(Buffer, 0, 2048);
	read(OutSocket, Buffer, 2048);

	sprintf(Buffer, "USER %s\n", UserName.c_str());
	DoCommand(Buffer, 331);
	
	sprintf(Buffer, "PASS %s\n", Password.c_str());
	DoCommand(Buffer, 230);

	sprintf(Buffer, "CWD %s\n", Path.c_str());
	DoCommand(Buffer, 250);

	DoCommand("PASV\n", 227);

	//TODO: Make sure it's valid
	char* parse = strchr(Buffer, '(') + 1;
	uint32_t a, b, c, d, e, f;
	sscanf(parse, "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f);
	char newtarget[200];
	char newport[10];			
	sprintf(newtarget, "%d.%d.%d.%d", a,b,c,d);
	sprintf(newport, "%d", e * 256 + f);

	InSocket = MakeSocket(newtarget, newport);
}

uint32_t						FTPFileList::DoCommand					(const std::string& aCommand, uint32_t aNeededResult, bool aResult)
{
	write(OutSocket, aCommand.c_str(), aCommand.length());
	
	if(aResult)
	{
		memset(Buffer, 0, 2048);
		read(OutSocket, Buffer, 2048);
		
		uint32_t code;
		sscanf(Buffer, "%d", &code);
		
		if(aNeededResult && code != aNeededResult)
		{
			throw "FTPFileList::DoCommand: FTP Command Failed";
		}
		
		return code;
	}
	
	return 0;
}
