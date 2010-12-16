#include <ps3_system.h>

								FTPItem::FTPItem								(const char* aName, const char* aPath, struct ftpparse aData) : ListItem(aName)
{
	Name = strdup(aName);
	Path = strdup(aPath);

	File = aData.flagtryretr;
	Directory = aData.flagtrycwd;
	
	LabelImage = Directory ? "FolderICON" : "FileICON";
}

								FTPItem::~FTPItem								()
{
	free(Name);
	free(Path);
}
	
const char*						FTPItem::GetPath								()
{
	return Path;
}


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

								FTPFileList::FTPFileList				(const char* aHost, const char* aPort, const char* aPath) : WinterfaceList("[Remote Files]")
{
	OutSocket = MakeSocket("192.168.0.250", "21");

	memset(Buffer, 0, 2048);
	read(OutSocket, Buffer, 2048);
	ps3_log->Log(Buffer);

	DoCommand("USER anonymous\n");
	ps3_log->Log(Buffer);
	ps3_log->Do();
	
	DoCommand("PASS \n");
	ps3_log->Log(Buffer);
	ps3_log->Do();
	
	sprintf(Buffer, "CWD %s\n", aPath);
	ps3_log->Log(Buffer);
	ps3_log->Do();	
	
	DoCommand(Buffer);
	ps3_log->Log(Buffer);
	ps3_log->Do();
	
	DoCommand("PWD\n");
	ps3_log->Log(Buffer);
	ps3_log->Do();		
	
	DoCommand("PASV\n");
	ps3_log->Log(Buffer);
	ps3_log->Do();		

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
	
	ps3_log->Do();
}

								FTPFileList::~FTPFileList				()
{
}

const char*						FTPFileList::GetFile					()
{
	return WasCanceled() ? "" : ((FTPItem*)GetSelected())->GetPath();
}

void							FTPFileList::DoCommand					(const char* aCommand, bool aResult)
{
	write(OutSocket, aCommand, strlen(aCommand));
	
	if(aResult)
	{
		memset(Buffer, 0, 2048);
		read(OutSocket, Buffer, 2048);
	}
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
	const char* result;

	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(*Lists.top()->GetFile() == 0)
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

								
						

