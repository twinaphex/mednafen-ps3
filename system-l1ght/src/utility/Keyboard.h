#ifndef SYSTEM__KEYBOARD_H
#define SYSTEM__KEYBOARD_H

class										Keyboard : public Winterface
{
	public:
											Keyboard				(std::string aHeader, std::string aText);
		virtual								~Keyboard				();

		bool								DrawLeft				();
		bool								Input					();
				
		std::string							GetText					()						{return Text;};
		void								SetText					(std::string aText)		{Text = aText;};

		std::string							GetHeader				()						{return Header;};
		void								SetHeader				(std::string aHeader)	{Header = aHeader;};
				
		bool								WasCanceled				()						{return Canceled;};
				
		
	protected:
		std::string							Text;
		std::string							Header;
		
		int32_t								Column;
		int32_t								Row;
		uint32_t							Shift;
		
		bool								Canceled;
};

#endif