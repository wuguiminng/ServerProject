/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerAes.h
*				File Identify: decryption file with AES
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#ifndef SERVERAES_H_
#define SERVERAES_H_

#include "ServerPublic.h"
#include <openssl/aes.h>
#include <openssl/evp.h>

class Aes
{
public:
	Aes() 
	{
		
	}
	
	virtual ~Aes() 
	{
		
	}
	
	int AesDecryptFile(const unsigned char *ucp_EnBuff, int nEnSize,\
		unsigned char *ucp_DeBuff);
	
private:	
	
};

#endif 
