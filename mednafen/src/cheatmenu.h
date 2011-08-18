#pragma once

///Class holding and editing the cheats for the loaded game.
class							CheatMenu
{
	class											Cheat;
	typedef AnchoredListView<Cheat>					CheatListType;

	private:
		///Class describing a Cheat as a SummerfaceItem.
		class					Cheat : public SummerfaceItem
		{
			public:
				///Create an empty cheat item with text indicating that no cheats are available.
								Cheat					() :
					SummerfaceItem("No Cheats Found", ""),
					Name(""),
					Address(0),
					Value(0),
					Compare(0),
					Status(0),
					Type(0),
					Length(0),
					BigEndian(false),
					SpaceHolder(true)
				{
				}

				///Create a new Cheat item.
				///@param aName Display name of the Cheat.
				///@param aAddress Memory address of the Cheat.
				///@param aValue Value of the Cheat.
				///@param aCompare Condition of the Cheat.
				///@param aStatus Determine if the Cheat is active.
				///@param aType Unknown...
				///@param aLength Number of bytes the Cheat should patch.
				///@param aBigEndian True if the Cheat should patch memory as big endian.
								Cheat					(char* aName, uint32_t aAddress, uint64_t aValue, uint64_t aCompare, int aStatus, int8_t aType, uint32_t aLength, bool aBigEndian) :
					SummerfaceItem("", ""),
					Name(aName),
					Address(aAddress),
					Value(aValue),
					Compare(aCompare),
					Status(aStatus),
					Type(aType),
					Length(aLength),
					BigEndian(aBigEndian),
					SpaceHolder(false)
				{
					std::stringstream str;
					str << Name << " (" << std::hex << std::uppercase << Address << std::dec << "->" << Value << ":" << Compare << ")";
					SetText(str.str());
				}

				///Override the image used for displaying the Cheat.
				///@return "CheckIMAGE" if the Cheat is enabled, "ErrorIMAGE" if it is disabled.
				std::string		GetImage				()
				{
					return SpaceHolder ? "" : (Status ? "CheckIMAGE" : "ErrorIMAGE");
				}

				///Add as a new Cheat to Mednafen. Cheat is always enabled after adding.
				void			Add						()
				{
					assert(!SpaceHolder);

					Status = 1;
					MDFNI_AddCheat(Name.c_str(), Address, Value, Compare, Type, Length, BigEndian);
				}

				///Insert the cheats values into mednafen.
				///@param aIndex Index of cheat to write to.
				void			Insert					(uint32_t aIndex)
				{
					assert(!SpaceHolder);
					MDFNI_SetCheat(aIndex, Name.c_str(), Address, Value, Compare, Status, Type, Length, BigEndian);
				}

			public:
				std::string		Name;					///<Friendly name of the cheat.
				uint32_t		Address;				///<Address to patch.
				uint64_t		Value;					///<Value to patch.
				uint64_t		Compare;				///<Condition of the cheat.
				int				Status;					///<True to enable cheat.
				int8_t			Type;					///<??
				uint32_t		Length;					///<Number of bytes to patch.
				bool			BigEndian;				///<Memory order of bytes.
				bool			SpaceHolder;			///<True for a fake cheat. Used to display messages.
		};


	public:
		///Create a new CheatMenu.
								CheatMenu				() :
			CheatList(new CheatListType(Area(10, 10, 80, 80))),
			Blank(false)
		{
			//Make the menu
			CheatList->SetHeader("Available Cheats");

			//Insert all of the cheats
			//NOTE: These MUST NOT be sorted, their index in the list is passed to mednafen
			MDFNI_ListCheats(AttachCheat, (void*)this);

			//Note if none were found
			if(CheatList->GetItemCount() == 0)
			{
				Blank = true;
				CheatList->AddItem(new Cheat());
			}
		}

		///Run the CheatMenu.
		void					Do						()
		{
			Summerface sface("CHEATS", CheatList);
			sface.AttachConduit(new SummerfaceTemplateConduit<CheatMenu>(this));
			sface.Do();
		}

		///Implement SummerfaceInputConduit to handle the CheatMenu.
		///@param aInterface Pointer to Summerface making the call.
		///@param aWindow Name of the active SummerfaceWindow.
		///@return 0: Ignore, 1: Eat, -1: Close Interface
		int						HandleInput				(Summerface* aInterface, const std::string& aWindow, uint32_t aButton)
		{
			//Get a pointer to the selected cheat
			Cheat* cheat = CheatList->GetSelected();

			//Toggle a cheat's status
			if(!Blank && aButton == ES_BUTTON_LEFT || aButton == ES_BUTTON_RIGHT)
			{
				cheat->Status = !cheat->Status;
				MDFNI_ToggleCheat(CheatList->GetSelection());
				return 1;
			}

			//Add a cheat
			if(!Blank && aButton == ES_BUTTON_ACCEPT)
			{
				//Get a new value for the cheat and tell mednafen about it
				int64_t value = cheat->Value;
				if(ESSUB_GetNumber(value, "Input new value for cheat.", 10, false))
				{
					cheat->Value = value;
					cheat->Insert(CheatList->GetSelection());
				}
				return 1;
			}

			//Delete a cheat
			if(!Blank && aButton == ES_BUTTON_SHIFT)
			{
				//Ask for confirmation
				std::stringstream str;
				str << "Really delete cheat: " << cheat->GetText();

				//Is accepted, delete the cheat and exit
				if(ESSUB_Confirm(str.str().c_str()))
				{
					MDFNI_DelCheat(CheatList->GetSelection());
					return -1;	//TODO: Don't leave, reorder the list and continue
				}
			}

			//Add a new cheat
			if(aButton == ES_BUTTON_TAB)
			{
				//Get the address to patch
				int64_t address = 0;
				while(ESSUB_GetNumber(address, "Enter address to patch (in hex)", 8, true))
				{
					//Get the new value
					int64_t value = 0;
					while(ESSUB_GetNumber(value, "Enter value to patch to (in decimal)", 10, false))
					{
						//Get the number of bytes
						int64_t bytes = 1;
						while(ESSUB_GetNumber(bytes, "Enter number of bytes to patch", 1, false))
						{
							//Handle case where bytes is invalid
							if(bytes == 0 || bytes > 8)
							{
								ESSUB_Error("Bytes value must be between 1 and 8 inclusive.");
							}
							else
							{
								//Get endianess
								bool bigendian = false, canceled = false;
								if(bytes != 1)
								{
									bigendian = ESSUB_Confirm("Is memory big-endian?", &canceled);
									if(canceled)
									{
										continue;
									}
								}

								//Get name and add it
								std::string name = ESSUB_GetString("Enter name for the cheat", "");
								if(!name.empty())
								{
									MDFNI_AddCheat(name.c_str(), address, value, 0, 'R', bytes, bigendian);
									return -1; //Close the cheat menu to hide the fact that this cheat won't be visible until it's reloaded...
								}
							}
						}
					}
				}
			}

			return 0;
		}

	private:
		///Static callback function for MDFNI_ListCheats.
		static int				AttachCheat				(char* aName, uint32_t aAddress, uint64_t aValue, uint64_t aCompare, int aStatus, char aType, uint32_t aLength, bool aBigEndian, void* aData)
		{
			if(aData)
			{
				CheatMenu* menu = (CheatMenu*)aData;
				menu->CheatList->AddItem(new Cheat(aName, aAddress, aValue, aCompare, aStatus, aType, aLength, aBigEndian));
			}

			return 1;
		}

	private:
		CheatListType*			CheatList;				///<List of Cheats.
		bool					Blank;					///<True if no Cheats were found.
};


