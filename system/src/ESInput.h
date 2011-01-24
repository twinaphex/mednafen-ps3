#ifndef ESINPUT_H
#define ESINPUT_H

class				ESInput
{
	public:
		virtual								~ESInput				(){};
	
		virtual uint32_t					PadCount				() = 0;
		virtual void						Reset					() = 0;
		
		virtual int32_t						GetAxis					(uint32_t aPad, uint32_t aAxis) = 0;

		virtual bool						ButtonPressed			(uint32_t aPad, uint32_t aButton) = 0;
		virtual bool						ButtonDown				(uint32_t aPad, uint32_t aButton) = 0;
		
		virtual uint32_t					GetAnyButton			(uint32_t aPad) = 0;
		virtual std::string					GetButtonName			(uint32_t aButton) = 0;
		virtual std::string					GetButtonImage			(uint32_t aButton) = 0;
		
	public:	//Helpers
		static void							RefreshButton			(uint32_t aState, uint32_t& aHeld, uint32_t& aSingle)
		{
			if(aState && aHeld == 0xFFFFFFFF)
			{
				return;
			}

			aHeld = aState ? 1 : 0;
		
			if(!aState)
			{
				aSingle = 0;
			}
			else if(aState && aSingle == 0)
			{
				aSingle = 1;
			}
		}

		static bool							HandleSingleState		(uint32_t aHeld, uint32_t& aSingle)
		{
			if(aHeld == 0xFFFFFFFF)
			{
				return false;
			}
			else if(aSingle == 1)
			{
				aSingle = 2;
				return aHeld;
			}
			else
			{
				return false;
			}
		}
};

#endif

