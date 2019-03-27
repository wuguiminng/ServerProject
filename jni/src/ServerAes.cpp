/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: ServerAes.cpp
*				File Identify: decryption file with AES
*				Current Version: 1.0.4
*				Author(s)：wuguiming(joey)
***********************************************************************************************/

#include "ServerAes.h"


int Aes::AesDecryptFile(const unsigned char *ucp_EnBuff, int nEnSize,\
	unsigned char *ucp_DeBuff)
{
	
	int nRet = 0;
	int nTemp = 0;
	int nDeSize = 0;
	unsigned char ucaIvec[AES_BLOCK_SIZE];
	unsigned char ucaKey[AES_BLOCK_SIZE+1];
	memset(ucaKey, 0, sizeof(ucaKey));
	memcpy(ucaKey, "joeyjackjackjoey", 16);
	
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	
	for (int index = 0; index < AES_BLOCK_SIZE; index++)
	{
		ucaIvec[index] = 0;
	}
	
	nRet = EVP_DecryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, ucaKey, ucaIvec);
	if (1 != nRet)
	{
		cout << "EVP_DecryptInit_ex error" << endl;
		return -1;
	}
	
	nRet = EVP_DecryptUpdate(&ctx, ucp_DeBuff, &nDeSize, ucp_EnBuff, nEnSize);
	if (1 != nRet)
	{
		cout << "EVP_DecryptUpdate error" << endl;
		return -1;
	}

	nRet = EVP_DecryptFinal_ex(&ctx, ucp_DeBuff + nDeSize, &nTemp);
	nDeSize = nDeSize + nTemp;

	cout << "nDeSize = " << nDeSize << endl;
	EVP_CIPHER_CTX_cleanup(&ctx);

	return nDeSize;
}
