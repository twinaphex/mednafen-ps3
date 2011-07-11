#pragma once

class								Menu
{
	public:
									Menu						() : InputDelay(5)
		{
		}

		virtual						~Menu						()
		{
		}

		virtual bool				Draw						() = 0;
		virtual bool				Input						() = 0;

		void						SetInputDelay				(uint32_t aDelay)
		{
			InputDelay = aDelay;
		}
		
		void						Do							()
		{
			uint32_t lasthit = 0;

			while(!WantToDie())
			{
				uint32_t now = Utility::GetTicks();

				if(now > lasthit + (20 * InputDelay))
				{
					ESInput::Refresh();
	
					if(Input())
					{
						break;
					}

					lasthit = now;
				}
	
				if(Draw())
				{
					break;
				}

				ESVideo::Flip();
			}

			ESInput::Reset();
		}
		
	protected:
		uint32_t					InputDelay;
};

