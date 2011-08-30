#pragma once

class												InputEnumerator
{
	public:
		virtual void								System							(const MDFNGI* aDescription) = 0;
		virtual bool								Port							(const InputPortInfoStruct* aDescription) = 0;
		virtual bool								Device							(const InputDeviceInfoStruct* aDescription) = 0;
		virtual void								Button							(const InputDeviceInputInfoStruct* aDescription) = 0;
		virtual void								Finish							() {};
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

