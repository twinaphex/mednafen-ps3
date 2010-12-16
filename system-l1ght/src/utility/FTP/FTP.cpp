#include <ps3_system.h>

namespace
{
	bool								AlphaSortC								(ListItem* a, ListItem* b)
	{
		if(((FTPItem*)a)->Directory && !((FTPItem*)b)->Directory)
		{
			return true;
		}

		if(((FTPItem*)b)->Directory && !((FTPItem*)a)->Directory)
		{
			return false;
		}

	
		return strcmp(a->GetText(), b->GetText()) <= 0;
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


								FTPItem::FTPItem								(const char* aName, const char* aPath, struct ftpparse aData) : ListItem(aName)
{
	Path = strdup(aPath);

	File = aData.flagtryretr;
	Directory = aData.flagtrycwd;
	
	SetImage(Directory ? "FolderICON" : "FileICON");
}

								FTPItem::~FTPItem								()
{
	free(Path);
}
	
const char*						FTPItem::GetPath								()
{
	return Path;
}



								FTPFileList::FTPFileList				(const char* aHost, const char* aPort, const char* aPath) : WinterfaceList("[Remote Files]")
{
	OutSocket = MakeSocket("192.168.0.250", "21");

	memset(Buffer, 0, 2048);
	read(OutSocket, Buffer, 2048);

	if(DoCommand("USER anonymous\n") != 331)
	{
		ps3_log->Log("FTP 'USER' command failed: %s", Buffer);
		ps3_log->Do();
		exit(1);
	}
	
	if(DoCommand("PASS \n") != 230)
	{
		ps3_log->Log("FTP 'PASS' command failed: %s", Buffer);
		ps3_log->Do();
		exit(1);
	}
	
	sprintf(Buffer, "CWD %s\n", aPath);
	if(DoCommand(Buffer) != 250)
	{
		ps3_log->Log("FTP 'CWD' command failed: %s", Buffer);
		ps3_log->Do();	
		exit(1);
	}
	
	if(DoCommand("PASV\n") != 227)
	{
		ps3_log->Log("FTP 'PASV' command failed: %s", Buffer);
		ps3_log->Do();		
		exit(1);
	}

	char* parse = strchr(Buffer, '(') + 1;
	uint32_t a, b, c, d, e, f;
	sscanf(parse, "%d,%d,%d,%d,%d,%d", &a, &b, &c, &d, &e, &f);
	char newtarget[200];
	char newport[10];			
	sprintf(newtarget, "%d.%d.%d.%d", a,b,c,d);
	sprintf(newport, "%d", e * 256 + f);

	InSocket = MakeSocket(newtarget, newport);
	
	DoCommand("LIST\n", false);
	
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

const char*						FTPFileList::GetFile					()
{
	return WasCanceled() ? 0 : ((FTPItem*)GetSelected())->GetPath();
}

uint32_t						FTPFileList::DoCommand					(const char* aCommand, bool aResult)
{
	write(OutSocket, aCommand, strlen(aCommand));
	
	if(aResult)
	{
		memset(Buffer, 0, 2048);
		read(OutSocket, Buffer, 2048);
		
		uint32_t code;
		sscanf(Buffer, "%d", &code);
		return code;
	}
	
	return 0;
}

								FTPSelect::FTPSelect					(const char* aHeader, const char* aHost, const char* aPort, MenuHook* aInputHook)
{
	Header = strdup(aHeader);
	Host = strdup(aHost);
	Port = strdup(aPort);

	InputHook = aInputHook;
	Lists.push(new FTPFileList(Host, Port, "/"));
}

								FTPSelect::~FTPSelect					()
{
	while(Lists.size() != 0)
	{
		delete Lists.top();
		Lists.pop();
	}
	
	free(Host);
	free(Port);
	free(Header);
}


const char*						FTPSelect::GetFile						()
{
	const char* result = 0;

	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(Lists.top()->GetFile() == 0)
		{
			if(Lists.size() > 1)
			{
				delete Lists.top();
				Lists.pop();
			}
			else
			{
				break;
			}
			
			continue;
		}
		
		const char* out = Lists.top()->GetFile();
		if(out[strlen(out) - 1] == '/')
		{
			Lists.push(new FTPFileList(Host, Port, Lists.top()->GetFile()));
			continue;
		}

		result = Lists.top()->GetFile();
		break;
	}

	return result;
}

								
						

