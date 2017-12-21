#include <iostream>
#include <pthread.h>
#include <string>

#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For Sockets (socket_addr)

#include <arpa/inet.h> //For inet_addr

#include <stdlib.h> // For atoi

#include <stdio.h>
#include <fstream>
#include <cstring>
#include <unistd.h> //For sleep
#include <sstream>

#include <pthread.h> //For multithreading

#define MAXBUFSIZE 1024
#define NUMTHREADS 20

struct threadArgs {
	char buf[MAXBUFSIZE];
	int sockN;
};

/*
 * Source: https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c
 */
std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

/*
 * Source: https://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
 */
bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

int getPort()
{
	char *portNum = new char[4];
	char fileReader[1024];
	char lineRead[1024];
	FILE *confFile = fopen("ws.conf", "r");	
	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", lineRead, portNum);
		if(strcmp(lineRead, "Listen") == 0)
			break;
	}

	return atoi(portNum);
}

/* getRootDir
 * get the root directory
 *
 */
char* getRootDir()
{
	char *rootD = new char[1024];
	char fileReader[1024];
	char lineRead[1024];
	FILE *confFile = fopen("ws.conf", "r");	
	int i;	
	int j = 0;

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", lineRead, rootD);
		if(strcmp(lineRead, "DocumentRoot") == 0)
			break;
	}

	char *unQuoteRootD = new char[1024];
	for(i = 0; i < 1024; i++)
	{
		if(rootD[i] != '\"')
		{
			unQuoteRootD[j] = rootD[i];
			j++;
		}
		else if(rootD[i] == ' ')
		{
			break;
		}
	}
	memset(&rootD, 0, sizeof(rootD));
	std::strcat(unQuoteRootD, "/");
	return unQuoteRootD;
}

char* getDirIndex()
{
	char *indexD = new char[1024];
	char fileReader[1024];
	char lineRead[1024];
	FILE *confFile = fopen("ws.conf", "r");	
	

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", lineRead, indexD);
		if(strcmp(lineRead, "DirectoryIndex") == 0)
			break;
	}

	return indexD;
}

char* checkContentType(char fileType[]) //Get file type then transfer it
{
	char fileReader[1024];
	char lineRead[1024];
	char *typeToReturn;
	char *nothing;
	bool doesExist = false;
	FILE *confFile = fopen("ws.conf", "r");

	while(fgets(fileReader, 100, confFile) != NULL)
	{
		sscanf(fileReader, "%s %s", lineRead, typeToReturn);
		if(strcmp(lineRead, fileType) == 0)
		{
			doesExist = true;
			break;
		}
	}

	if(doesExist == true)
	{
		//printf("Successfully found\n");
		return typeToReturn;
	}
	else
	{
		std::strcpy(nothing, "NOT FOUND");
		return nothing;
	}
}

bool isIndex(char pageName[])
{
	char index1[MAXBUFSIZE];
	char index2[MAXBUFSIZE];
	char index3[MAXBUFSIZE];
	char fileReader[1024];
	char lineRead[1024];
	FILE *confFile = fopen("ws.conf", "r");	
	
	if(strcmp(pageName, "/") == 0)
	{
		return true;
	}	

	char fileName[MAXBUFSIZE];
	for(int i = 1; i < strlen(pageName)+1; i++)
	{
		fileName[i-1] = pageName[i];
	}

	//printf("File Name: %s\n", fileName);
	while(fgets(fileReader, 100, confFile) != NULL)
	{
		char quotes;
		sscanf(fileReader, "%s %s %s %s", lineRead, index1, index2, index3);
		if(strcmp(lineRead, "DirectoryIndex") == 0)
		{
			//printf("PAGENAME: %s\nINDEX1: %s\nINDEX2: %s\nINDEX3: %s\n",fileName,index1,index2,index3);
			if(strcmp(index1, fileName) == 0 || strcmp(index2, fileName) == 0 || strcmp(index3, fileName) == 0)
			{	
				fclose(confFile);
				return true;
			}
			break;
		}
	}
	fclose(confFile);

	return false;
	
}

char* getFileExtension(char* fileT)
{
	char* extension;
	extension = strrchr(fileT, '.');

	return extension;
}


/* handleRequest
 * Takes the user sent buffer and generates a HTTP Response
 * Will throw 400, 404, 501 if there is an error with the request
 * Sends the results from server back to the client
 * Each generated thread will use this function
 */
void* handleRequest(void *args) 
{
	struct threadArgs *thisArgs = (struct threadArgs*) args;
	char req[10];
	char dir[MAXBUFSIZE];
	char bufs[MAXBUFSIZE];
	char version[MAXBUFSIZE];
	char* verNum;
	int nbytes;
	int totalBytes = 0;

	char htmlres[MAXBUFSIZE];

	sscanf(thisArgs->buf, "%s %s %s", req, dir, version);
	verNum = strrchr(version, '/');

	memset(&bufs, 0, sizeof(bufs));

	if(strcmp(verNum, "/1.1") == 0 || strcmp(verNum, "/1.0") == 0)
	{
		if(strcmp(req, "GET") == 0)
		{
			//printf("ISINDEX: %i\n", isIndex(dir));
			//if(strcmp(dir, "/") == 0) //Just get index.html, the request is only for root
			if(isIndex(dir))
			{
				char* indexH = std::strcat(getRootDir(), getDirIndex());	
				
				printf("indexH: %s\n",indexH); 

				FILE *rootDir = fopen(indexH, "rb");

				int sizeFile = filesize(indexH);

				std::ostringstream sFile;

				sFile << sizeFile;
				
				printf("\n");
				char respGood[] = "HTTP";
				std::strcat(respGood, verNum);
				std::strcat(respGood, " 200 OK\nContent-Type: text/html\nContent-Length: ");
				std::strcat(respGood, sFile.str().c_str());
				std::strcat(respGood, "\n\n");
				
				printf("%s", respGood);
			
				nbytes = send(thisArgs->sockN, respGood, strlen(respGood), 0);
				memset(&respGood, 0, sizeof(respGood));	
				while(totalBytes < sizeFile)
				{
					memset(&bufs, 0, sizeof(bufs));
					int read = fread(bufs, 1, sizeof(bufs), rootDir);
					nbytes = send(thisArgs->sockN, bufs, 1024, 0);
					totalBytes += nbytes;
					// printf("%s",bufs);
					
				}
				
				fclose(rootDir);
			}	
			else //Get everything else TODO: double check file extension to see if it's valid (.conf file and getFileExtension and checkContentType)
			{
				char* fileH = std::strcat(getRootDir(), dir);
				char* ext = getFileExtension(fileH);
				if(is_file_exist(fileH))
				{
					char* result = checkContentType(getFileExtension(fileH));
					if(strcmp(result,"NOT FOUND") != 0)
					{
						FILE *fileDir = fopen(fileH, "rb");
				
						int sizeFile = filesize(fileH);
						std::ostringstream sFile;

						sFile << sizeFile;
						
						printf("\n");
						char respGood[] = "HTTP";
						std::strcat(respGood, verNum);
						std::strcat(respGood, " 200 OK\nContent-Type: ");
						std::strcat(respGood, checkContentType(getFileExtension(fileH)));
						std::strcat(respGood,"\n");
						std::strcat(respGood, "Content-Length: ");
						std::strcat(respGood, sFile.str().c_str());
						std::strcat(respGood, "\n\n");

						printf("%s\n",respGood);
						nbytes = send(thisArgs->sockN, respGood, strlen(respGood), 0);
						memset(&respGood, 0, sizeof(respGood));			
						
						int remaining = 0;
						
						while(totalBytes < sizeFile)
						{ 
							memset(&bufs, 0, sizeof(bufs));
						 	int read = fread(bufs, 1, sizeof(bufs), fileDir);
						 	// printf("BUFFER SENT: %s\n", bufs);
						 	nbytes = send(thisArgs->sockN, bufs, read, 0);

						 	totalBytes += nbytes;
						 }
						
						fclose(fileDir);
					}
					else //If the requested file format is not supported - throw 501 File type error
					{
						std::strcat(htmlres, "<html><body>501 Not Implemented - File Type Error: ");
						std::strcat(htmlres, ext);
						std::strcat(htmlres, "</body></html>");

						char response[] = "HTTP/1.1 501 Not Implemented\n";
						int sizeRes = strlen(htmlres);

						std::ostringstream sFile;

						sFile << sizeRes;	

						std::strcat(response, "Content-Type: text/html\nContent-length: ");
						std::strcat(response, sFile.str().c_str());
						std::strcat(response, "\n\n");
						std::strcat(response, htmlres);
						nbytes = send(thisArgs->sockN, response, strlen(response), 0);
					}
				}
				else //If can't find the file on the server, throw 404 Not Found
				{
					std::strcat(htmlres,"<html><body>404 Not Found Reason - URL does not exist : ");
					std::strcat(htmlres, dir);
					std::strcat(htmlres, "</body></html>");

					int sizeRes = strlen(htmlres);
					std::ostringstream sFile;

					sFile << sizeRes;	

					char response[] = "HTTP/1.1 404 Not Found\n";

					std::strcat(response, "Content-Type: text/html\nContent-length: ");
					std::strcat(response, sFile.str().c_str());
					std::strcat(response, "\n\n");
					std::strcat(response, htmlres);

					nbytes = send(thisArgs->sockN, response, strlen(response), 0);
					memset(&htmlres, 0, sizeof(htmlres));
					memset(&response, 0, sizeof(response));
				}
			}
		}
		else if(strcmp(req, "POST") == 0)
		{

			int fileSize = 0;
			if (strcmp(getFileExtension(dir), ".html") == 0)
			{
				recv(thisArgs->sockN, bufs, sizeof(bufs), 0);

				std::strcat(htmlres,"<html><body><h1> ");
				std::strcat(htmlres, bufs);
				std::strcat(htmlres, "</h1></body></html>");

				int sizeRes = strlen(htmlres);
				std::ostringstream sFile;

				sFile << sizeRes;	
				char response[] = "HTTP/1.1 200 OK\n";

				std::strcat(response, "Content-Type: text/html\nContent-length: ");
				std::strcat(response, sFile.str().c_str());
				std::strcat(response, "\n\n");
				std::strcat(response, htmlres);

				nbytes = send(thisArgs->sockN, response, strlen(response), 0);
				
			}
			else
			{
				std::strcat(htmlres, "<html><body>501 Not Implemented - File Type Error: ");
				std::strcat(htmlres, dir);
				std::strcat(htmlres, "</body></html>");

				char response[] = "HTTP/1.1 501 Not Implemented\n";
				int sizeRes = strlen(htmlres);

				std::ostringstream sFile;
				sFile << sizeRes;	

				std::strcat(response, "Content-Type: text/html\nContent-length: ");
				std::strcat(response, sFile.str().c_str());
				std::strcat(response, "\n\n");
				std::strcat(response, htmlres);
				nbytes = send(thisArgs->sockN, response, strlen(response), 0);
			}

		}
		else //Not a GET/HEAD/POST, throw 400 Bad Request on Method
		{
			std::strcat(htmlres,"<html><body>400 Bad Request Reason: Invalid Method :");
			std::strcat(htmlres, req);
			std::strcat(htmlres, "</body></html>");

			char response[] = "HTTP/1.1 400 Bad Request\n";
			int sizeRes = strlen(htmlres);

			std::ostringstream sFile;

			sFile << sizeRes;	

			std::strcat(response, "Content-Type: text/html\nContent-length: ");
			std::strcat(response, sFile.str().c_str());
			std::strcat(response, "\n\n");
			std::strcat(response, htmlres);

			printf("%s",response);
			nbytes = send(thisArgs->sockN, response, strlen(response), 0);
		}
	}
	else //Not HTTP/1.1 or HTTP/1.0, throw a Bad Request Invalid HTTP Version
	{
		std::strcat(htmlres, "<html><body>400 Bad Request Reason: Invalid HTTP-Version: ");
		std::strcat(htmlres, verNum);
		std::strcat(htmlres, "</body></html>");

		char response[] = "HTTP/1.1 400 Bad Request\n";
		int sizeRes = strlen(htmlres);

		std::ostringstream sFile;

		sFile << sizeRes;	

		std::strcat(response, "Content-Type: text/html\nContent-length: ");
		std::strcat(response, sFile.str().c_str());
		std::strcat(response, "\n\n");
		std::strcat(response, htmlres);
		nbytes = send(thisArgs->sockN, response, strlen(response), 0);
	}
}

int main(void)
{
	/*
	 * TODO: 
	 * (check) Create socket successfully from conf file
	 * (check) Pthread creation - one for every user/socket
	 * (check) Infinite Loop to do servering
	 * (check) accept() and listen() in infinite loop
	 * (check) Send results from server back to client
	 * Error Handling - 400, 404, 501, 500 -> 404, 501, and 400 Invalid Method
     * Time out after 10 seconds - disconnect the client 
	 * If user enters the request index.htm, index.ws, or index.html, it gets the index  
	 */

	struct sockaddr_in cliaddr, servaddr;          

	int fileSize = filesize("ws.conf");
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;                 //address family
	servaddr.sin_port = htons(getPort());	
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //sets local ip

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		 perror("Failed to bind to port");
	}

	char buffer[MAXBUFSIZE];
	pthread_t threads[150];
	int threadCount = 0;
	int rc;
	struct threadArgs newArgs;
	int nbytes = 0;

	if(listen(sockfd, 10) < 0)
	{
			printf("Failed to Listen\n");
	}

	for(;;)
	{	
		socklen_t clilen = sizeof(cliaddr);

		int connfd = accept(sockfd, (struct sockaddr*) &cliaddr, &clilen);

		recv(connfd, buffer, sizeof(buffer), 0);

		std::strcpy(newArgs.buf, buffer);
		newArgs.sockN = connfd;
		printf("\n%s\n", buffer);

		rc = pthread_create(&threads[threadCount], NULL, handleRequest, &newArgs);	

		if(rc < 0)
		{
			char htmlres[MAXBUFSIZE];
			std::strcat(htmlres, "<html><body>500 Internal Server Error: Cannot Allocate Memory ");
			std::strcat(htmlres, "</body></html>");

			char response[] = "HTTP/1.1 500 Internal Server Error\n";
			int sizeRes = strlen(htmlres);

			std::ostringstream sFile;

			sFile << sizeRes;	

			std::strcat(response, "Content-Type: text/html\nContent-length: ");
			std::strcat(response, sFile.str().c_str());
			std::strcat(response, "\n\n");
			std::strcat(response, htmlres);
			nbytes = send(sockfd, response, strlen(response), 0);

		}

		pthread_join(threads[threadCount], NULL);

		threadCount += 1;
		memset(&buffer, 0 , sizeof(buffer));
		close(connfd);
	}
	printf("Closing Socket");
	close(sockfd);

	return 0;

}