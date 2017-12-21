#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>

#include <string.h> //For Strings
#include <dirent.h> //For directories
#include <stdio.h> // For remove() in delete
#include <fstream> //For checking the file size
#include <stdlib.h>
#include <sstream> //For string to num conversions
#include <openssl/md5.h>


#define MAXBUFSIZE 6144

/*
 * Source: https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

int getHash(char fileName[]) {
	FILE* put_file = fopen(fileName, "rb"); 
	unsigned char hash[MD5_DIGEST_LENGTH], buf[MAXBUFSIZE]; 
    MD5_CTX mdContext; 
    int bytes, i;
  	unsigned long mod_hash = 0, mod = 4;

    MD5_Init(&mdContext);

    while ((bytes = fread (buf, 1, 1024, put_file)) != 0) {
        MD5_Update(&mdContext, buf, bytes);
    }

    MD5_Final(hash,&mdContext);

    for(i = 0; i < MD5_DIGEST_LENGTH + 1; i++) {
    	int cur_val = (hash[i] > '9')?(hash[i] &~0x20)-'A'+10:(hash[i] - '0'); 
    	mod_hash = mod_hash*16 + cur_val;

    	if (mod_hash >= mod) {
    		mod_hash %= mod;
    	}
    }

    return (int)mod_hash;
}

int sendAll(int* sockets, char msg[], int msgLen)
{	
	int i;
	int result;
	int numSocks = (sizeof(sockets)/sizeof(sockets[0])-1);

	// printf("NUM SOCKS: %i\n", numSocks);
	for(i = 0; i < numSocks; i++)
	{
		result = send(sockets[i], msg, msgLen, 0);
		// printf("RESULTS: %i\n", result);
	}
	return result;
}

int recvAll(int* sockets, char buf[], int bufLen)
{
	int i;
	int result = 0;
	int numSocks = (sizeof(sockets)/sizeof(sockets[0]) -1);
	for(i = 0; i < numSocks; i++)
	{
		result = recv(sockets[i], buf, bufLen, 0);
	}

	return result;
}

void sendHeader(int sockets[], char command[], char configFile[])
{
	char request[MAXBUFSIZE];
	strcpy(request, "COMMAND: ");
	strcat(request, command);
	strcat(request, "\n");

	char fileReader[MAXBUFSIZE];
	char lineRead[MAXBUFSIZE];
	char contents[MAXBUFSIZE];
	char userSend[100];
	char passSend[100];


	memset(&userSend, 0 , sizeof(userSend));
	memset(&passSend, 0 , sizeof(passSend));

	FILE *confFile = fopen(configFile, "r");

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", lineRead,contents);
		if(strcmp(lineRead, "Username:") == 0)
		{
			strcpy(userSend, "USERNAME: ");
			strcat(userSend, contents);
			strcat(request, userSend);
			strcat(request, "\n");
			memset(&contents, 0 , sizeof(contents));
		}
		else if(strcmp(lineRead, "Password:") == 0)
		{
			strcpy(passSend, "PASSWORD: ");
			strcat(passSend, contents);
			strcat(request, passSend);
			strcat(request, "\n");
			memset(&contents, 0 , sizeof(contents));
			memset(&passSend, 0 , sizeof(passSend));
		}
	}
	// printf("\n%s\n", request);

	// int result = sendAll(sockets, request, strlen(request));

	for(int i = 0; i < 4; i++)
	{
		send(sockets[i], request, sizeof(request), 0);
	}

	memset(&request, 0 , strlen(request));
}


int parsePort(char address[])
{
	char parsed[MAXBUFSIZE];
	int counter = 0;

	for(int i = 0; i < strlen(address); i++)
	{
		if(address[i] == ':')
		{
			for(int j = i + 1; j < strlen(address); j++)
			{
				parsed[counter] = address[j];
				counter += 1;
			}
			break;
		}
	}

	return atoi(parsed);

}

void setConnections(int socks[], struct sockaddr_in addr, char configFile[])
{
	char portNum[10];
	char fileReader[MAXBUFSIZE];
	char lineRead[MAXBUFSIZE];
	char dirName[MAXBUFSIZE];
	char servAddr[MAXBUFSIZE];
	int i = 0;

	FILE *confFile = fopen(configFile, "r");

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s %s", lineRead, dirName, servAddr);
		if(strcmp(lineRead, "Server") == 0)
		{
			// printf("PORT: %i\n", parsePort(servAddr));
			bzero(&addr,sizeof(addr));         
			addr.sin_family = AF_INET;       
			addr.sin_port = htons(parsePort(servAddr)); // Create Helper      
			addr.sin_addr.s_addr = INADDR_ANY; 

			if ((socks[i] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			{
				printf("unable to create socket");
				exit(1);
			}

			if (connect(socks[i], (struct sockaddr *)&addr, sizeof(addr)) < 0)
			{
				close(socks[i]);
				//printf("CONNECTION FAILED!\n");
			}

			i = i + 1;
		}
	}
	fclose(confFile);
}

void splitFileGet(char fileName[], int* socks, char configFile[])
{
	int partsGot[4];
	partsGot[0] = 1;
	partsGot[1] = 2;
	partsGot[2] = 3;
	partsGot[3] = 4;

	char req[100];
	strcpy(req, "get .");
	int nbytes1, nbytes2, nbytes3, nbytes4;

	FILE *myFile = fopen(fileName, "wb");

	/*
	 * TODO in Get
	 *  Check hash value
	 *  based on hash value- hard code the 
	 */

	int parts = 1;

	char toWrite1[MAXBUFSIZE];
	char toWrite2[MAXBUFSIZE];
	char toWrite3[MAXBUFSIZE];
	char toWrite4[MAXBUFSIZE];

	memset(&toWrite1, 0, sizeof(toWrite1));
	memset(&toWrite2, 0, sizeof(toWrite2));
	memset(&toWrite3, 0, sizeof(toWrite3));
	memset(&toWrite4, 0, sizeof(toWrite4));


	if(parts == 1)
	{
		strcat(req, fileName);
		strcat(req, ".1");
		// printf("req: %s\n", req);
		sendHeader(socks, req, configFile);

		nbytes1 = recv(socks[0], toWrite1, sizeof(toWrite1), 0);
		nbytes2 = recv(socks[1], toWrite2, sizeof(toWrite2), 0);
		nbytes3 = recv(socks[2], toWrite3, sizeof(toWrite3), 0);
		nbytes4 = recv(socks[3], toWrite4, sizeof(toWrite4), 0);

		// printf("recved: %s\n", toWrite3);
		if(nbytes1 > 100 && strcmp(toWrite1, "\0") != 0)
		{
			// printf("SERV 1 has fragment 1\n");
			 // printf("recved: %s\n", toWrite1);
			fwrite(toWrite1, 1, nbytes1, myFile);
		}
		else
		{
			if(nbytes2 > 100 && strcmp(toWrite2, "\0") != 0)
			{
				// printf("SERV 2 has fragment 1\n");
				fwrite(toWrite2, 1, nbytes2, myFile);
			}
			else
			{
				if(nbytes3 > 100 && strcmp(toWrite3, "\0") != 0)
				{
					printf("SERV 3 has fragment 1\n");
					fwrite(toWrite3, 1, nbytes3, myFile);
					printf("WROTE: %s\nand %i bytes\n", toWrite3, nbytes3);
				}
				else
				{
					if(nbytes4 > 100 && strcmp(toWrite4, "\0") != 0)
					{
						// printf("SERV 4 has fragment 1\n");
						fwrite(toWrite4, 1, nbytes4, myFile);
					}
				}
			}
		}
	}

	memset(&toWrite1, 0, sizeof(toWrite1));
	memset(&toWrite2, 0, sizeof(toWrite2));
	memset(&toWrite3, 0, sizeof(toWrite3));
	memset(&toWrite4, 0, sizeof(toWrite4));
	parts++;

	memset(&req, 0, sizeof(req));
	if(parts == 2)
	{
		strcpy(req, "get .");
		strcat(req, fileName);
		strcat(req, ".2");
		// printf("req: %s\n", req);

		sendHeader(socks, req, configFile);
		nbytes1 = recv(socks[0], toWrite1, sizeof(toWrite1), 0);

		// printf("recved: %s\n", toWrite1);
		nbytes2 = recv(socks[1], toWrite2, sizeof(toWrite2), 0);
		nbytes3 = recv(socks[2], toWrite3, sizeof(toWrite3), 0);
		nbytes4 = recv(socks[3], toWrite4, sizeof(toWrite4), 0);

		// printf("recved: %s\n", toWrite1);
		if(nbytes1 > 100 && strcmp(toWrite1, "\0") != 0)
		{
			// printf("SERV 1 has fragment 2\n");
			// printf("BUF RECVEd: %s\n", toWrite1);
			fwrite(toWrite1, 1, nbytes1, myFile);
		}
		else
		{
			if(nbytes2 > 100 && strcmp(toWrite2, "\0") != 0)
			{
				// printf("SERV 2 has fragment 2\n");
				fwrite(toWrite2, 1, nbytes2, myFile);
			}
			else
			{
				if(nbytes3 > 100 && strcmp(toWrite3, "\0") != 0)
				{
					// printf("SERV 3 has fragment 2\n");
					fwrite(toWrite3, 1, nbytes3, myFile);
				}
				else
				{
					if(nbytes4 > 100 && strcmp(toWrite4, "\0") != 0)
					{
						// printf("SERV 4 has fragment 2\n");
						fwrite(toWrite4, 1, nbytes4, myFile);
					}
				}
			}
		}
	}

	memset(&toWrite1, 0, sizeof(toWrite1));
	memset(&toWrite2, 0, sizeof(toWrite2));
	memset(&toWrite3, 0, sizeof(toWrite3));
	memset(&toWrite4, 0, sizeof(toWrite4));
	parts++;

	memset(&req, 0, sizeof(req));
	if(parts == 3)
	{
		strcpy(req, "get .");
		strcat(req, fileName);
		strcat(req, ".3");
		// printf("req: %s\n", req);

		sendHeader(socks, req, configFile);
		nbytes1 = recv(socks[0], toWrite1, sizeof(toWrite1), 0);

		// printf("recved: %s\n", toWrite1);
		nbytes2 = recv(socks[1], toWrite2, sizeof(toWrite2), 0);
		nbytes3 = recv(socks[2], toWrite3, sizeof(toWrite3), 0);
		nbytes4 = recv(socks[3], toWrite4, sizeof(toWrite4), 0);

		if(nbytes1 > 100 && strcmp(toWrite1, "\0") != 0)
		{
			// printf("SERV 1 has fragment 3\n");
			fwrite(toWrite1, 1, nbytes1, myFile);
		}
		else
		{
			if(nbytes2 > 100 && strcmp(toWrite2, "\0") != 0)
			{
				// printf("SERV 2 has fragment 3\n");
				fwrite(toWrite2, 1, nbytes2, myFile);
			}
			else
			{
				if(nbytes3 > 100 && strcmp(toWrite3, "\0") != 0)
				{
					// printf("SERV 3 has fragment 3\n");
					fwrite(toWrite3, 1, nbytes3, myFile);
				}
				else
				{
					if(nbytes4 > 100 && strcmp(toWrite4, "\0") != 0)
					{
						// printf("SERV 4 has fragment 3\n");
						fwrite(toWrite4, 1, nbytes4, myFile);
					}
				}
			}
		}
	}

	parts++;

	memset(&toWrite1, 0, sizeof(toWrite1));
	memset(&toWrite2, 0, sizeof(toWrite2));
	memset(&toWrite3, 0, sizeof(toWrite3));
	memset(&toWrite4, 0, sizeof(toWrite4));

	memset(&req, 0, sizeof(req));

	if(parts == 4)
	{
		strcpy(req, "get .");
		strcat(req, fileName);
		strcat(req, ".4");
			// printf("req: %s\n", req);

		sendHeader(socks, req, configFile);
		nbytes1 = recv(socks[0], toWrite1, sizeof(toWrite1), 0);

		// printf("recved: %s\n", toWrite1);
		nbytes2 = recv(socks[1], toWrite2, sizeof(toWrite2), 0);
		nbytes3 = recv(socks[2], toWrite3, sizeof(toWrite3), 0);
		nbytes4 = recv(socks[3], toWrite4, sizeof(toWrite4), 0);

		if(nbytes1 > 100 && strcmp(toWrite1, "\0") != 0)
		{
			// printf("SERV 1 has fragment 4\n");
			fwrite(toWrite1, 1, nbytes1, myFile);
		}
		else
		{
			if(nbytes2 > 100 && strcmp(toWrite2, "\0") != 0)
			{
				// printf("SERV 2 has fragment 4\n");
				fwrite(toWrite2, 1, nbytes2, myFile);
			}
			else
			{
				if(nbytes3 > 100 && strcmp(toWrite3, "\0") != 0)
				{
					// printf("SERV 3 has fragment 4\n");
					fwrite(toWrite3, 1, nbytes3, myFile);
				}
				else
				{
					if(nbytes4 > 100 && strcmp(toWrite4, "\0") != 0)
					{
						// printf("SERV 4 has fragment 4\n");
						fwrite(toWrite4, 1, nbytes4, myFile);
					}
				}
			}
		}
	}
	fclose(myFile);

}

void listContents(int* socks)
{

	bool incomplete = false;

	int ele[4]; //To check if fragment exists
	ele[0] = 1;
	ele[1] = 2;
	ele[2] = 3;
	ele[3] = 4;

	char serv1[MAXBUFSIZE];
	char serv2[MAXBUFSIZE];
	char serv3[MAXBUFSIZE];
	char serv4[MAXBUFSIZE];

	int nbytes1;
	int nbytes2;
	int nbytes3;
	int nbytes4;

	char alreadyListed[1024];
	bzero(&alreadyListed, sizeof(alreadyListed));

	nbytes1 = recv(socks[0], serv1, sizeof(serv1), 0);
	nbytes2 = recv(socks[1], serv2, sizeof(serv2), 0);
	nbytes3 = recv(socks[2], serv3, sizeof(serv3), 0);
	nbytes4 = recv(socks[3], serv4, sizeof(serv4), 0);

	printf("\nLIST\n\n");

	char * found = NULL;

	// printf("nbytes list: 1. %i\n2. %i\n3. %i\n4. %i\n", nbytes1, nbytes2, nbytes3, nbytes4);


	if(nbytes1 <= 0)
	{
		if(nbytes2 <= 0)
		{
			if(nbytes3 <= 0)
			{
				std::string ser1(serv4);
				std::istringstream val1(ser1);
			}
			else
			{
				std::string ser1(serv3);
				std::istringstream val1(ser1);
			}
		}
		else
		{
			std::string ser1(serv2);
			std::istringstream val1(ser1);
		}
	}
	else
	{
		std::string ser1(serv1);
		std::istringstream val1(ser1);
	}
	std::string ser1(serv1);
	std::istringstream val1(ser1);

	if (nbytes1 > 0)
	{
		std::string ser1(serv1);
		std::istringstream val1(ser1);
		
		char lineNum[50];
		char fileName1[50];
		char fileName2[50];
		char val[50];
		char *endingNum;

		std::string line;

		while(getline(val1, line))
		{
			// printf("INDEX: %i\n", atoi(line.substr(0,1).c_str()));
			if(strcmp(line.substr(0,1).c_str(), ".") == 0)
			{
				// printf("LIST LINE: %s\n", line.c_str());
				sscanf(line.c_str(), "%s", val);
				char* endVal;
				endVal = strrchr(val, '.');
				char serFile01[50];
				char serFile02[50];
				char* endVal01;
				char* endVal02;
				char *endingNum01;
				char *endingNum02;

				sscanf(val, ".%s", fileName1);
				bzero(&fileName2, sizeof(fileName2));
				strncpy(fileName2, fileName1, strlen(fileName1)-2);
				endingNum = endVal;
				endingNum++;

				int fragVal = atoi(endingNum);

				found = strstr(serv1, fileName2);

				// printf("FileName: %s\n FOUND IN SERV1: %s\n", fileName2,found);

				sscanf(found, "%s %s", serFile01,serFile02);

				// printf("fileName1: %s fileName2: %s\n", serFile01, serFile02);

				endVal01 = strrchr(serFile01, '.');
				endVal02 = strrchr(serFile02, '.');

					// printf("ENDVAL1 :%s and ENDVAL2 : %s\n", endVal1,endVal2);

				endingNum01 = endVal01;
				endingNum02 = endVal02;

				endingNum01++;
				endingNum02++;


				for(int i = 0; i < 4; i++)
				{
					if(atoi(endingNum01) == ele[i] || atoi(endingNum02) == ele[i])
					{
						ele[i] = 0;
					}
				}

				// printf("ALREADY LISTED: %s\n", alreadyListed);
				if(nbytes2 > 0)
				{
					char serFile1[50];
					char serFile2[50];
					char* endVal1;
					char* endVal2;
					char *endingNum1;
					char *endingNum2;
					
					found = strstr(serv2, fileName2);

					// printf("SERV2: %s FILENAME2: %s FOUND: %s\n", serv2, fileName2, found);

					sscanf(found, "%s %s", serFile1,serFile2);

					// printf("fileName1: %s fileName2: %s\n", serFile1, serFile2);

					endVal1 = strrchr(serFile1, '.');
					endVal2 = strrchr(serFile2, '.');

					// printf("ENDVAL1 :%s and ENDVAL2 : %s\n", endVal1,endVal2);

					endingNum1 = endVal1;
					endingNum2 = endVal2;

					endingNum1++;
					endingNum2++;


					for(int i = 0; i < 4; i++)
					{	
						if(atoi(endingNum1) == ele[i] || atoi(endingNum2) == ele[i])
						{
							ele[i] = 0;
						}
					}
					if(nbytes3 > 0)
					{
						char serFile11[50];
						char serFile22[50];
						char* endVal11;
						char* endVal22;
						char *endingNum11;
						char *endingNum22;
						
						found = strstr(serv3, fileName2);

						// printf("SERV2: %s FILENAME2: %s FOUND: %s\n", serv2, fileName2, found);

						sscanf(found, "%s %s", serFile11, serFile22);

						// printf("fileName1: %s fileName2: %s\n", serFile1, serFile2);

						endVal11 = strrchr(serFile11, '.');
						endVal22 = strrchr(serFile22, '.');

						// printf("ENDVAL1 :%s and ENDVAL2 : %s\n", endVal1,endVal2);

						endingNum11 = endVal11;
						endingNum22 = endVal22;

						endingNum11++;
						endingNum22++;


						for(int i = 0; i < 4; i++)
						{	
							if(atoi(endingNum11) == ele[i] || atoi(endingNum22) == ele[i])
							{
								ele[i] = 0;
							}
						}

						if(nbytes4 > 0)
						{
							char serFile111[50];
							char serFile222[50];
							char *endVal111;
							char *endVal222;
							char *endingNum111;
							char *endingNum222;
							
							found = strstr(serv4, fileName2);

							// printf("SERV4: %s FILENAME2: %s FOUND: %s\n", serv4, fileName2, found);

							sscanf(found, "%s %s", serFile111, serFile222);

							// printf("fileName1: %s fileName2: %s\n", serFile1, serFile2);

							endVal111 = strrchr(serFile111, '.');
							endVal222 = strrchr(serFile222, '.');

							// printf("ENDVAL1 :%s and ENDVAL2 : %s\n", endVal111,endVal222);


							endingNum111 = endVal111;
							endingNum222 = endVal222;

							endingNum111++;
							endingNum222++;

							// printf("ENDING NUMS: %s %s\n", endingNum111, endingNum222);

							for(int i = 0; i < 4; i++)
							{	
								if(atoi(endingNum111) == ele[i] || atoi(endingNum222) == ele[i])
								{
									ele[i] = 0;
								}
							}
						}
					}

			}
			
				char *doesExist = strstr(alreadyListed, fileName2);
				if(doesExist == NULL)
				{
					// for(int i = 0; i < 4; i++)
					// {
					// 	printf("ELE: %i\n", ele[i]);
					// }	
					for(int i = 0; i < 4; i++)
					{
						if(ele[i] != 0)
						{
							printf("%s [incomplete]\n\n", fileName2);
							incomplete = true;
							break;
						}
					}
					if(incomplete != true)
					{
						printf("%s\n\n", fileName2);					
					}
					incomplete = false;		
					strcat(alreadyListed, "\n");
					strcat(alreadyListed, fileName2);
					ele[0] = 1;	
					ele[1] = 2;	
					ele[2] = 3;	
					ele[3] = 4;
				}	
		}
		
	}
	}
	else
	{
		// printf("RECEIVED NOTHING FROM SERVER 1 - IS SERVER 1 DOWN?\n");
	}


	// printf("FROM SERVER 1: %s\nFROM SERVER 2: %s\n", serv1,serv2);

}

void splitFilePut(char fileName[], int* socks)
{
	FILE *myFile = fopen(fileName, "rb");

	// std::ifstream myFile (fileName, std::ifstream::binary);

	int fileSize = filesize(fileName);
	int fSize1, fSize2, fSize3, finalS;

	if(fileSize % 4 != 0) //If the file can't be split into 4 equal pieces
	{
		fSize1 = fileSize /4;
		fSize2 = fSize1, fSize3 = fSize1;
		finalS = fSize1 + (fileSize %4);

		// printf("fSize1: %i fSize2: %i fSize3: %i fSize4: %i\n", fSize1, fSize2, fSize3, finalS);
	}
	else //If the file can be split into 4 equal pieces
	{
		fSize1 = fileSize/4;
		fSize2 = fSize1, fSize3 = fSize1, finalS = fSize1;
	}

	int fAr[4];
	fAr[0] = fSize1;
	fAr[1] = fSize2;
	fAr[2] = fSize3;
	fAr[3] = finalS;

	// for(int c = 0; c < 4; c++)
	// {
	// 	printf("fARRR: %i\n", fAr[c]);
	// }

	//Send the filename as a header to the contents of the file

	char contents[MAXBUFSIZE];
	bzero(&contents, sizeof(contents));

	int transferred = 0;
	int totalBytes = 0;
	int i;
	int nbytes1, nbytes2;

	int md5F = getHash(fileName);
	// int md5F = 2;
	printf("MD5 HASH OF FILE: %i\n", md5F);

	while(transferred < fileSize)
	{	
		fseek(myFile, 0, SEEK_SET);
			if(md5F == 0)
			{
				std::stringstream ss;

				fread(contents, 1, fAr[0], myFile);
				ss << fAr[0];
				char toSend1[] = "Fragment: 1 Size: ";
				strcat(toSend1, ss.str().c_str());
				strcat(toSend1, "\n");
				// printf("FREAD 1: %s\n", contents);
				send(socks[0], toSend1, strlen(toSend1), 0);
				send(socks[3], toSend1, strlen(toSend1), 0);
				nbytes1 = send(socks[0], contents, fAr[0], 0);
				nbytes2 = send(socks[3], contents, fAr[0], 0);
				transferred += fAr[0];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[1], myFile);
				// printf("FREAD 2: %s\n", contents);
				ss << fAr[1];
				char toSend2[] = "Fragment: 2 Size: ";
				strcat(toSend2, ss.str().c_str());
				strcat(toSend2, "\n");
				send(socks[0], toSend2, strlen(toSend2), 0);
				send(socks[1], toSend2, strlen(toSend2), 0);
				nbytes1 = send(socks[0], contents, fAr[1], 0);
				nbytes2 = send(socks[1], contents, fAr[1], 0);
				transferred += fAr[1];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[2], myFile);
				// printf("FREAD 3: %s\n", contents);
				char toSend3[] = "Fragment: 3 Size: ";
				ss << fAr[2];
				strcat(toSend3, ss.str().c_str());
				strcat(toSend3, "\n");
				// printf("toSend: %s\n", toSend3);
				send(socks[1], toSend3, strlen(toSend3), 0);
				send(socks[2], toSend3, strlen(toSend3), 0);
				nbytes1 = send(socks[1], contents, fAr[2], 0);
				nbytes2 = send(socks[2], contents, fAr[2], 0);
				transferred += fAr[2];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[3], myFile);
				// printf("FREAD 4: %s\n", contents);
				char toSend4[] = "Fragment: 4 Size: ";
				ss << fAr[3];
				strcat(toSend4, ss.str().c_str());
				strcat(toSend4, "\n");
				send(socks[2], toSend4, strlen(toSend4), 0);
				send(socks[3], toSend4, strlen(toSend4), 0);
				nbytes1 = send(socks[2], contents, fAr[3], 0);
				nbytes2 = send(socks[3], contents, fAr[3], 0);
				transferred += fAr[3];
				bzero(&contents, sizeof(contents));

			}
			if(md5F == 1)
			{
				std::stringstream ss;

				fread(contents, 1, fAr[0], myFile);
				ss << fAr[0];
				char toSend1[] = "Fragment: 1 Size: ";
				strcat(toSend1, ss.str().c_str());
				strcat(toSend1, "\n");
				// printf("FREAD 1: %s\n", contents);
				send(socks[0], toSend1, strlen(toSend1), 0);
				send(socks[1], toSend1, strlen(toSend1), 0);
				nbytes1 = send(socks[0], contents, fAr[0], 0);
				nbytes2 = send(socks[1], contents, fAr[0], 0);
				transferred += fAr[0];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[1], myFile);
				// printf("FREAD 2: %s\n", contents);
				ss << fAr[1];
				char toSend2[] = "Fragment: 2 Size: ";
				strcat(toSend2, ss.str().c_str());
				strcat(toSend2, "\n");
				send(socks[1], toSend2, strlen(toSend2), 0);
				send(socks[2], toSend2, strlen(toSend2), 0);
				nbytes1 = send(socks[1], contents, fAr[1], 0);
				nbytes2 = send(socks[2], contents, fAr[1], 0);
				transferred += fAr[1];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[2], myFile);
				// printf("FREAD 3: %s\n", contents);
				char toSend3[] = "Fragment: 3 Size: ";
				ss << fAr[2];
				strcat(toSend3, ss.str().c_str());
				strcat(toSend3, "\n");
				// printf("toSend: %s\n", toSend3);
				send(socks[2], toSend3, strlen(toSend3), 0);
				send(socks[3], toSend3, strlen(toSend3), 0);
				nbytes1 = send(socks[2], contents, fAr[2], 0);
				nbytes2 = send(socks[3], contents, fAr[2], 0);
				transferred += fAr[2];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[3], myFile);
				// printf("FREAD 4: %s\n", contents);
				char toSend4[] = "Fragment: 4 Size: ";
				ss << fAr[3];
				strcat(toSend4, ss.str().c_str());
				strcat(toSend4, "\n");
				send(socks[0], toSend4, strlen(toSend4), 0);
				send(socks[3], toSend4, strlen(toSend4), 0);
				nbytes1 = send(socks[0], contents, fAr[3], 0);
				nbytes2 = send(socks[3], contents, fAr[3], 0);
				transferred += fAr[3];
				bzero(&contents, sizeof(contents));
			}
			if(md5F == 2)
			{
				std::stringstream ss;

				fread(contents, 1, fAr[0], myFile);
				ss << fAr[0];
				char toSend1[] = "Fragment: 1 Size: ";
				strcat(toSend1, ss.str().c_str());
				strcat(toSend1, "\n");
				// printf("FREAD 1: %s\n", contents);
				send(socks[1], toSend1, strlen(toSend1), 0);
				send(socks[2], toSend1, strlen(toSend1), 0);
				nbytes1 = send(socks[1], contents, fAr[0], 0);
				nbytes2 = send(socks[2], contents, fAr[0], 0);
				transferred += fAr[0];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[1], myFile);
				// printf("FREAD 2: %s\n", contents);
				ss << fAr[1];
				char toSend2[] = "Fragment: 2 Size: ";
				strcat(toSend2, ss.str().c_str());
				strcat(toSend2, "\n");
				send(socks[2], toSend2, strlen(toSend2), 0);
				send(socks[3], toSend2, strlen(toSend2), 0);
				nbytes1 = send(socks[2], contents, fAr[1], 0);
				nbytes2 = send(socks[3], contents, fAr[1], 0);
				transferred += fAr[1];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[2], myFile);
				// printf("FREAD 3: %s\n", contents);
				char toSend3[] = "Fragment: 3 Size: ";
				ss << fAr[2];
				strcat(toSend3, ss.str().c_str());
				strcat(toSend3, "\n");
				// printf("toSend: %s\n", toSend3);
				send(socks[3], toSend3, strlen(toSend3), 0);
				send(socks[0], toSend3, strlen(toSend3), 0);
				nbytes1 = send(socks[3], contents, fAr[2], 0);
				nbytes2 = send(socks[0], contents, fAr[2], 0);
				transferred += fAr[2];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[3], myFile);
				// printf("FREAD 4: %s\n", contents);
				char toSend4[] = "Fragment: 4 Size: ";
				ss << fAr[3];
				strcat(toSend4, ss.str().c_str());
				strcat(toSend4, "\n");
				send(socks[0], toSend4, strlen(toSend4), 0);
				send(socks[1], toSend4, strlen(toSend4), 0);
				nbytes1 = send(socks[0], contents, fAr[3], 0);
				nbytes2 = send(socks[1], contents, fAr[3], 0);
				transferred += fAr[3];
				bzero(&contents, sizeof(contents));
			}
			if(md5F == 3)
			{
				std::stringstream ss;

				fread(contents, 1, fAr[0], myFile);
				ss << fAr[0];
				char toSend1[] = "Fragment: 1 Size: ";
				strcat(toSend1, ss.str().c_str());
				strcat(toSend1, "\n");
				// printf("FREAD 1: %s\n", contents);
				send(socks[2], toSend1, strlen(toSend1), 0);
				send(socks[3], toSend1, strlen(toSend1), 0);
				nbytes1 = send(socks[2], contents, fAr[0], 0);
				nbytes2 = send(socks[3], contents, fAr[0], 0);
				transferred += fAr[0];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[1], myFile);
				// printf("FREAD 2: %s\n", contents);
				ss << fAr[1];
				char toSend2[] = "Fragment: 2 Size: ";
				strcat(toSend2, ss.str().c_str());
				strcat(toSend2, "\n");
				send(socks[3], toSend2, strlen(toSend2), 0);
				send(socks[0], toSend2, strlen(toSend2), 0);
				nbytes1 = send(socks[3], contents, fAr[1], 0);
				nbytes2 = send(socks[0], contents, fAr[1], 0);
				transferred += fAr[1];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[2], myFile);
				// printf("FREAD 3: %s\n", contents);
				char toSend3[] = "Fragment: 3 Size: ";
				ss << fAr[2];
				strcat(toSend3, ss.str().c_str());
				strcat(toSend3, "\n");
				// printf("toSend: %s\n", toSend3);
				send(socks[0], toSend3, strlen(toSend3), 0);
				send(socks[1], toSend3, strlen(toSend3), 0);
				nbytes1 = send(socks[0], contents, fAr[2], 0);
				nbytes2 = send(socks[1], contents, fAr[2], 0);
				transferred += fAr[2];
				bzero(&contents, sizeof(contents));
				ss.str(std::string());

				fread(contents, 1, fAr[3], myFile);
				// printf("FREAD 4: %s\n", contents);
				char toSend4[] = "Fragment: 4 Size: ";
				ss << fAr[3];
				strcat(toSend4, ss.str().c_str());
				strcat(toSend4, "\n");
				send(socks[1], toSend4, strlen(toSend4), 0);
				send(socks[2], toSend4, strlen(toSend4), 0);
				nbytes1 = send(socks[1], contents, fAr[3], 0);
				nbytes2 = send(socks[2], contents, fAr[3], 0);
				transferred += fAr[3];
				bzero(&contents, sizeof(contents));
			}
	}	
// 	fflush(myFile);
// 	fclose(myFile);
}




int main (int argc, char * argv[])
{
                            // number of bytes send by sendto()
	int socks[MAXBUFSIZE];                               //this will be our socket
	char buffer[MAXBUFSIZE];

	struct sockaddr_in addr;              //"Internet socket address structure"

	if (argc < 2)
	{
		printf("USAGE: ./dfc CONFIGFILE\n");
		exit(1);
	}

	setConnections(socks, addr, argv[1]);

	char command[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];

	for(;;)
	{
		printf("Please enter one of the commands:\n1. LIST\n2. GET\n3. PUT\n4. EXIT\n");
		scanf("%s", command);
		if(strcmp(command, "LIST") == 0 || strcmp(command, "list") == 0)
		{
			sendHeader(socks, command, argv[1]);
			
			listContents(socks);
			//printf("RECVED: %i\n", recved);
			// printf("%s\n", buffer);
			
		}
		else if(strcmp(command, "GET") == 0 || strcmp(command, "get") == 0)
		{
			scanf("%s", fileName);
			strcat(command, " ");
			strcat(command, fileName);

			// sendHeader(socks, command, argv[1]);

			memset(&buffer, 0 , sizeof(buffer));
			// int recved = recvAll(socks, buffer, sizeof(buffer));

			splitFileGet(fileName, socks, argv[1]);
			// printf("RECVED: %i\n", recved);
			// printf("%s\n", buffer);

			// int fileSize = atoi(buffer);
			// int totalBytes = 0;

			// FILE * myFile = fopen(fileName, "wb");

			// char fileBufs[MAXBUFSIZE];
			// int nbytes;

			// while(totalBytes < fileSize)
			// {
			// 	memset(&fileBufs, 0, sizeof(fileBufs));
			// 	nbytes = recvAll(socks, fileBufs, sizeof(fileBufs));
			// 	int write = fwrite(fileBufs, 1, nbytes, myFile);
			// 	totalBytes += nbytes;
			// }
			// fclose(myFile);
			
			// if(res < 0)
			// 	printf("FAILED TO SEND MESSAGE!\n");
			
		}
		else if(strcmp(command, "PUT") == 0 || strcmp(command, "put") == 0)
		{
			//send(socks[0], "Exit", strlen("Exit"), 0);
			scanf("%s", fileName);
			strcat(command, " ");
			strcat(command, fileName);
			sendHeader(socks, command, argv[1]);
			
			DIR *dir;
			struct dirent *ent;
			bool exists = false;

			std::string fileStr (fileName);
		
			if((dir = opendir(".")) != NULL)
			{
				while((ent = readdir (dir)) != NULL)
				{			
					//if(strcmp(fileN, ent->d_name) == 0)
					if(strcmp(fileName, ent->d_name) == 0)
					{
						exists = true;

						printf("File exists, preparing for transfer...\n\n");
					}
					
				}
				closedir(dir);
			}
			else
			{
				perror("");
			}


			// int totalBytes = 0;
			// char fileBufs[MAXBUFSIZE];
			// int nbytes;

			// printf("SIZEFILE: %i\n", sizeFile);
			splitFilePut(fileName, socks);
			// while(totalBytes < sizeFile)
			// {
			// 	int read = fread(fileBufs, 1, sizeof(fileBufs), myFile);
			// 	nbytes = sendAll(socks, fileBufs, read);
			// 	printf("FILEBUFS :%s\n", fileBufs);
			// 	// sleep(1);
			// 	totalBytes += nbytes;	
			// 	printf("totalBytes: %i and file size: %i\n", totalBytes, sizeFile);
			// 	memset(&fileBufs, 0, sizeof(fileBufs));
			// }
			// fclose(myFile);
			
		}
		else if(strcmp(command, "EXIT") == 0 || strcmp(command, "exit") == 0)
		{
			int numSocks = (sizeof(socks)/sizeof(socks[0]) -1);
			sendHeader(socks, command, argv[1]);
			for(int i = 0; i < numSocks; i++)
			{
				close(socks[i]);
			}
			break;
		}
		memset(&buffer, 0 , sizeof(buffer));
		memset(&command, 0, sizeof(command));
	}

}

