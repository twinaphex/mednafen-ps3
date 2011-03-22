#include <es_system.h>

										FileSelect::FileSelect				(const std::string& aHeader, std::vector<std::string>& aBookMarks, const std::string& aPath, MenuHook* aInputHook) : BookMarks(aBookMarks)
{
	Header = aHeader;
	InputHook = aInputHook;

	try
	{
		FileList* list = new FileList(Area(10, 10, 80, 80), aPath, aBookMarks);
		list->SetHeader("[%s] %s", Header.c_str(), aPath.c_str());
		Summerface* sface = new Summerface("FileList", list);
		sface->SetHook(InputHook);
		Lists.push(sface);
		Valid = true;
	}
	catch(ESException ex)
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
			
			if(list->GetSelected()->IntProperties["DIRECTORY"])
			{
				try
				{
					FileList* nlist = new FileList(Area(10, 10, 80, 80), list->GetSelected()->Properties["PATH"], BookMarks);
					nlist->SetHeader("[%s] %s", Header.c_str(), list->GetSelected()->Properties["PATH"].c_str());
					Summerface* sface = new Summerface("FileList", nlist);
					sface->SetHook(InputHook);
					Lists.push(sface);
				}
				catch(ESException ex)
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
		throw ESException("FileSelect::GetFile: FileSelect object is invalid.");
	}
}

bool									FileSelect::IsValid					()
{
	return Valid;
}

