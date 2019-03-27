/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: main.cpp
*				File Identify: The beginning of the whole program.
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/
#include "ServerSocket.h"

int main(void)
{
	Server server;
	
	server.ServerInit();
	server.AcceptClient();
	
	return 0;
}
