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
#include <fstream>

using namespace std;

// #define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 128 // max number of bytes we can get at once




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
	char buf[MAXDATASIZE],request[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 5) {
	    fprintf(stderr,"usage: hostname portno request\n");
	    exit(1);
	}

	char *PORT;
	PORT=argv[2];

	 bzero(request,MAXDATASIZE);
	//char buf[MAXDATASIZE];

	strcat(request,argv[3]);
	strcat(request," ");
	strcat(request,argv[4]);

	
	//strcat(buf,argv[5]);

	cout<<"hy "<<request;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	//for(p = servinfo; p != NULL; p = p->ai_next) {
			p=servinfo;
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("client: socket");
			exit(0);
			//continue;
		}

		int bindfd = bind(sockfd, p->ai_addr, p->ai_addrlen);

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connection error");
			exit(0);
			//continue;
		}


        int optval = 1; //is 
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	
	// inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	// printf("client: connecting to %s\n", s);
	// if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	//     perror("recv");
	//     exit(1);
	// }

	// buf[numbytes] = '\0';

	//printf("client: received '%s'\n",buf);
    char getRequest[1024];

    if(strcmp(argv[3],"GET")==0)
		sprintf(getRequest, "GET /%s HTTP/1.0\n\n",argv[4]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP
	
	else
	{
		if(strcmp(argv[3],"PUT")==0)
		sprintf(getRequest, "PUT /%s \n",argv[4]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP

		else
			{cout<<"Invalid request";
			exit(0);}
	}





	int sent=send(sockfd,getRequest, strlen(getRequest),0);

	cout<<"sent "<<argv[3]<<" ";
		  // while(inout(sockfd, 1) > 0);
	char* filename=argv[4];
	if(strcmp(argv[3],"GET")==0)
	{		
		char recvpkt[MAXDATASIZE];
		
		ofstream myfile(filename,ios::out | ios::binary);

		while( (numbytes = read(sockfd, recvpkt, MAXDATASIZE))>0)
		{
			//cout<<"getting";//<<recvpkt;
			myfile.write(recvpkt,numbytes);
			bzero(recvpkt,MAXDATASIZE);
			myfile.close();
			myfile.open(filename,ios::app | ios::binary);

		}

		myfile.close();
	}

	// else{	
	// 		//sleep(10);
	// 		char tosend[MAXDATASIZE];
	// 		ifstream myfile;
	// 		myfile.open(filename,ios::in | ios::binary);

	// 		while( numbytes = myfile.read(tosend,MAXDATASIZE-1) > 0)
	// 		{
	// 			cout<<myfile.gcount()<<" "<<numbytes;
	// 			send(sockfd, tosend, myfile.gcount(), 0);
	// 			//cout<<"getting";//<<recvpkt;
				
	// 		}
	// 	//myfile<<"\n";
	// 	myfile.close();


	// 		//  FILE *fp;
	// 		//  if ((fp = fopen(filename, "r")) == NULL )
	// 		// 	{
	// 		// 		//send(new_fd,"404 Not Found",13,0);
	// 		// 		cout<<"no file found";
	// 		// 		exit(1);
	// 		// 	}
				  
	// 		// 	cout<<"File found";
	// 		// 		int count=0;
	// 		// 		

	// 		// while (fgets ( tosend, MAXDATASIZE, fp ) ) 
	// 		// { //sends the content of the file
	// 		// 	//cout<<" sending....";
	// 		// 		 send(sockfd, tosend, strlen(tosend), 0);
	// 		// }
	// }

	// if ((numbytes = recv(sockfd, page, MAXDATASIZE, 0)) >0) {
	// 	  cout<<"received"<<page;
	// 		    exit(1);
	// }
	 //  while(inout(sockfd, 1) > 0);


	freeaddrinfo(servinfo); // all done with this structure

	close(sockfd);

	return 0;
}
