/*
** client.c -- A client for a http server supporting GET and PUT
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <cstring>
#include <netdb.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>

using namespace std;
#define MAXDATASIZE 128 // max number of bytes we can get at once


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool is_file_binary(char * filepath)
{
    string file = filepath;
    string extension = file.substr(file.find_last_of(".")+1);
    if(	extension=="png" || extension=="gif" || extension=="jpg"|| extension=="pdf"|| extension=="jpeg")
		{return true;}
	else
		{return false;}
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char* buf = (char*) calloc(MAXDATASIZE, sizeof(char));
	struct addrinfo hints, *servinfo, *p;
	int rv,n;
	char s[INET6_ADDRSTRLEN];

	if (argc != 5) {
	    fprintf(stderr,"Usage: ./client host port request filepath\n");
	    exit(1);
	}

	char *port = argv[2];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	char* hostptr;
	char* hostname=argv[1];
	char* filename=argv[4];


	if ( (hostptr = strstr(hostname, "http://www.")) != NULL || (hostptr = strstr(hostname, "https://www.")) != NULL ) 
	{
		hostname = hostname + 11; //To ignore 'http://'
	}
	if ( (hostptr = strstr(hostname, "www.")) != NULL) 
	{
		hostname = hostname + 4; //To ignore 'www://'
	}

	
	if ((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}


	// Creating Connection--------------------------------------------------
		
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		int bindfd = bind(sockfd, p->ai_addr, p->ai_addrlen);
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	printf("client: connecting to %s\n", s);

	int optval = 1; 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    freeaddrinfo(servinfo); // all done with this structure

	// Formatting Request String-----------------------------------------------------------
    char getRequest[1024];
    if(strcmp(argv[3],"GET")==0 || strcmp(argv[3],"get")==0)
    {
		// Sending the Request------------
    	sprintf(getRequest, "GET /%s HTTP/1.0\r\nhost: %s\r\n\r\n",argv[4],argv[1]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP
    	
    	int sent=send(sockfd,getRequest, strlen(getRequest),0);
    	char recvpkt[MAXDATASIZE];
				
			// Receiving Header-------------------
			 n = read(sockfd,recvpkt, 15);
			if (n < 0)
			printf("ERROR reading from socket");

			if(!strcmp(recvpkt, "HTTP/1.1 404 NO")|| !strcmp(recvpkt, "HTTP/1.1 404 No"))				//Error message displayed if the file is not found
			{
				printf("\n404 NOT FOUND\n");
			}
			else
				printf("\n%s",recvpkt);


			//  If file found then receive it char by char
			if(strcmp(recvpkt, "HTTP/1.1 200 OK")==0)	
			{
					
					if(strstr(filename,"/")!=NULL) 
						{ filename = strrchr(filename, '/'); filename++; }

					ofstream myfile(filename,ios::out | ios::binary);
					bzero(recvpkt,MAXDATASIZE);

					while( (numbytes = read(sockfd, recvpkt, 1))>0)
					{
						//cout<<"getting";//<<recvpkt;
						if(recvpkt[0]!=EOF)
						cout<<recvpkt[0];
						myfile.write(recvpkt,1);

					}

					myfile.close();
			}

    }

	else
	{
		if(strcmp(argv[3],"PUT")== 0 || strcmp(argv[3],"put")==0)
			sprintf(getRequest, "PUT /%s HTTP/1.0\r\nhost: %s\r\n\r\n",argv[4],argv[1]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP
		// Sending the Request------------
		else
			{
				cout<<"Invalid request";
			exit(0);
		}
		int sent=send(sockfd,getRequest, strlen(getRequest),0);

		// if the file is a binary formatted
		if(is_file_binary(filename))
		{
			char tosend[MAXDATASIZE];
			if(strstr(filename,"/")!=NULL) 
						{ filename = strrchr(filename, '/'); filename++; }

			ifstream myfile;
			myfile.open(filename,ios::in | ios::binary);
			while( numbytes = myfile.read(tosend,1) > 0)
			{
				send(sockfd, tosend, 1, 0);
			}
			myfile.close();
		}

		// if it is a normal file
		else
		{
			FILE *fp;
			char ch;
			int n;
			fp = fopen(filename,"r");
			if(fp)
			{
				//Send char by char
				do{
					ch = fgetc(fp);
					if(ch!=EOF)
					n = write(sockfd, &ch, 1);
					if (n < 0) 
						printf("ERROR writing to socket");
								
				}while( ch != EOF );

				fclose(fp);
				cout<<"200 OK FILE CREATED";
			}
			else
			{
				printf("File not found\n");
			}
		}
	}			
	close(sockfd);

	return 0;
}
