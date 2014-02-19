/*
** client.c -- a stream socket client demo
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
// #define PORT "9999" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

using namespace std;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
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
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure




	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}
	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);



	//printf("Please enter the message: ");

    bzero(buf,MAXDATASIZE);

    buf=strcat(buf,argv[3]);
    buf=strcat(buf," ");
    char * filename = argv[4];
    buf=strcat(buf,argv[4]);
    FILE* fp;
    if(!strcmp(argv[3],"GET") || !strcmp(argv[3],"get") )
    {	
    	buf=strcat(buf," HTTP/1.0");
	    n = write(sockfd,buf,MAXDATASIZE);
		if (n < 0) 
			fprintf(stderr, "ERROR writing to socket\n");
   	 	
		ofstream myfile (filename, ios::out | ios::binary);
   	 	int temp=0;
   	 	int len;
		while( (len = read(sockfd,buf,MAXDATASIZE) )>0)
		{	
			temp++;
			//cout<<len;
   	 		myfile.write(buf,len);
   	 		//cout<<buf;
   	 		bzero(buf,MAXDATASIZE);	
   	 		myfile.close();
   	 		myfile.open (filename, ios::app | ios::binary);
		}

   	 	myfile.close();
    	if (n < 0) 
         fprintf(stderr, "finish reading from socket\n");
    	
   }
    printf("sent");

	close(sockfd);

	return 0;
}

