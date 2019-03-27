/********************************************************************************************
*				Copyright(c) 2004,shenzhen SDMC Microelectronic
*
*				File Name: Client.c
*				File Identify: this is a client, can send xml, json,
							   picture, file or video to server
*				Current Version: 1.0.1
*				Author(s)：wuguiming
***********************************************************************************************/

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define MAX_SIZE 1024

/*
	Name：getCmd
	Function：get cmd from packageBuff
	Argument：
		@packageBuff:  data package from Client
	return ：success return cmd，error -1
*/
int getCmd(char packageBuff[])
{
	if (NULL == packageBuff)
	{
		return -1;
	}
	
	int m_cmd = bytesToInt(packageBuff + 4);
	
	if ((m_cmd >= 0) && (m_cmd <= 6))
	{
		return m_cmd;
	}
	
	printf("Package cmd is not correct \n");
	
	return -1;
}


/*
	Name：getDataSize
	Function：get the size of the file content.
	Argument：
		@packageBuff:  data package from Client
	return ：success return data size，error -1
*/
int getDataSize(char packageBuff[])
{
	if (NULL == packageBuff)
	{
		return -1;
	}
	
	int m_dataSize = bytesToInt(packageBuff + 8);
	
	if (m_dataSize > 0)
	{
		return m_dataSize;
	}
	
	printf("Package dataSize is not correct ");
	
	return -1;
}


/*
	Name：bytesToInt
	Function：Converts 4 bytes of bits to int.
	Argument：
		@Buff: 4 bytes character
	return ：success return int number，error -1
*/
int bytesToInt(char Buff[])
{
	int num = 0;
	int i;

	for (i = 0; i < 4; i++)
	{
		num <<= 8;
		num |= (Buff[i] & 0xff);
	}

    return num;
}


/*
	Name: intToBytes
	Function: Converts int number to 4 bytes of char
	Argument：
		@Buff: int number
	return: success return int num，error -1
*/
char *intToBytes(int num)
{
	int i;
	char *bytes = (char*)malloc(sizeof(int));

	for(i = 0; i < 4; i++)
	{
		int set = 32 - (i + 1) * 8;
		bytes[i] = (char) ((num >> set) & 0xff);
	}

	return bytes;
}


/*
	Name: sendMsg
	Function:send message to server
	Argument：
		@socketfd: server socket
		@sendMessage:message
	return: success return int num，error -1
*/
int sendMsg(int socketfd, char *sendMessage, int sendSize)
{
	if ((socketfd < 0) || (NULL == sendMessage) || (sendSize < 0))
	{
		return -1;
	}
	int ret = -1;
	ret = send(socketfd, sendMessage, sendSize, 0);
	printf("sendMessage = %s\n", sendMessage); //测试
	printf("wriet %d words\n", ret);
	return 0;
}


/*
	Name: clientInit
	Function:init a server socket
	Argument：
		@port: server port
		@ipAdd: server IP address
	return: success return server socket ，error -1
*/
int clientInit(char *port, char *ipAdd)
{
	if ((NULL == port) || (NULL == ipAdd))
	{
		return -1;
	}
	
	int ret = 0;
	int socketfd = 0;
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (-1 == socketfd)
	{
		perror("create socket error");
		return -1;
	}

	//init server informations
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET; //IPV4
	serverAddr.sin_port = htons(atoi(port)); //port
	serverAddr.sin_addr.s_addr = inet_addr(ipAdd); //ip
	

	ret = connect(socketfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	
	if (-1 == ret)
	{
		perror("socket connect error");
		return -1;
	}
	
	return socketfd;
}


/*
	Name：aes_encrypt
	Function：encryption the file of data
	Argument：
		@en_buff:  plaintext
		@en_size:  the size of plaintext
		@de_buff:  ciphertext
	return ：success return the size of ciphertext，error -1
*/
int aes_encrypt(unsigned char *de_buff, int de_size, unsigned char *en_buff)
{
	int i;
		int nRet = 0;
		int nTemp = 0;
		int en_size = 0; //生成的密文大小
		unsigned char ivec[AES_BLOCK_SIZE];
		unsigned char key[AES_BLOCK_SIZE+1];
		memset(key, 0, sizeof(key));
		memcpy(key, "joeyjackjackjoey", 16);


		//1、初始化一个EVP_CIPHER_CTX结构体，只有初始化后该结构体才能使用下面的函数
		EVP_CIPHER_CTX ctx;
		EVP_CIPHER_CTX_init(&ctx);

		for (i = 0; i < AES_BLOCK_SIZE ; i++)
		{
			ivec[i] = 0;
		}

		//2、设置并初始化加密结构体，  加密模式
		nRet = EVP_EncryptInit_ex(&ctx, EVP_aes_128_cbc(), NULL, key, ivec);
		if (1 != nRet)
		{
			printf("EVP_EncryptInit_ex error");
			return -1;
		}

		EVP_CIPHER_CTX_set_padding(&ctx, 2);

		//3、加密操作
		nRet = EVP_EncryptUpdate(&ctx, en_buff, &en_size, de_buff, de_size);
		if (1 != nRet)
		{
			printf("EVP_EncryptUpdate error");
			return -1;
		}

		//4、对最后一块进行填充 并进行加密处理
		nRet = EVP_EncryptFinal_ex(&ctx, en_buff + en_size, &nTemp);
		en_size = en_size + nTemp;

		printf("en_size = %d\n", en_size);

		//5、加密结束 清除EVP_CIPHER_CTX结构体的所有信息并是否内存
		EVP_CIPHER_CTX_cleanup(&ctx);

		return en_size;
}


/*
	Name：sendFile
	Function：send file data to server
	Argument：
		@socketfd:  server socket
		@cmd:  type of command  cmd == 2(xml) cmd == 3(json)  cmd == 4(bmp) cmd == 2(txt)
		@path:  path of file
		@flag: 1:no encryption >1: encryption
	return ：success 0，error -1
*/
int sendFile(int socketfd, int cmd, char *path, int flag)
{
	if ((socketfd < 0) || (cmd < 0) || (flag < 0) || (NULL == path))
	{
		return -1;
	}
	
	FILE *fp = NULL;
	int en_fileSize = 0;
	int read_len = 0;
	int send_len = 0;
	unsigned int fileSize = 0;
	char sendMessage[MAX_SIZE] = {0};
	bzero(sendMessage, MAX_SIZE);

	if (NULL == (fp = fopen(path, "r")))
	{
		perror("open file failed");
		return -1;
	}
	
	fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	memcpy(sendMessage, "SDMC", 4);
	memcpy(sendMessage+4, intToBytes(cmd), 4);

	char packBuff[MAX_SIZE] = {0};
	bzero(packBuff, MAX_SIZE);
	
	unsigned char de_buff[MAX_SIZE];
	memset((char*)de_buff, 0, MAX_SIZE);
	//
	if (2 == flag)
	{
		printf("cyphertext transmitting\n");		
		unsigned char en_buff[MAX_SIZE];
		memset((char*)en_buff, 0, MAX_SIZE);
		if (fileSize < 1008)
		{
			if (0 == (fileSize % 16))
			{
				en_fileSize = fileSize;
			}
			else
			{
				en_fileSize = ((fileSize / 16) + 1) * 16;
			}
		}
		else
		{
			en_fileSize = fileSize + ((fileSize / 1008) * 16) + (16 - (fileSize % 16));
		}
		

		printf("en_fileSize = %d\n", en_fileSize);
		memcpy(sendMessage+8, intToBytes(en_fileSize), 4);

		sendMsg(socketfd, sendMessage, 12);

		bzero(sendMessage, MAX_SIZE);
		//memcpy(sendMessage, intToBytes(fileSize), 4);
		//send(socketfd, sendMessage, 4, 0);
		
		int en_len = 0;
		send_len = 0;
		while(1)
		{
			memset((char*)en_buff, 0, MAX_SIZE);
			memset((char*)de_buff, 0, MAX_SIZE);
			if (fileSize <= 1008)
			{
				fread(de_buff, sizeof(char), fileSize, fp);
				en_len = aes_encrypt(de_buff, fileSize, en_buff);
				send_len += send(socketfd, en_buff, en_len, 0);
				break;
			}
			fread(de_buff, sizeof(char), MAX_SIZE-16, fp);
			aes_encrypt(de_buff, MAX_SIZE-16, en_buff);
			send_len += send(socketfd, en_buff, MAX_SIZE, 0);
			fileSize -= (MAX_SIZE - 16);
		}
		
		if (en_fileSize != send_len)
		{
			printf("send message error\n");
			return -1;
		}
		memset((char*)en_buff, 0, MAX_SIZE);
		printf("send message success\n");
	}
	else if (1 == flag)
	{
		memcpy(sendMessage+8, intToBytes(fileSize), 4);
		memcpy(packBuff, sendMessage, 12);
		

		sendMsg(socketfd, packBuff, 12);
		
		en_fileSize = fileSize;
		send_len = 0;
		while(1)
		{
			printf("send_len = %d fileSize = %d\n", send_len, fileSize);
			bzero(packBuff, MAX_SIZE);
			if (fileSize <= 1024)
			{
				fread(packBuff, sizeof(char), fileSize, fp);
				send_len += send(socketfd, packBuff, fileSize, 0);
				break;
			}
			fread(packBuff, sizeof(char), MAX_SIZE, fp);
			send_len += send(socketfd, packBuff, MAX_SIZE, 0);
			fileSize -= MAX_SIZE;
		}
		if (en_fileSize != send_len)
		{
			printf("send message error\n");
			return -1;
		}
		printf("send message success\n");
	}
	printf("read_len = %d\n", en_fileSize);
	
	bzero(sendMessage, MAX_SIZE);
	bzero(packBuff, MAX_SIZE);
	
	fclose(fp);
	return 0;
}


/*
	Name： sendUrl
	Function：send URL video link to server
	Argument：
		@socketfd: server socket
		@cmd:  type of command  cmd == 2(xml) cmd == 3(json)  cmd == 4(bmp) cmd == 2(txt)
		@url:  URL link
	return ：success 0，error -1
*/
int sendUrl(int socketfd, int cmd, char *url)
{
	if ((socketfd < 0) || (cmd < 0) || (NULL == url))
	{
		return -1;
	}
	
	int urlSizeLength = strlen(url);
	char sendMessage[MAX_SIZE];
	memset(sendMessage, 0, MAX_SIZE);
	
	memcpy(sendMessage, "SDMC", 4);
	memcpy(sendMessage + 4, intToBytes(cmd), 4);
	memcpy(sendMessage + 8, intToBytes(urlSizeLength), 4);
	sendMsg(socketfd, sendMessage, 12);
	memset(sendMessage, 0, MAX_SIZE);
	memcpy(sendMessage, url, urlSizeLength);
	
	send(socketfd, sendMessage, urlSizeLength, 0);
	
	return 0;
}



void menu(void)
{
	printf("\n=============== input the operation cmd ===================\n");
	printf("================= 0: request connect      ===================\n");
	printf("================= 1: disconnect           ===================\n");
	printf("================= 2: send xml file        ===================\n");
	printf("================= 3: send json file       ===================\n");
	printf("================= 4: send picture         ===================\n");
	printf("================= 5: send normal file     ===================\n");
	printf("================= 6: send url link        ===================\n");
}


/*
	Name：switchOperation
	Function：do operations
	Argument：
		@socketfd: server Socket
		@flag: 1:no encryption >1: encryption
	return ：success 0，error -1
	@cmd == 0: Request sending data
	@cmd == 1: disconnection
	@cmd == 2: send XML file
	@cmd == 3: send JSON file
	@cmd == 4: send bmp picture
	@cmd == 5: send txt file
	@cmd == 6: send URL download link
*/
int switchOperation(int socketfd, int flag)
{
	if ((socketfd < 0)|| (flag < 0))
	{
		return -1;
	}
	
	int ret = -1;
	int cmd = -1;
	int ack = -1;
	menu();
	scanf("%d", &cmd);
	
	char recvBuff[MAX_SIZE] = {0};
	char SendBuff[MAX_SIZE] = {0};
	bzero(SendBuff, sizeof(SendBuff));
	bzero(recvBuff, sizeof(recvBuff));

	printf("func=%s,line=%d\n",__FUNCTION__,__LINE__);
	switch (cmd)
	{
		case 1:
		{
			memcpy(SendBuff, "SDMC", 4);
			memcpy(SendBuff + 4, intToBytes(cmd), 4);
			memcpy(SendBuff + 8, intToBytes(1), 4);
			memcpy(SendBuff + 12, "1", 1);
			sendMsg(socketfd, SendBuff, 13);
			exit(0);
			break;
		}
		case 2:
		{
			char path[50] = {0};
			bzero(&path,sizeof(path));
			printf("please input the path of xml file\n");
			scanf("%s", path);
			
			ret = sendFile(socketfd, cmd , path, flag);
			if (-1 != ret)
			{
				recv(socketfd, recvBuff, sizeof(recvBuff), 0);
				printf("xml data from Server:  %s\n", recvBuff+12);
				bzero(recvBuff, sizeof(recvBuff));
			}
			else
			{
				return 0;
			}
			break;
		}
		case 3:
		{
			char path[50] = {0};
			bzero(&path,sizeof(path));
			printf("please input the path of json file\n");
			scanf("%s", path);
			
			ret = sendFile(socketfd, cmd , path, flag);
			if (-1 != ret)
			{
				int i;
				for (i = 0; i < 3; i++)
				{
					recv(socketfd, recvBuff, sizeof(recvBuff), 0);
					printf("json data from Server:  %s\n", recvBuff+12);
					bzero(recvBuff, sizeof(recvBuff));
				}
			}
			else
			{
				return 0;
			}
			break;
		}
		case 4:
		{
			char path[50] = {0};
			bzero(&path,sizeof(path));
			printf("please input the path of bmp picture\n");
			scanf("%s", path);
			
			ret = sendFile(socketfd, cmd , path, flag);
			if (-1 != ret)
			{
				recv(socketfd, recvBuff, sizeof(recvBuff), 0);
				sscanf(recvBuff+12,"%d",&ack);
				if (1 == ack)
				{
					printf("server save picture success\n");
				}
				else if(2 == ack)
				{
					printf("server save picture failue\n");
				}
			}
			else
			{
				return 0;
			}
			break;
		}
		case 5:
		{
			char path[50] = {0};
			bzero(&path,sizeof(path));
			printf("please input the path of txt file\n");
			scanf("%s", path);
			
			ret = sendFile(socketfd, cmd , path, flag);
			if (-1 != ret)
			{
				recv(socketfd, recvBuff, sizeof(recvBuff), 0);
				sscanf(recvBuff+12,"%d",&ack);
				if (1 == ack)
				{
					printf("server save picture success\n");
				}
				else if(2 == ack)
				{
					printf("server save picture failue\n");
				}
			}
			else
			{
				return 0;
			}
			break;
		}
 		case 6:
		{
			char type[10] = {0};
			char url[50] = {0};
			bzero(&type,sizeof(type));
			bzero(&url,sizeof(url));
			printf("please input the file url link(http://test.mp4)\n");
			scanf("%s",url);
			
			ret = sendUrl(socketfd, cmd, url);
			if (-1 != ret)
			{
				recv(socketfd, recvBuff, sizeof(recvBuff), 0);
				sscanf(recvBuff+12,"%d",&ack);
				if (1 == ack)
				{
					printf("server save picture success\n");
				}
				else if(2 == ack)
				{
					printf("server save picture failue\n");
				}
			}
			else
			{
				return 0;
			}
			break;
		}
		default:
		return 0;
	}
}


/*
	Name：interface
	Function：do some operations for client
	Argument：
		@socketfd:  server socket
	return ：success return command (0:reject 1:no encryption 2:encryption)
			error -1
*/
int interface(int socketfd)
{
	if (socketfd < -1)
	{
		perror("error socket");
		return -1;
	}
	
	int ret = -1;
	int cmd = -1;
	menu();
	scanf("%d", &cmd);
	
	int flag = -1;
	int dataSize = 0;
	char sdmc[5] = "SDMC";
	char dataBuff[MAX_SIZE] = {0};
	char SendBuff[MAX_SIZE] = {0};
	char recvBuff[MAX_SIZE] = {0};
	bzero(dataBuff, sizeof(dataBuff));
	bzero(SendBuff, sizeof(SendBuff));
	bzero(recvBuff, sizeof(recvBuff));
	
	switch(cmd)
	{
		case 0: // send request
		{
			memcpy(SendBuff, sdmc, 4);
			memcpy(SendBuff + 4, intToBytes(cmd), 4);
			memcpy(SendBuff + 8, intToBytes(1), 4);

			sendMsg(socketfd, SendBuff, 12);

			recv(socketfd, recvBuff, sizeof(recvBuff), 0);
			sscanf(recvBuff+12,"%d",&flag);
			printf("flag = %d\n", flag);
			
			if (0 == flag)
			{
				printf("The sever has refused the connection");
				memcpy(SendBuff, sdmc, 4);
				memcpy(SendBuff + 4, intToBytes(1), 4);
				memcpy(SendBuff + 8, intToBytes(1), 4);
				
				sendMsg(socketfd, SendBuff, 12);
				exit(0);
			}
			else if ((2 == flag) || (1 == flag))
			{
				printf("connect server success\n");
			}
			break;
		}
		
		case 1: //disconnection
		{
			memcpy(SendBuff, sdmc, 4);
			memcpy(SendBuff + 4, intToBytes(cmd), 4);
			memcpy(SendBuff + 8, intToBytes(1), 4);
			
			sendMsg(socketfd, SendBuff, 12);
			
			exit(0);
			break;
		}
		
		default:
		break;
	}

	return flag;
}


/*
	Name: main
	Argument:
		@argc: The number of parameters to be entered.
		@argv: The parameters needs to enter.
	run as：./clinetDemo xxx xxx.xxx.xxx.xxx
*/
int main(int argc, char *argv[])
{
	int socketfd = 0;
	
	if (argc < 3)
	{
		printf("usage:%s xxx xxx.xxx.xxx.xxx\n", argv[0]);
		return -1;
	}

	int flag = -1;
	socketfd = clientInit(argv[1], argv[2]);
	
	if (-1 != socketfd)
	{
		flag = interface(socketfd);
		if(-1 != flag)
		{
			while(1)
			{
				switchOperation(socketfd, flag);
			}
			printf("func=%s,line=%d\n",__FUNCTION__,__LINE__);
		}
	}
	close(socketfd);
	return 0;
}
