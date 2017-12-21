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
#include <stdlib.h>
#include <sstream> //For string to num conversions

/* You will have to modify the program below */

#define MAXBUFSIZE 1024


int ls(char output[])
{
	DIR *dir;
	struct dirent *ent;
	
	if((dir = opendir("./serverContents")) != NULL)
	{
		while((ent = readdir (dir)) != NULL)
		{
			if((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name,"..") != 0))
			{
				strcat(output, ent->d_name);
				strcat(output, " ");		
			}
		}
		closedir(dir);
	}
	else
	{
		perror("");
		return -1;
	}
	return 0;
}

int deleteFile(char* fileName)
{
	/* delete
	 * Search directory for filename, and see if it exists
	 * If it exists, delete it from the directory
	 * Else do return that the file to be deleted doesn't exist	 */

	DIR *dir;
	struct dirent *ent;

	bool found = false;
	int i = 0;

	char fullPath[100];
	
	strcpy(fullPath, "./serverContents/");

	for (i = 0; i < strlen(fileName)+1; i++)
	{
		fullPath[i+17] = fileName[i];
	}

	if((dir = opendir("./serverContents")) != NULL)
	{
		while((ent = readdir (dir)) != NULL)
		{
			if(strcmp(ent->d_name, fileName) == 0)
			{
				found = true;
				printf("The File exists, preparing to delete...\n");
				if(remove(fullPath) != 0)
				{
					perror("Error deleting the file");
					return -1;
				}
				else
				{
					printf("Successful deletion of the file\n");
					return 0;
				}

			}
		}
		if(found == false)
		{
			printf("Can't find the file: %s\n", fileName);
			return -1;
		}
		closedir(dir);
	}
	else
	{
		perror("");
		return -1;
	}
}

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


int getFile(char* fileName, int sock, struct sockaddr_in remote, struct sockaddr_in sin)
{
	/*
	 * Get
	 * - Check if the file exists on the server first
	 * - If it exists, start transfer
	 * - If it doesn't don't do anything
	 * - Get the file's size, and check how many bytes
	 * - Based on the number of bytes, we sendTo that many times
	 *
	 */

	DIR *dir;
	struct dirent *ent;
	bool exists = false;
	int nbytes; 
	char fullPath[100];
	int i;
	size_t remoteLen = sizeof(sin);
	
	strcpy(fullPath, "./serverContents/");

	for (i = 0; i < strlen(fileName)+1; i++)
	{
		fullPath[i+17] = fileName[i];
	}
	
	if((dir = opendir("./serverContents")) != NULL)
	{
		while((ent = readdir (dir)) != NULL)
		{
			if(strcmp(fileName, ent->d_name) == 0)
			{
				exists = true;
				printf("File exists, preparing for transfer...\n");
			}
		}

		if(exists == false)
		{
			printf("The file does not exist...\n");
			return -1;
		}
		closedir(dir);
	}
	else
	{
		perror("");
		return -1;
	}

	char msg[MAXBUFSIZE];
	bzero(msg, sizeof(msg));

	int fileSize = filesize(fullPath);

	//printf("Size of file: %d\n",fileSize);

	std::ostringstream sizeFile;

	sizeFile << fileSize;

	strcpy(msg, sizeFile.str().c_str());

	nbytes = sendto(sock, msg, sizeof(msg), 0,(struct sockaddr *) &remote, remoteLen);
	
	FILE *myFile = fopen(fullPath, "rb");

	long fSize2 = ftell (myFile);
	rewind(myFile);

	int transmitted = 0;

	while((transmitted) <= fileSize)
	{	
		bzero(msg, sizeof(msg));
		fread(msg, 1, sizeof(msg), myFile);		
		//memcpy(&msg, &myFile, sizeof(myFile));

		nbytes = sendto(sock, msg, 1024, 0,(struct sockaddr *) &remote, remoteLen);
		transmitted += sizeof(msg);
	}
	return 0;
}

int putFile(char* fileName, int sock, struct sockaddr_in remote, struct sockaddr_in sin)
{

		char fileBuf[MAXBUFSIZE];
		char fullPath[100];
		int i;
		
	
		strcpy(fullPath, "./serverContents/");

		for (i = 0; i < strlen(fileName)+1; i++)
		{
			fullPath[i+17] = fileName[i];
		}
	
		socklen_t len = sizeof(remote);
		int nbytes; 

		//Check if file exists
		if(std::ifstream(fileName)); //If it does exist, overwrite it
		{
			printf("File Exists\n");
		}

		struct sockaddr_in from_addr;
		int addr_length = sizeof(struct sockaddr);
		bzero(fileBuf,sizeof(fileBuf));
			
		nbytes = recvfrom(sock, fileBuf, sizeof(fileBuf), 0, (struct sockaddr *) &remote, &len); 

		int fileSize = atoi(fileBuf);

		std::ofstream myFile;
		myFile.open(fullPath);

		int received = 0;

		while(received < fileSize)
		{	
			nbytes = recvfrom(sock, fileBuf, sizeof(fileBuf), 0, (struct sockaddr *) &remote, &len); 
			myFile << fileBuf;

			//printf("FILE BUF: %s\n", fileBuf);
			received += sizeof(fileBuf);
		}
		return 0;
}

int parseCmd(char cmd[], std::string expected) //To parse get, put, and delete commands easier
{
	int i;

	char expectedC[expected.length()];
	strcpy(expectedC, expected.c_str());
	for(i = 0; i < expected.length(); i++)
	{
		if(cmd[i] != expectedC[i])
		{
			return -1;
		}
	}
	return 0;
}

void getFileName(char cmd[], char file[]) //Parses the file name from the get,put, and delete commands
{
	int i;
	if(parseCmd(cmd, "delete") == 0)
	{
		for(i = 7; i < strlen(cmd); i++)
		{
			file[i-7] = cmd[i];
		} 
	}
	else if(parseCmd(cmd, "get") == 0)
	{
		for(i = 4; i < strlen(cmd); i++)
		{
			file[i-4] = cmd[i];
		} 
	}
	else if(parseCmd(cmd, "put") == 0)
	{
		for(i = 4; i < strlen(cmd); i++)
		{
			file[i-4] = cmd[i];
		} 
	}
	
}

int main (int argc, char * argv[] )
{
	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}


	/******************
	  Once we've created a socket, we must bind that socket to the 
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

	remote_length = sizeof(remote);

	//waits for an incoming message
	for(;;)
	{
		bzero(buffer,sizeof(buffer));
		socklen_t len = sizeof(remote);
		nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, &len); 

		printf("The client says %s\n", buffer);
		
		char msg[MAXBUFSIZE];
		bzero(msg, sizeof(msg)); //To clear out random garbage in the array

		std::string commandC(buffer); //Get the command from the client

		char inputFile[100]; //Allocate space for input file

		if(strcmp(buffer, "ls") == 0)
		{
			int l = ls(msg);
			if(l == 0)
			{
				printf("%s\n",msg);
			}
			
			size_t remoteLen = sizeof(sin);
			nbytes = sendto(sock, msg, sizeof(msg), 0,(struct sockaddr *) &remote, remoteLen);
		}
		else if(strcmp(buffer, "exit") == 0)
		{
			printf("Exiting...\n");
			close(sock);
			break;
		}
		else if(parseCmd(buffer, "delete") == 0)
		{		
			getFileName(buffer, inputFile);
			int l = deleteFile(inputFile);
			if(l == 0)
				printf("Successfully deleted the file: %s\n", inputFile);
			
			size_t remoteLen = sizeof(sin);
			nbytes = sendto(sock, msg, sizeof(msg), 0,(struct sockaddr *) &remote, remoteLen);
		}
		else if(parseCmd(buffer, "get") == 0)
		{
			getFileName(buffer, inputFile);
			
			bzero(buffer, MAXBUFSIZE);
		
			int l = getFile(inputFile, sock, remote, sin);
			if(l == 0)
				printf("Successfully got and sent the file: %s\n", inputFile);
		}
		else if(parseCmd(buffer, "put") == 0)
		{
			getFileName(buffer, inputFile);
			int l = putFile(inputFile, sock, remote, sin);
			if(l == 0)
				printf("Successfully put the file: %s\n", inputFile);
		}
		/*
		else
		{
			printf("Invalid command: Nothing to do.");
		}
		*/
		
	}
}

