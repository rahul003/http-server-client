#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <fstream>

#define PORT "9999"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100
#define http_notfound "notfound"
#define http_ok "ok"
using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(0);
}


void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int recv_request(int newsockfd, char *buffer)			//Recv_request to read the incoming request character by character in a buffer
{
	cout<<" hi enterin";
	int end, n;
	end = 0;
	char input;
	int pos = 0;
	while(end != 4)
	{
		n = recv(newsockfd, &input, 1, 0);
		if(n < 0)
			{
				printf("ERROR reading from socket");
				return 0;
			}
		buffer[pos] = input;
		pos++;
		if(end == 0)
		{
		
			if(input == '\r')
				end = 1;
		}
		else if(end == 1)
		{
			if(input == '\n')
				end = 2;
			else
				end = 0;
		}			
		else if(end == 2)
		{
			if(input == '\r')
				end = 3;
			else
				end = 0;
		}			
		else if(end == 3)
		{
			if(input == '\n')
				end = 4;
			else
				end = 0;
		}			
		bzero(&input, 1);
		
	}
	buffer[pos] = '\0';
	return 1;
}
int main(void)
{
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	int n;
	char buf[MAXDATASIZE];
	char s[INET6_ADDRSTRLEN];
	int rv;

	int looper;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		printf("forke1d\n");
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			cout<<"forked"<<endl;
			bzero(buf,MAXDATASIZE);
 
			n = recv_request(new_fd,buf);
     		//n = read(new_fd,buf,MAXDATASIZE);
     		 if (n < 0) 
        		 error("ERROR reading from socket");
        	cout<<buf<<endl;

        	cout<<"buf is:"<<buf<<endl;

        	char * tokens = strtok (buf,"\n");
        	//printf("%s",tokens);
	   		
        	char * request = strtok (buf," ");
        	char * filename = strtok (NULL," ");
        	filename++;
        	printf("request: %s, filename: %s\n",request,filename);

        	if(!strcmp(request,"GET"))
		     {

		     	ifstream in(filename, ios::in | ios::binary);
		     	if(in.fail())
		     	{
		     	cout<<http_notfound;
				send(new_fd, http_notfound, strlen(http_notfound), 0); //send HTTP 404 Not Found if the file does not exist
			    } 

			    else {
				send(new_fd, http_ok, strlen(http_ok), 0); // if found send HTTP 200 OK  
				//send(fd, header, strlen(header), 0); // send the header
				cout<<endl<<"Sent ok"<<endl;

				int temp=0;
				in.read(buf,MAXDATASIZE);
				int len=in.gcount();
				while(len>0)
				{
					temp++;
					send(new_fd, buf, len, 0);
					in.read(buf,MAXDATASIZE);
					len=in.gcount();
				}

		  			// while ( len = in.read(buf,MAXDATASIZE).gcount() > 0)
		  			// { //sends the content of the file
		  			// 	cout<<len;
		  			// 	
		  			// 	cout<<"ASda";
		  			// 	cout<<buf;
				  	//  	send(new_fd, buf, len, 0);
		  			// }
		  			//cout<<"wrote"<<temp<<endl;
				}	
        	}


        	if(!strcmp(request,"PUT") || !strcmp(request,"put") )
		    {	//cout<<"put"<<endl;
				ofstream myfile (filename, ios::out | ios::binary);
		   	 	int temp=0;
		   	 	int len;
		   	 	bzero(buf,MAXDATASIZE);	
				while( (len = read(new_fd,buf,MAXDATASIZE) )>0)
				{	//cout<<len;
					temp++;
					cout<<temp;
		   	 		myfile.write(buf,len);
		   	 		bzero(buf,MAXDATASIZE);	
				}
				//cout<<"lenght is"<<len;
				n = write(new_fd,"200 OK File Created\n",strlen("200 OK FIle Created\n"));
		   	 	myfile.close();
		    	//if (n < 0) 
		         //fprintf(stderr, "finish reading from socket\n");
		   }
        	
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}

