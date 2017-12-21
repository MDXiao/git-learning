#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <string>

#include <sys/socket.h> //For Sockets
#include <netinet/in.h> //For Sockets (socket_addr)
#include <netdb.h> //For getaddrinfo

#include <arpa/inet.h> //For inet_addr

#include <stdlib.h> // For atoi

#include <fstream>
#include <cstring>
#include <sstream>
#include <ctype.h>

#include <sys/time.h> // For timeout 
#include <sys/types.h>

#include <algorithm>
#include <time.h>


#define MAXBUFSIZE 1024
#define NUMTHREADS 20


// Helper function for textual date and time.
// DTTMSZ must allow extra character for the null terminator.

#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
static char *getDtTm (char *buff) {
    time_t t = time (0);
   	 	strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
    	return buff;
}


struct threadArgs {
	int sockN;
	int cacheTime;
	int numCached;
	// std::unordered_map<char*, char*> cacheMap;
};


std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

void getHost(char buf[],char toWrite[])
{

	char *doesExist;

	char toCom[] = "Host: ";
	// memset(&toWrite, 0, sizeof(toWrite));
	
	int i, j;
	int counter = 0;

	doesExist = strstr(buf, toCom);
	if(doesExist != NULL)
	{
		int indexer = 0;
		bool found = false;

		for(i = 0; i < 1024; i++)
		{
			if(buf[i] == toCom[0])
			{
				for(j = i; j < strlen(toCom)+i; j++)
				{
					if(buf[j] == toCom[indexer])
					{
						found = true;
						indexer++;
					}
					else
					{
						found = false;
						break;
					}
				}
				if(found == true)
				{
					indexer = i;
					break;
				}
				else
				{
					indexer = 0;
				}
			}
		}

		for(i = indexer + 6; i < 1024; i++)
		{
			if(isspace(buf[i]))
			{	
				break;
			}
			else
			{
				toWrite[counter] = buf[i];
				counter++;
			}
			
		}
	}
	else
	{
		char req[10];
		char hostN[500];
		char httV[50];
		sscanf(buf, "%s %s %s", req, hostN, httV);
		
		char httP[] = "http://";
		char www[] = "www.";
		int length;

		doesExist = strstr(hostN, www);

		if(doesExist != NULL)
		{
			length = strlen(httP) + strlen(www);
		}
		else
		{
			length = strlen(httP);
		}

		int counter = 0;
		for(i = length; i < strlen(hostN); i++)
		{
			// printf("Hostname: %c\n", hostN[i]);
			toWrite[counter] = hostN[i];
			counter++;
			
		}


	}
	
	// for(i = 0; i < strlen(toWrite); i++)
	// {
	// 	printf("HOST: %c\n", toWrite[i]);
	// }

}

int checkBlockList(char hostName[])
{
	FILE *blockList = fopen("blockList.txt", "r");	
	char fileReader[1024];
	char lineRead[1024];

	while(fgets(fileReader, 100, blockList) != NULL)
	{
		sscanf(fileReader, "%s", lineRead);
		if(strcmp(lineRead, hostName) == 0)
		{
			return -1;
		}
	}

	fclose(blockList);
	return 0;
}

bool checkIfCached(char key[])
{
	std::string fileN(key);

	std::replace(fileN.begin(), fileN.end(), '/', '.');
	fileN.erase(0,7);
	fileN.insert(0, "cachedResources/");
	fileN.insert(0,"/");
	fileN.insert(0,".");


	return is_file_exist(fileN.c_str());
}

void fetchFromCache(char key[], int sock)
{
	std::string fileN(key);

	std::replace(fileN.begin(), fileN.end(), '/', '.');
	fileN.erase(0,7);
	std::string fileHeader = fileN;


	fileN.insert(0, "cachedResources/");
	fileN.insert(0,"/");
	fileN.insert(0,".");


	// printf("fileN: %s\n", fileN.c_str());
	
	std::ifstream myFile(fileN.c_str(), std::ios::binary);
	

	myFile.seekg (0, myFile.end);
    int length = myFile.tellg();
    myFile.seekg (0, myFile.beg);

    char * myBuf2 = new char [length];

	myFile.read(myBuf2, length);
	write(sock, myBuf2, length);
	bzero(&myBuf2, sizeof(myBuf2));

	myFile.close();
}

void createCache(char key[], char buf[], int sizeBuf)
{
	std::string fileN(key);

	std::replace(fileN.begin(), fileN.end(), '/', '.');
	fileN.erase(0,7);
	fileN.insert(0, "cachedResources/");

	char header[5000];
	char body[20240];
	// parseHeader(buf,header,body);
	// std::ofstream myFile (fileN.c_str(), std::ios::binary|std::ios::app);
	
	fileN.insert(0,"/");
	fileN.insert(0,".");
	std::ofstream myFile (fileN.c_str(), std::ios::binary|std::ios::app);
	myFile.write(buf, sizeBuf);
	

	// printf("WRITING  BUF: %s\n", buf);
	
	myFile.close();

}


bool checkHostName(char hostName[])
{
	std::ifstream cacheHost("hostCache.txt");
	std::string line;

	char hostN[100];
	char ipAddr[100];
	char timeOut[1000];

	if (cacheHost.is_open())
  	{
    	while ( getline (cacheHost,line) )
    	{
      		sscanf(line.c_str(), "%s %s %s", hostN, ipAddr, timeOut);
      		if(strcmp(hostN, hostName) == 0)
      		{
      			printf("found hostname in cache!\n");
      			return true;
      		}
    	}
   		cacheHost.close();
  		return false;
  	}

}

void inputHost(char hostName[], char ipAddr[])
{
	char buff[DTTMSZ];
    std::fstream filestr;
    std::fstream filestr2;
    filestr.open ("hostCache.txt", std::fstream::out|std::fstream::app);

    int offset; 
    std::string line;
    std::ifstream Myfile;
    Myfile.open ("hostCache.txt");

    if(Myfile.is_open())
    {
        while(!Myfile.eof())
        {
            getline(Myfile,line);
            if ((offset = line.find(hostName, 0)) != std::string::npos) 
            {
	             printf("Hostname exists in file!\n");
	             filestr<< getDtTm (buff) << hostName << " "<< ipAddr <<std::endl;
	             break;
            }
            else
            {
            	// printf("getDTM: %s\ngetDTM2: %s\n", getDtTm(buff, 0), getDtTm(buff,60));
               filestr<< getDtTm (buff) << hostName << " "<< ipAddr <<std::endl;
    			break;


            }
        }
        Myfile.close();
    }
 
    filestr.close();

    

}

void *handleRequest(void *args)
{
	struct threadArgs *thisArgs = (struct threadArgs*) args;


	char recvBuf[10240];

	memset(&recvBuf, 0, sizeof(recvBuf));
	recv(thisArgs->sockN, recvBuf, sizeof(recvBuf),0);

	printf("\n%s\n",recvBuf);

	char cmd[10];
	char hostName[100];
	char httpVer[100];
	char* verNum;
	char host[1024];

	char htmlres[MAXBUFSIZE];
	memset(&htmlres, 0, sizeof(htmlres));
	int nbytes;

	memset(&host, 0, sizeof(host));
	

	sscanf(recvBuf, "%s %s %s",cmd,hostName,httpVer);

	// printf("cKey: %s\n", cKey);
	verNum = strrchr(httpVer, '/');

	getHost(recvBuf, host);



	// printf("Host received: %s\n", host);


	if(checkBlockList(host) == 0)
	{
		if(strcmp(verNum, "/1.1") == 0 || strcmp(verNum, "/1.0") == 0)
		{
			if(strcmp(cmd, "GET") == 0)
			{

				if(checkIfCached(hostName)) //If element is in cache
				{
					printf("Fetching from Cache!\n");
					fetchFromCache(hostName, thisArgs->sockN);
				}
				else
				{
					if(checkHostName(host))
					{
						//No DNS
					}
					else
					{
						struct addrinfo hints, *servinfo, *p;
						int hostRes;

						memset(&hints, 0, sizeof hints);
						hints.ai_family = AF_UNSPEC; 
						hints.ai_socktype = SOCK_STREAM;

						hostRes = getaddrinfo(host, "80", &hints, &servinfo); 
						// printf("hostRes: %i\n", hostRes);
						
						if(hostRes != 0)
						{
							std::strcpy(htmlres, "<html><body>400 Bad Request Reason: Invalid Host: ");
							std::strcat(htmlres, host);
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
							memset(&response, 0, sizeof(response));
						}
						else
						{
							int sockfd2;
							for(p = servinfo; p != NULL; p = p->ai_next) 
							{
			    				if ((sockfd2 = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
			    				{
			        				perror("Socket Proxy Creation Failed!");
			        				continue;
			    				}


			   				    if (connect(sockfd2, p->ai_addr, p->ai_addrlen) == -1) 
			   				    {
			        				perror("Socket Proxy Connection Failed!");
			        				close(sockfd2);
			        				continue;
							    }


			    				break; 
							}

							struct addrinfo *p;
							p = servinfo;
	  						char hostID[256];

	  						getnameinfo(p->ai_addr, p->ai_addrlen, hostID, sizeof(hostID), NULL, 0, NI_NUMERICHOST);
							
							inputHost(host, hostID);

							send(sockfd2, recvBuf, strlen(recvBuf), 0);


							// printf("SENDING TO SERVER REMOTE: \n%s\n", recvBuf);

							int opt = 0;
							while(1)
							{
								nbytes = read(sockfd2, recvBuf, sizeof(recvBuf));

								if(nbytes <= 0)
								{
									break;
								}
								else
								{
									// printf("ADDING: %s\nBODY\n%s\n", cKey, recvBuf);
									createCache(hostName, recvBuf, nbytes);
									write(thisArgs->sockN, recvBuf, nbytes);

									// printf("RECEIVED: %s\n", recvBuf);
									// write(thisArgs->sockN, recvBuf, nbytes);
				
									memset(&recvBuf, 0, sizeof(recvBuf));
								}
							}


							printf("Finished sending to local\n");
							close(sockfd2);	
						}						
					}
				}
			}
			else
			{
				std::strcat(htmlres,"<html><body>400 Bad Request Reason: Invalid Method: ");
				std::strcat(htmlres, cmd);
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
				memset(&response, 0, sizeof(response));
			} 
		}
	}
	else
	{
		std::strcat(htmlres,"<html><body>403 Forbidden - Blocked Host: ");
		std::strcat(htmlres, host);
		std::strcat(htmlres, "</body></html>");

		char response[] = "HTTP/1.1 403 Forbidden\n";
		int sizeRes = strlen(htmlres);

		std::ostringstream sFile;

		sFile << sizeRes;	

		std::strcat(response, "Content-Type: text/html\nContent-length: ");
		std::strcat(response, sFile.str().c_str());
		std::strcat(response, "\n\n");
		std::strcat(response, htmlres);
		printf("%s",response);
		nbytes = send(thisArgs->sockN, response, strlen(response), 0);
		memset(&response, 0, sizeof(response));
	}

	memset(&htmlres, 0, sizeof(htmlres));
	memset(&host, 0, sizeof(host));
	close(thisArgs->sockN);
}



int main(int argc, char* argv[])
{

	/* TODO
	 * Fork each request (DONE)
	 * Parse information from client (DONE)
	 * Check if valid HTTP request (DONE)
	 * handlerequest (IN PROGRESS)
	 * Hash mapping to cache proxy
	 *   E.x. check if GET request when hashed exists in the hash table
	 *.  If the hashed request is in the map, get the values
	 *	 Else, just do the send and recv already implemented in handlerequest
	 * Implement timeout to objects within the hash function - if exceeds time the user set, delete the link
	 */

	if(argc < 3)
	{
		printf("USAGE: ./webproxy <PORTNUMBER> <CACHETIMEOUT>\n");
		exit(0);
	}

	fd_set rfds;

	int portNum = atoi(argv[1]);
	int timeOpt = atoi(argv[2]);
	


	struct sockaddr_in cliaddr, servaddr;          
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;                 //address family
	servaddr.sin_port = htons(portNum);	
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //sets local ip

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed");

	if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		 perror("Failed to bind to port");
	}

	// char buffer[MAXBUFSIZE];
	struct threadArgs newArgs;
	int nbytes = 0;

	if(listen(sockfd, 50) < 0)
	{
			printf("Failed to Listen\n");
	}

	FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

	
	socklen_t clilen = sizeof(cliaddr);

	newArgs.cacheTime = timeOpt;
	for(;;)
	{	
		int connfd = accept(sockfd, (struct sockaddr*) &cliaddr, &clilen);

		// printf("NEW CONNECTION: %i\n", connfd);
		newArgs.sockN = connfd;

		int pid = fork();

		if(pid == -1)
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
			exit(1);
		}
		else if(pid == 0)
		{
			handleRequest(&newArgs);
			exit(0);
		}

		close(connfd);

	}
	printf("Closing Socket");
	close(sockfd);

	return 0;
}