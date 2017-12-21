/*
 * Michael Xiao
 * PA1 UDP Client Server
 */

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

#define MAXBUFSIZE 1024

/*
 * parseCmd
 * Exactly like in server
 * Parse the command to check if the command is equal to expected
 * For checking purposes only
 */
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

/*
 * getFilename
 * Exactly like in server
 * Get the filename forom the command and store it into another char array, which is fileN[]
 * Only used to get the file name
 */
void getFileName(char cmd[], char fileN[]) //Parses the file name from the get,put, and delete commands
{
	int i;
	if(parseCmd(cmd, "delete") == 0)
	{
		for(i = 7; i < strlen(cmd); i++)
		{
			fileN[i-7] = cmd[i];
		} 
	}
	else if(parseCmd(cmd, "get") == 0)
	{
		for(i = 4; i < strlen(cmd); i++)
		{
			fileN[i-4] = cmd[i];
		} 
	}
	else if(parseCmd(cmd, "put") == 0)
	{
		for(i = 4; i < strlen(cmd); i++)
		{
			fileN[i-4] = cmd[i];
		} 
	}
	
}

/*
 * fileSize function
 * Source: https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

int main (int argc, char * argv[])
{

	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];

	struct sockaddr_in remote;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet 
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address
	

	//Causes the system to create a generic socket of type UDP (datagram)
	
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
		return -1;
	}

	/******************
	  sendto() sends immediately.  
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
	for(;;)
	{
		/*
		 * Like in server, infinite loop to continously send data to the server from client
		 * Pop a menu up to enter, either get, put, ls, delete, and exit
		 * Based on user input, do one of the commands
		 * 
		 */

		char command[MAXBUFSIZE];	
		bzero(command, sizeof(command));
		bzero(buffer, sizeof(buffer));
		printf("Please enter one of the following commands:\n1) get [file_name]\n2) put [file_name]\n3) delete [file_name]\n4) ls\n5) exit\n");
		
		std::cin.getline(command, 100);
		
		std::string commandC(command);
		socklen_t len = sizeof(remote);

		//nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));
	
		if(commandC.substr (0,3).compare("get") == 0)
		{
			nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));
			
			char fileN[100];
			getFileName(command, fileN);
			bzero(command, sizeof(command));

			//Check if file exists
			if(std::ifstream(fileN)) //If it does exist, overwrite it
			{
				printf("File exists, currently overwriting existing file...\n\n");
			}

			struct sockaddr_in from_addr;
			int addr_length = sizeof(struct sockaddr);
			bzero(buffer,sizeof(buffer));

			nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, &len); 

			int fileSize = atoi(buffer);

			//printf("File Size: %d\n", fileSize);

			FILE * myFile = fopen(fileN, "wb");

			int received = 0;

			if(fileSize > 1024) //If the size of the file is greater than 1024 bytes
			{	
				/*
				 * Loop through till the entire file is read
				 * If the remaining bytes is less than 1024, just transmit the remaining bytes
				 * in a fixed buffer with a length of the remaining bytes
				 */
				while((received+1024) <= fileSize)
				{	
					bzero(buffer, sizeof(buffer));
					nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, &len); 
					int bytesWritten = fwrite(buffer, sizeof(buffer[0]), sizeof(buffer), myFile);

					received += bytesWritten;
				}
				int remaining = fileSize - received;
				if(received != 0) //Fix padding so it only reads the file length and no more
				{
					char newMsg[remaining];
					nbytes = recvfrom(sock, newMsg, sizeof(newMsg), 0,(struct sockaddr *) &remote, &len);
			
					int bytesWritten = fwrite(newMsg, sizeof(buffer[0]), sizeof(newMsg), myFile);

					received += bytesWritten;
				}
			}
			else if(fileSize <= 1024 && fileSize > 0)
			{
				char newMsg[fileSize];
				nbytes = recvfrom(sock, newMsg, sizeof(newMsg), 0,(struct sockaddr *) &remote, &len);
			
				int bytesWritten = fwrite(newMsg, sizeof(buffer[0]), sizeof(newMsg), myFile);

				received += bytesWritten;
			}
			else
			{ /*
				if (fileSize == 0)
					printf("Notice: The file you are trying to get is size 0.\n\n");
				else 
					printf("Notice: The file youare trying to get does not exist.\n\n");
				*/
			}
			fclose(myFile);
			printf("File get() complete.\n");
		}
		else if(commandC.substr (0,3).compare("put") == 0)
		{

			/*
			 * put() statment
			 * send command to server first
			 * Check if the file exists on the client end first
			 * If it does exist, continously sendTo 1024 file bytes
			 * Then create a seperate char array for the remaining bytes if they are less than
			 * 1024, and statements to handle file sizes less than 1024 as well
			 */

			nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));
		
			DIR *dir;
			struct dirent *ent;
			bool exists = false;
			char fileN[100];
			getFileName(command, fileN);

			std::string fileStr (fileN);
		
			if((dir = opendir(".")) != NULL)
			{
				while((ent = readdir (dir)) != NULL)
				{			
					//if(strcmp(fileN, ent->d_name) == 0)
					if(fileStr.compare(ent->d_name) == 0)
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

			if(exists != false)
			{
				bzero(command, sizeof(command));

				int fileSize = filesize(fileN);

				std::ostringstream sizeFile;

				sizeFile << fileSize; //Get file size for loop and debugging purposes

				strcpy(buffer, sizeFile.str().c_str());

				//Send the file size to the server, so the server can keep track 	
				nbytes = sendto(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, sizeof(remote));

				FILE *myFile = fopen(fileN, "rb");

				int transmitted = 0;

				if(fileSize > 1024)
				{
					while((transmitted + 1024) <= fileSize)
					{	
						bzero(buffer, sizeof(buffer)); //Clear Buffer
						fread(buffer, 1, sizeof(buffer), myFile); //Read file contents		

						nbytes = sendto(sock, buffer, 1024, 0,(struct sockaddr *) &remote, sizeof(remote));
						transmitted += sizeof(buffer);
					}
					int remaining;
					remaining = fileSize - transmitted;
					if(remaining != 0) //If all the bytes haven't been transmitted
					{
						//Create new buffer array, and fread remaining bytes from file
						char newMsg[remaining];

						bzero(newMsg, sizeof(newMsg));

						int bytesRead = fread(newMsg, 1, sizeof(newMsg), myFile);
						nbytes = sendto(sock, newMsg, sizeof(newMsg), 0,(struct sockaddr *) &remote, sizeof(remote));
						transmitted += sizeof(newMsg); //Keep track of transmitted data in bytes
					}
				}
				else //If the file size is smaller than 1024 bytes
				{
					char newMsg[fileSize]; //Just create array based on the size of the file 
					//So no padding is left on the file/buffer
					bzero(newMsg, sizeof(newMsg));

					int bytesRead = fread(newMsg, 1, sizeof(newMsg), myFile);
					nbytes = sendto(sock, newMsg, sizeof(newMsg), 0,(struct sockaddr *) &remote, sizeof(remote));
					transmitted += sizeof(newMsg);
				}
			}
			else
			{
				printf("The following file does not exist, please enter another file...\n\n");
			}
			printf("File Put() complete.\n");
		}
		else if(commandC.substr (0,6).compare("delete") == 0)
		{
			/* Delete() statement
			 * Like the other if statements, first send command to server for parsing 
			 * Receive response from server, and print the results 
			 *
			 */

			nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));
			printf("Deleting...\n");
				struct sockaddr_in from_addr;
			int addr_length = sizeof(struct sockaddr);
			bzero(buffer,sizeof(buffer));

			socklen_t len = sizeof(remote);

			nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, &len); 
		
			printf("Successful deletion of the file from the server.\n");
		}
		else if(commandC.substr (0,2).compare("ls") == 0)
		{
			/*  ls() statement
			 *	Send the command to the server first
			 *  Then print the received buffer from the server
			 *  The client and see the listed elements in the directory via printf
			 */ 

			nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));
			printf("Listing...\n");
			struct sockaddr_in from_addr;
			int addr_length = sizeof(struct sockaddr);
			bzero(buffer,sizeof(buffer));

			socklen_t len = sizeof(remote);

			nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &remote, &len); 
			
			printf("\n%s\n\n",buffer);
		}
		
		else if(commandC.substr (0,4).compare("exit") == 0)
		{
			/* Exit() statement
			 * Send the command to the server so the server also knowns to close the socket
			 * Close the socket here
			 * Break loop
			 */

			printf("Exiting...\n");
			nbytes = sendto(sock, command, sizeof(command), 0, (struct sockaddr *) &remote, sizeof(remote));

			close(sock);
			break;
		}
		else
		{
			printf("ERROR: Unrecongized Command, please try again.\n");
		}
		

	}

}

