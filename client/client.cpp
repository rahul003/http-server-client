/*
** client.c -- a client to connect to http servers
Supports GET/PUT requests.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <fstream>

#define MAXDATASIZE 100 // max number of bytes we can get at once 
using namespace std;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char* buf = (char*) calloc(MAXDATASIZE, sizeof(char));
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
 	int n;

	if (argc != 5) {
	    fprintf(stderr,"usage: client hostname port requesttype filename\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
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

	freeaddrinfo(servinfo); // all done with this structure

	int optval = 1; //is 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    bzero(buf,MAXDATASIZE);
    char * filename = argv[4];
   

    if(!strcmp(argv[3],"GET") || !strcmp(argv[3],"get") )
    {	char * request;
    	sprintf(buf, "GET /%s HTTP/1.1\r\nHost: %s\r\nAccept-Encoding:gzip,deflate,sdch\r\n\r\n",argv[4],argv[2]);
	    n = write(sockfd,buf,MAXDATASIZE);
		if (n < 0) 
			fprintf(stderr, "ERROR writing to socket\n");
   	 	
		ofstream myfile (filename, ios::out | ios::binary);
   	 	int len;
		while( (len = read(sockfd,buf,MAXDATASIZE) )>0)
		{	
   	 		myfile.write(buf,len);
   	 		bzero(buf,MAXDATASIZE);	
		}
   	 	myfile.close();
    	if (n < 0) 
         fprintf(stderr, "finish reading from socket\n");
    	
   }

   if(!strcmp(argv[3],"PUT") || !strcmp(argv[3],"put") )
    {	
    	char* request = (char*) calloc(35, sizeof(char));
    	sprintf(request, "PUT /%s HTTP/1.1\r\n\r\n",argv[4]);
    	n = write(sockfd,request,MAXDATASIZE);
		if (n < 0) 
			fprintf(stderr, "ERROR writing to socket\n");

		ifstream in (filename, ios::in);
   	 	int temp=0;
		in.read(buf,MAXDATASIZE);
		int len=in.gcount();
		while(len>0)
		{
			temp++;
			cout<<buf<<endl;
			cout<<send(sockfd, buf, len, 0)<<endl;
			bzero(buf,MAXDATASIZE);	
			in.read(buf,MAXDATASIZE);
			len=in.gcount();
		}
   	 	in.close();
    	if (n < 0) 
         fprintf(stderr, "finish reading from socket\n");

     	
    	
   }

	close(sockfd);

	return 0;
}

