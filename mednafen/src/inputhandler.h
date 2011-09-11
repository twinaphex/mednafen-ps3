#pragma once

class												InputEnumerator
{
	public:
		virtual void								System							(const MDFNGI* aDescription) = 0;
		virtual void								FinishSystem					() {};

		virtual bool								Port							(const InputPortInfoStruct* aDescription) = 0;
		virtual void								FinishPort						() {};

		virtual bool								Device							(const InputDeviceInfoStruct* aDescription) = 0;
		virtual void								FinishDevice					() {};

		virtual void								Button							(const InputDeviceInputInfoStruct* aDescription) = 0;
};

class												InputHandler
{
	public:
		static void									SetGameInfo						(const MDFNGI* aSystem);
		static void									EnumerateInputs					(const MDFNGI* aSystem, InputEnumerator* aEnumerator);

		static void									Process							();
		
		static void									Configure						();
		static void									ReadSettings					();

		static void									GenerateSettings				(std::vector<MDFNSetting>& aSettings);
};

