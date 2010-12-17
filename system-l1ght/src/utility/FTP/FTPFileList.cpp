#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(((FTPItem*)a)->IsDirectory() && !((FTPItem*)b)->IsDirectory())	return true;
		if(((FTPItem*)b)->IsDirectory() && !((FTPItem*)a)->IsDirectory())	return false;
	
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


								FTPFileList::FTPFileList				(const std::string& aHeader, const std::string& aHost, const std::string& aPort, const std::string& aPath) : WinterfaceList(aHeader)
{
	Header = aHeader;
	Host = aHost;
	Port = aPort;
	Path = aPath;

	MakePassiveConnection();
	
	DoCommand("LIST\n", 0, false);
	
	read(InSocket, Buffer, 2048);
	
	char* parbuffer = Buffer;
	char* parend = Buffer;
	int32_t length = 0;
	
	while(*parend != 0)
	{
		if(*parend == '\n')
		{
			struct ftpparse pdata;
			ftpparse(&pdata, parbuffer, length);
			Items.push_back(new FTPItem(std::string(pdata.name, pdata.namelen - 1).c_str(), (std::string(aPath) + std::string(pdata.name, pdata.namelen - 1) + (pdata.flagtrycwd ? "/" : "")).c_str(), pdata));
			
			parbuffer = parend + 1;
			length = -1;
		}

		parend ++;
		length ++;
	}

	std::sort(Items.begin(), Items.end(), AlphaSortC);

	close(InSocket);
	close(OutSocket);
}

								FTPFileList::~FTPFileList				()
{
}

void							FTPFileList::DownloadFile				(const std::string& aDest)
{
	MakePassiveConnection();

	DoCommand("TYPE I\n");
	sprintf(Buffer, "RETR %s\n", GetSelected()->GetText().c_str());
	DoCommand(Buffer, 0, false);

	FILE* outputFile = fopen((aDest + "/" + GetSelected()->GetText()).c_str(), "wb");
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
	return WasCanceled() ? "" : ((FTPItem*)GetSelected())->GetPath();
}

void							FTPFileList::MakePassiveConnection		()
{
	OutSocket = MakeSocket(Host.c_str(), Port.c_str());

	memset(Buffer, 0, 2048);
	read(OutSocket, Buffer, 2048);

	DoCommand("USER anonymous\n", 331);
	DoCommand("PASS \n", 230);

	sprintf(Buffer, "CWD %s\n", Path.c_str());
	DoCommand(Buffer, 250);


	DoCommand("PASV\n", 227);

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
			ps3_log->Log("FTP Command Failed: %s", aCommand.c_str());
			ps3_log->Do();
			exit(1);
		}
		
		return code;
	}
	
	return 0;
}