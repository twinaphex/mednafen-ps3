#include <ps3_system.h>

										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook) : BookMarks(aBookMarks)
{
	Header = aHeader;
	InputHook = aInputHook;

	Lists.push(new FileList(Header, aPath, InputHook));
}

										FileSelect::~FileSelect				()
{
	while(Lists.size() != 0)
	{
		delete Lists.top();
		Lists.pop();
	}
}

std::string								FileSelect::GetFile					()
{
	std::string result;

	while(!WantToDie())
	{
		Lists.top()->Do();
		
		if(Lists.top()->GetFile().empty())
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
		
		if(Lists.top()->GetFile()[Lists.top()->GetFile().length() - 1] == '/')
		{
			Lists.push(new FileList(Header, Lists.top()->GetFile(), InputHook));
			continue;
		}

		result = Lists.top()->GetFile();
		break;
	}

	return result;
}
