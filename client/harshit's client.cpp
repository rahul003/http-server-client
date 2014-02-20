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

	cout<<"hy "<<request<<endl;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

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
		sprintf(getRequest, "GET /%s HTTP/1.0\r\nhost: %s\r\n\r\n",argv[4],argv[1]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP
	
	else
	{
		if(strcmp(argv[3],"PUT")==0)
		sprintf(getRequest, "PUT /%s HTTP/1.0\r\nhost: %s\r\n\r\n",argv[4],argv[1]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP

		else
			{cout<<"Invalid request";
			exit(0);}
	}


	cout<<"sending req"<<endl;
	cout<<getRequest<<endl;

	int sent=send(sockfd,getRequest, strlen(getRequest),0);
	cout<<sent;
	cout<<"sent "<<strlen(getRequest)<<" ";
	int n;
		  // while(inout(sockfd, 1) > 0);
	char* filename=argv[4];
	if(strcmp(argv[3],"GET")==0)
	{		
			char recvpkt[MAXDATASIZE];
					//send(fd, header, strlen(header), 0); // send the header
			 n = read(sockfd,recvpkt, 15);
			if (n < 0)
			printf("ERROR reading from socket");

		if(!strcmp(recvpkt, "HTTP/1.1 404 NO"))				//Error message displayed if the file is not found
		 {
		 	printf("404 NOT FOUND");
		}
		else
			printf("\n%s",recvpkt);
		
		if(strcmp(recvpkt, "HTTP/1.1 200 OK")==0)	
		{
				
				ofstream myfile(filename,ios::out | ios::binary);

					while( (numbytes = read(sockfd, recvpkt, 1))>0)
					{
						//cout<<"getting";//<<recvpkt;
						myfile.write(recvpkt,1);

					}

					myfile.close();
		}
	}

	else 
		//if(strcmp(argv[3],"PUT")==0)
	{	
		int is_filetype_binary;
		string file= filename;
		string extension = file.substr(file.find_last_of(".")+1);
		if(	extension=="png" || extension=="gif" || extension=="jpg"	|| extension=="pdf"|| extension=="jpeg")
		{
			is_filetype_binary = 1;
		}
		else
			is_filetype_binary = 0;

		if(is_filetype_binary)
		{
			char tosend[MAXDATASIZE];
			ifstream myfile;
			myfile.open(filename,ios::in | ios::binary);
			cout<<"enter";

			while( numbytes = myfile.read(tosend,1) > 0)
			{
				//cout<<myfile.gcount()<<" "<<tosend;
				send(sockfd, tosend, 1, 0);
				//cout<<"getting";//<<recvpkt;
				
			}
		//myfile<<"\n";
			myfile.close();
		}

		// CHA BY CHAR C Code-----------------------------------

		// cout<<"putting..";
		// char tosend[MAXDATASIZE];
		else
		{
			FILE *fp;
			char ch;

				int n;
				fp = fopen(filename,"r");
				if(fp)
				{
					// x = '1';
					// n = write(sockfd, &x, 1);
					// if(n < 0) 
					// 	printf("ERROR writing to socket");
					
					do{
						ch = fgetc(fp);
						n = write(sockfd, &ch, 1);
						if (n < 0) 
							printf("ERROR writing to socket");
								
					}while( ch != EOF );

					fclose(fp);
					// n = read(sockfd,tosend,1);
					// printf("%s\n", tosend);
					cout<<"sent";



				}
				else
				{
					printf("File not found\n");
				}
		}
	}			
		




	freeaddrinfo(servinfo); // all done with this structure

	close(sockfd);

	return 0;
}

