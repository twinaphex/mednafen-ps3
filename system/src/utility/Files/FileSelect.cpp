#include <ps3_system.h>

										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook) : BookMarks(aBookMarks)
{
	Header = aHeader;
	InputHook = aInputHook;

	try
	{
		FileList* list = new FileList(Header, aPath, BookMarks, InputHook);
		Lists.push(list);
		Valid = true;
	}
	catch(FileException ex)
	{
		ErrorDialog(ex.what()).Do();
		Valid = false;
	}
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
	if(Valid)
	{
		std::string result;
	
		while(!WantToDie())
		{
			Lists.top()->Do();
			
			if(Lists.top()->WasCanceled())
			{
				if(Lists.size() == 1)
				{
					return "";
				}
			
				delete Lists.top();
				Lists.pop();
				
				continue;
			}
			
			if(Lists.top()->IsDirectory())
			{
				try
				{
					FileList* list = new FileList(Header, Lists.top()->GetFile(), BookMarks, InputHook);
					Lists.push(list);
				}
				catch(FileException ex)
				{
					ErrorDialog(ex.what()).Do();
				}
			}
			else
			{
				return Lists.top()->GetFile();
				break;
			}
		}
	
		return "";
	}
	else
	{
		throw FileException("FileSelect::GetFile: FileSelect object is invalid.");
	}
}

bool									FileSelect::IsValid					()
{
	return Valid;
}

