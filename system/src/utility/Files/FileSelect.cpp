#include <ps3_system.h>

//TODO: Move this
void ESSUB_Error(const char* aMessage);

										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook) : BookMarks(aBookMarks)
{
	Header = aHeader;
	InputHook = aInputHook;

	try
	{
		FileList* list = new FileList(Area(10, 10, 80, 80), Header, aPath, aBookMarks);
		Summerface* sface = new Summerface("FileList", list);
		Lists.push(sface);
		Valid = true;
	}
	catch(FileException ex)
	{
		ESSUB_Error(ex.what());
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
			FileList* list = (FileList*)Lists.top()->GetWindow("FileList");			

			if(list->WasCanceled())
			{
				if(Lists.size() == 1)
				{
					return "";
				}
			
				delete Lists.top();
				Lists.pop();
				
				continue;
			}
			
			if(list->GetSelected()->Properties["DIRECTORY"] == "1")
			{
				try
				{
					FileList* nlist = new FileList(Area(10, 10, 80, 80), Header, list->GetSelected()->Properties["PATH"], BookMarks);
					Summerface* sface = new Summerface("FileList", nlist);
					Lists.push(sface);
				}
				catch(FileException ex)
				{
					ESSUB_Error(ex.what());
				}
			}
			else
			{
				return list->GetSelected()->Properties["PATH"];
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

