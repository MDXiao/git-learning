#include <iostream>
#include <sys/types.h>
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

#include <string.h> //For Strings
#include <dirent.h> //For directories
#include <stdio.h> // For remove() in delete
#include <fstream> //For checking the file size
#include <sstream> //For string to num conversions

#include <pthread.h>
#include <sys/stat.h>

#include <ctime>

/* You will have to modify the program below */

#define MAXBUFSIZE 6144

struct threadArgs {
	char buf[MAXBUFSIZE];
	int sockN;
	char thisDir[MAXBUFSIZE];
};

/*
 * Source: https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

void getField(char buf[], char ret[],int option)
{
	int i;
	int j;
	int start = 0;
	int loc;
	bool found = false;
	if(option == 1) // Get Username
	{
		char check[] = "USERNAME:";
		for(i = 0; i < strlen(buf)-2; i++)
		{
			loc = i;
			if(buf[i] == 'U')
			{
				for(j = i; j < strlen(check)+i; j++)
				{
					if(buf[j] == check[start])
					{	
						found = true;
						loc++;
						start++;
					}
					else
					{
						start = 0;
						found = false;
						loc = i;
						break;
					}	
				}
			}
			if(found == true)
			{
				break;
			}
		}
	}
	else // Get Password
	{
		char check[] = "PASSWORD:";
		for(i = 0; i < strlen(buf)-2; i++)
		{
			loc = i;
			if(buf[i] == 'P')
			{
				for(j = i; j < strlen(check)+i; j++)
				{
					if(buf[j] == check[start])
					{	
						found = true;
						loc++;
						start++;
					}
					else
					{
						start = 0;
						found = false;
						loc = i;
						break;
					}	
				}
			}
			if(found == true)
			{
				break;
			}
		}
	}

	start = 0;
	for(i = loc+1; i < strlen(buf)-1;i++)
	{
		if(buf[i] == '\n')
			break;
		ret[start] = buf[i];
		start++;
	}
}


bool validateUser(char buf[], int sock, char toSend[])
{
	char userN[MAXBUFSIZE];
	char passN[MAXBUFSIZE];

	memset(&userN, 0, sizeof(userN));
	memset(&passN, 0, sizeof(passN));

	getField(buf, userN, 1);
	getField(buf, passN, 2);
	FILE *confFile = fopen("dfs.conf", "r");

	char fileReader[MAXBUFSIZE];
	char userF[MAXBUFSIZE];
	char passF[MAXBUFSIZE];

	memset(&userF, 0, sizeof(userF));
	memset(&passF, 0, sizeof(passF));

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", userF, passF);
		// printf("USERN: %s USERF: %s\n", userN, userF);
		// printf("PASSN: %s PASSF: %s\n", passN, passF);
		if(strcmp(userF, userN) == 0)
		{
			if(strcmp(passF, passN) == 0)
			{
				printf("User Sucessfully Authenticated\n");
				strcpy(toSend, "\nAuthentication Successful\n\n");
				return true;
			}
		}
			
	}
	printf("Failed to authenticate user.\n");
	return false;
}

void getCommand(char buf[], char cmd[])
{
	int i;
	int j = 0;
	for(i = 9; i < strlen(buf)-2; i++)
	{
		if(buf[i] == '\n' || buf[i] == '\0')
			break;
		cmd[j] = buf[i];
		j++;
	}	
}

void stripHeader(char res[])
{
	std::string str(res);
	int index = 0;
	int start = 0;
	int end = 0;
	int i;
	for(i = 0; i < 100; i++)
	{
		printf("IN BUFFFFF: %c\n", res[i]);
		if(res[i] == '\n')
		{	
			printf("i: %i\n", i);
			index = i;
			break;
		}
	}
	str.erase(0,index);
	printf("Index: %i ERASED STRING: %s\n",index, str.c_str());
}

int listContents(char servDir[], char buf[], char toSend[])
{
	char userN[MAXBUFSIZE];
	getField(buf, userN, 1);

	char userDir[50];
	strcpy(userDir, servDir);
	strcat(userDir,"/");
	strcat(userDir, userN);

	char fDir[50];
	int count = 1;


	strcpy(fDir, ".");
	strcat(fDir, userDir);

	DIR *dir;
	struct dirent *ent;
	printf("fDir: %s\n", fDir);
	
	char response[MAXBUFSIZE];
	memset(&response, 0, sizeof(response));

	strcpy(response, "LIST\n\n");

	if((dir = opendir(fDir)) != NULL)
	{
		while((ent = readdir (dir)) != NULL)
		{
			if((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name,"..") != 0))
			{
				// std::stringstream ss;
				// ss << count;
				// std::string str = ss.str();

				// strcat(response, str.c_str());
				strcat(response, ent->d_name);
				strcat(response, "\n\n");		
				count++;
			}
		}
		closedir(dir);
	}
	else
	{
		const int dir_err = mkdir(fDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (-1 == dir_err)
		{
		    printf("Error creating directory!n");
		    return -1;
		}
	}

	strcat(toSend, response);

	return 0;
}

int getContents(char servDir[], char fileToGet[], char res[], char buf[], int sock)
{
	char userN[MAXBUFSIZE];
	getField(buf, userN, 1);

	char userDir[50];
	strcpy(userDir, servDir);
	strcat(userDir,"/");
	strcat(userDir, userN);

	char fDir[50];
	int count = 1;


	strcpy(fDir, ".");
	strcat(fDir, userDir);

	DIR *dir;
	struct dirent *ent;
	
	char response[MAXBUFSIZE];
	memset(&response, 0, sizeof(response));

	// memset(&res, 0, strlen(res));

	bool exists = false;

	if((dir = opendir(fDir)) != NULL)
	{
		while((ent = readdir (dir)) != NULL)
		{
			if((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name,"..") != 0))
			{
				if(strcmp(ent->d_name, fileToGet) == 0)
				{
					exists = true;
				}
			}
		}
		closedir(dir);
	}
	else
	{
		strcpy(res, "That directory does not yet exist.\n");
		return -1;
	}

	if(exists == false)
	{
		strcpy(res, "The File you are trying to get does not exist.");
		send(sock, res, strlen(res), 0);
		return -1;
	}

	strcat(fDir,"/");
	strcat(fDir, fileToGet);

	int sizeFile = filesize(fDir);

	// std::ostringstream sFile;

	// sFile << sizeFile;

	// strcpy(response, sFile.str().c_str());

	// send(sock, response, strlen(response), 0);

	FILE *myFile = fopen(fDir, "rb");
	// printf("OPENING: %s\n", fDir);

	int totalBytes = 0;
	int nbytes;

	// printf("SIZEFILE: %i\n", sizeFile);
	fseek(myFile, 0, SEEK_SET);
	// while(totalBytes < sizeFile)
	// {
	// 	memset(&response, 0, sizeof(response));
	// 	int read = fread(response, 1, sizeof(response), myFile);
	// 	nbytes = send(sock, response, read, MSG_DONTWAIT);
	// 	// printf("Sending :%s\n", response);
	// 	// printf("nBYTES: %i\n", nbytes);
	// 	totalBytes += nbytes;
	// 	 // printf("TOTAL o: %s\n", response);
	// }
	memset(&response, 0, sizeof(response));
	int read = fread(response, 1, sizeFile, myFile);

	// printf("RES: %s\n", response);
	strcpy(res, response);
	send(sock, response, read, MSG_DONTWAIT);
	// memset(&response, 0, sizeof(response));
	fclose(myFile);

	return read;
}

int putContents(char servDir[], char fileToGet[], char res[], char buf[], int sock)
{
	char userN[MAXBUFSIZE];
	memset(&userN, 0, sizeof(userN));
	getField(buf, userN, 1);

	char userDir[50];
	strcpy(userDir, servDir);
	strcat(userDir,"/");
	strcat(userDir, userN);

	char fDir[50];

	strcpy(fDir, ".");
	strcat(fDir, userDir);
	
	char fragP[50];
	char header[50];
	char newDir[1024];
	char sizeHeader[50];
	char fSize[50];

	memset(&fragP, 0, sizeof(fragP));
	memset(&header, 0, sizeof(header));
	memset(&newDir, 0, sizeof(newDir));	
	memset(&sizeHeader, 0, sizeof(sizeHeader));	

	strcat(fDir,"/.");
	strcat(fDir, fileToGet);
	strcat(fDir, ".");

	// std::ofstream myFile;
	

	recv(sock, header, 22, 0); //CHANGE THIS LINE TO ACCOMDATE FOR FILE SIZES
	//22 FOR ONLY 3 character file sizes (512 bytes)
	//23 FOR ONLY 4 chacater file sizes (1024+ bytes)

	sscanf(header, "%s %s %s %s", newDir, fragP, sizeHeader, fSize);
	strcpy(newDir, fDir);
	strcat(newDir, fragP);
	printf("header: %s\n", header);
	// printf("FragP: %s\n", fragP);
	// printf("fSize: %s\n", fSize);

	// myFile.open(newDir, std::ofstream::binary);
	// int fileSize = atoi(fSize);
	printf("newDir: %s\n", newDir);

	// memset(&response, 0, sizeof(response));
	FILE *myFile = fopen(newDir, "wb");

	char response[MAXBUFSIZE];
	char newResponse[MAXBUFSIZE];
	memset(&response, 0, sizeof(response));
	int totalBytes = 0;
	int nbytes;

	// printf("SIZEFILE: %i\n", fileSize);
	// printf("FRAGMENT NUM: %i\n", fragNum);

	char *output = NULL;
	bool newFrag = false;
	int firstFrag = 0;
	while(1)
	{
		// printf("firstFrag: %i\n",firstFrag);
		if(firstFrag == 0)
		{
			nbytes = recv(sock, response, atoi(fSize), 0);
		}
		else if(firstFrag == 1)
		{
			nbytes = recv(sock, response, strlen(header), 0);

			fclose(myFile);
			sscanf(response, "%s %s", header, fragP);
			strcpy(newDir, fDir);
			strcat(newDir, fragP); 
			// printf("NEWDIR: %s\n", newDir);
			newFrag = true;

			FILE *myFile = fopen(newDir, "wb");
		}
		else
		{
			nbytes = recv(sock, response, atoi(fSize), 0);
		}
		// printf("CURRENT DIR: %s\n", newDir);
		// printf("header (IN LOOP): %s\n", header);
		// sleep(1);

		if(firstFrag == 0 || firstFrag == 2)
		{
			// output = strstr(response, "Fragment:");

			// if(output)
			// {
			// 	// printf("Response: %s\n", response);
			// 	// myFile.close();
			// 	fclose(myFile);
			// 	sscanf(output, "%s %s", header, fragP);
			// 	strcpy(newDir, fDir);
			// 	strcat(newDir, fragP); 
			// 	printf("NEWDIR: %s\n", newDir);
			// 	newFrag = true;

			// 	FILE *myFile = fopen(newDir, "wb");
			// 	// myFile.open(newDir, std::ofstream::binary);
				
			// 	totalBytes = 0;
			// }
			// else
			// {
			// 	newFrag = false;
			// }
			// printf("BUF: %s\n", response);

			if(nbytes == 0 || nbytes < 0)
			{
				break;
			}
			// printf("Response: %s\n", response);
			// int write = fwrite(response, 1, sizeof(response), myFile);
			// if(newFrag == true)
			// {
			// 	// stripHeader(response);
			// 	printf("response (fragmented): %s\n", response);
			// 	fwrite(response, 1, nbytes-26, myFile);
			// 	totalBytes += nbytes;
			// 	printf("totalBytes: %i\n", totalBytes);
			// 	memset(&response, 0, sizeof(response));
			// 	memset(&newResponse, 0, sizeof(newResponse));
			// }
			// else
			// {
				// printf("response: %s\n", response);
				// myFile.write(response, nbytes);
				fwrite(response, 1, nbytes, myFile);

				totalBytes += nbytes;
				printf("totalBytes (None Fragmented): %i\n", totalBytes);
				memset(&response, 0, sizeof(response));
			// }
		}
		if(firstFrag > 2)
			break;
		firstFrag++;

	}
	// printf("MADE IT OUT\n");
	memset(&response, 0, sizeof(response));
	// myFile.close();
	fclose(myFile);

	return 0;
}

void* handleRequest(void *args)
{
		struct threadArgs *thisArgs = (struct threadArgs*) args;

		char cmd[50];
		char failedRes[] = "Invalid Username/Password. Please try again.\n";
		char response[MAXBUFSIZE];
		char fileName[MAXBUFSIZE];
		char field[50];

		printf("BUFFER:\n%s\n", thisArgs->buf);

		// for(int i = 0 ; i < strlen(cmd); i++)
		// {
		// 	printf("BUFFFFFFFFFF :%c\n", cmd[i]);
		// }
		while(1)
		{
				memset(&cmd, 0 , sizeof(cmd));
				getCommand(thisArgs->buf, cmd);
				printf("CMD: %s\n", cmd);
				if(strcmp(cmd, "exit") == 0 || strcmp(cmd, "EXIT ") == 0)
				{
						printf("Closing Socket connection for Socket Number: %i\n", thisArgs->sockN);
						close(thisArgs->sockN);
						break;			
				}
				else
				{
					if(validateUser(thisArgs->buf, thisArgs->sockN,response))
					{
						sscanf(thisArgs->buf, "%s %s %s", field, cmd, fileName);
						if(strcmp(cmd, "list") == 0 || strcmp(cmd, "LIST") == 0)
						{
							memset(&thisArgs->buf, 0 , sizeof(thisArgs->buf));
							//recv(connfd, buffer,sizeof(buffer), 0);
							if(listContents(thisArgs->thisDir, thisArgs->buf, response) != 0)
								printf("LISTING OF DIRECTORY FAILED!\n");		
							printf("Response: \n%s\n", response);
		
							send(thisArgs->sockN, response, sizeof(response), 0);
							//close(thisArgs->sockN);
						}
						else if(strcmp(cmd, "get") == 0 || strcmp(cmd, "GET") == 0)
						{
							//printf("CMD: %s FILENAME: %s\n", cmd, fileName);
							// bzero(&response, sizeof(response));
							int sendSize = getContents(thisArgs->thisDir, fileName, response, thisArgs->buf, thisArgs->sockN);
							// if(sendSize < 0)
							// 	printf("Getting contents failed!\n");
							// printf("SEND SIZE: %i\n", sendSize);
							// send(thisArgs->sockN, response, sendSize, 0);	
							// printf("SENT: %s\n", response);						
						}
						else if(strcmp(cmd, "put") == 0 || strcmp(cmd, "PUT") == 0)
						{
							printf("Putting\n");
							putContents(thisArgs->thisDir, fileName, response, thisArgs->buf, thisArgs->sockN);
						}
					}
					else
					{
						send(thisArgs->sockN, failedRes, sizeof(failedRes), 0);	
					}
				}	
				memset(&cmd, 0 , sizeof(cmd));
				memset(&thisArgs->buf, 0, sizeof(thisArgs->buf));
				recv(thisArgs->sockN, thisArgs->buf, sizeof(thisArgs->buf), 0);
		}

}


int main (int argc, char * argv[])
{
	int sock;                           //This will be our socket
	struct sockaddr_in cliAddr, servAddr;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	if (argc != 3)
	{
		printf ("USAGE: ./dfs <DIR> <PORTNUMBER>\n");
		exit(1);
	}

	char dirName[MAXBUFSIZE];
	strcpy(dirName, argv[1]);

	bzero(&servAddr,sizeof(servAddr));                    //zero the struct
	servAddr.sin_family = AF_INET;                   //address family
	servAddr.sin_port = htons(atoi(argv[2]));        //htons() sets the port # to network byte order
	servAddr.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("unable to create socket");
	}

	if (bind(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		printf("unable to bind socket\n");
	}

	if(listen(sock, 10) < 0)
	{
			printf("Failed to Listen\n");
	}

	

	socklen_t cliLen = sizeof(cliAddr);
	int rc;
	pthread_t threads[150];
	int threadCount = 0;
	struct threadArgs newArgs;

	while(int connfd = accept(sock, (struct sockaddr*) &cliAddr, &cliLen))
	{

		// int connfd = accept(sock, (struct sockaddr*) &cliAddr, &cliLen);

		recv(connfd, buffer, sizeof(buffer), 0);

		strcpy(newArgs.buf, buffer);
		newArgs.sockN = connfd;
		strcpy(newArgs.thisDir, argv[1]);

		rc = pthread_create(&threads[threadCount], NULL, handleRequest, &newArgs);	
		
		if(rc < 0)
			printf("Thread Creation failed\n");

		pthread_join(threads[threadCount], NULL);
		threadCount++;

		memset(&buffer, 0 , sizeof(buffer));
	}
	close(sock);
}

