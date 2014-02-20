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
	int rv;
	char s[INET6_ADDRSTRLEN];
 	int n;

	if (argc != 5) {
	    fprintf(stderr,"usage: client hostname port requesttype filepath\n");
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


	int optval = 1; //is 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    freeaddrinfo(servinfo); // all done with this structure
    bzero(buf,MAXDATASIZE);
    char * filepath = argv[4];
   	cout<<"Request is: "<<argv[3]<<endl;
   	cout<<"Path is: "<<argv[4]<<endl;

    if(!strcmp(argv[3],"GET") || !strcmp(argv[3],"get") )
    {	


    	char * request;
    	sprintf(buf, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n",argv[4],argv[2]);
	    n = write(sockfd,buf,MAXDATASIZE);
		if (n < 0) 
			fprintf(stderr, "ERROR writing to socket\n");
   	 	

   	 	char  statusheader[MAXDATASIZE];
   	 	bzero(statusheader,MAXDATASIZE);	
   	 	n = read(sockfd,statusheader, 15);
   	 	//cout<<"status header is: "<<statusheader<<endl;
		if (n < 0)
			printf("ERROR reading from socket");

		if(!strcmp(statusheader, "HTTP/1.1 404 NO"))				//Error message displayed if the file is not found
		{
		 	printf("404 NOT FOUND");
		}
		else
			printf("Status is: %s\n",statusheader);

		if(strcmp(statusheader, "HTTP/1.1 200 OK")==0)	
		{
				char * file;
				if(strstr(filepath,"/")!=NULL)
				{
					char * file =  strrchr(filepath, '/');
					file++;
				}

				else{
					file = filepath;
				}

			ofstream myfile (file, ios::out | ios::binary);
	   	 	int len;
	   	 	len = read(sockfd,buf,MAXDATASIZE);

			while( (len = read(sockfd,buf,1)) >0)
			{	
	   	 		myfile.write(buf,len);
	   	 		bzero(buf,1);	
	   	 		
			}
	   	 	myfile.close();
	   	 	cout<<"File received"<<endl;
    	}
   }

   if(!strcmp(argv[3],"PUT") || !strcmp(argv[3],"put") )
    {	
  		char getRequest[1024];
		if(strcmp(argv[3],"PUT")==0)
			sprintf(getRequest, "PUT /%s HTTP/1.1\r\nhost: %s\r\n\r\n",argv[4],argv[1]);//sprintf(getRequest, "GET / HTTP/1.0\nHOST: %s\n\n", argv[1]); // create a get request only for the IP

		else
			{cout<<"Invalid request";
			exit(0);}



    	int sent=send(sockfd,getRequest, strlen(getRequest),0);

    	if(is_file_binary(filepath))
    	{
    		//char * sendingbuf = (char*) calloc(1, sizeof(char));
    		char sendingchar[MAXDATASIZE];


    		char * file;
				if(strstr(filepath,"/")!=NULL)
				{
					char * file =  strrchr(filepath, '/');
					file++;
				}

				else{
					file = filepath;
				}


			ifstream myfile;
			myfile.open(file,ios::in | ios::binary);
			while( numbytes = myfile.read(sendingchar,1) > 0)
			{
				n= send(sockfd, sendingchar, 1, 0);
				if (n < 0)
					printf("ERROR writing to socket");
			}
			myfile.close();
    	}

    	else
    	{
    
    		FILE *fp;
			char sendingchar;


			fp = fopen(filepath,"r");
			
			if(fp)
			{
				//cout<<"withing file writing"<<endl;
				do{
					sendingchar = fgetc(fp);
				
					if(sendingchar!=EOF)
						n = write(sockfd, &sendingchar, 1);
					//cout<<n;
					if (n < 0) 
						printf("ERROR writing to socket");			
				}while( sendingchar != EOF );

			fclose(fp);
			}
			else
			{
				printf("File not found\n");
			}
			cout<<"200 OK File Created"<<endl;
    	}

   }
   
   	
	close(sockfd);

	return 0;
}

