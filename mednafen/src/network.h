#ifndef MDFNS3_NETWORK_H
#define MDFNS3_NETROWK_H



class						MednafenNetwork
{
	public:
							MednafenNetwork							(const char* aHost)
		{
			Socket = MakeSocket("192.168.0.250", "12345");
		}
	
		int					Socket;
		
};

#endif