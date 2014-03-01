/*
** server.c -- A http server supporting GET and PUT
*/
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

using namespace std;

#define http_notfound "HTTP/1.1 404 NO"
#define http_ok "HTTP/1.1 200 OK"

#define BACKLOG 10	 // how many pending connections queue will hold
#define MAXDATASIZE 100

// Function to parse and receive the request----------------------
int get_request(int new_fd, char *buffer)			
{
	int end, n;
	end = 0;
	char input;
	int pos = 0;
	while(end != 4)
	{
		n = recv(new_fd, &input, 1, 0);
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
	if (argc != 2) {
	    fprintf(stderr,"usage: ./a.out portno \n");
	    exit(1);
	}
	char* PORT=argv[1];


	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

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
	char buf[100];
	int numbytes;
	while(1) {  
	// main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,	get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);

		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
				bzero(buf,100);
				int r = get_request(new_fd, buf);
				if (r >0) 
				{
			  		char* request = strtok (buf," ");
			  		char* filepath =strtok(NULL," ");

			  		filepath++;
			  		cout<<request<<" "<<filepath;
 
			  		if(strcmp(request,"PUT")==0)
			  		{
						char receivingbuffer[MAXDATASIZE];

						if(strstr(filepath,"/")!=NULL) 
						{ filepath = strrchr(filepath, '/'); filepath++; }

						ofstream myfile(filepath,ios::out | ios::binary);

						while( (numbytes = read(new_fd, receivingbuffer, MAXDATASIZE))>0)
						{
							myfile.write(receivingbuffer,numbytes);
							bzero(receivingbuffer,MAXDATASIZE);

						}
						myfile.close();
					}
			  		
			  		numbytes=0;

			  		if(strcmp(request,"GET")==0)
			  		{ 

			  		
						if(is_file_binary(filepath))
						{
					  			FILE *fp;
								int n;
								fp = fopen(filepath,"r");
								if(fp)
								{
									char ch;
									n = write(new_fd,"HTTP/1.1 200 OK",15);
									if(n < 0) 
										printf("ERROR writing to socket");
									
									do{
										ch = fgetc(fp);
										n = write(new_fd, &ch, 1);
										if (n < 0) 
											printf("ERROR writing to socket");
												
									}while( ch != EOF );

									fclose(fp);
								}
								else
								{
									n = write(new_fd,"HTTP/1.1 404 NOT FOUND\n",strlen("HTTP/1.1 404 NOT FOUND\n"));
									if (n < 0) 
										printf("ERROR writing to socket");
								}
						}
						
						else
						{
							char sendingbuffer[MAXDATASIZE];	
							ifstream myfile;
							myfile.open(filepath,ios::in | ios::binary);
							if(myfile.fail())
							{
			  		 			send(new_fd,http_notfound,15,0);
			  					exit(1);
			  		 		}
								send(new_fd, http_ok,strlen(http_ok),0);

								while(myfile.read(sendingbuffer,1)>0)
								{
									send(new_fd, sendingbuffer, 1, 0);
								}
								myfile.close();
						}
			  		}

			  		else
			  			send(new_fd,"Invalid request",15,0);			  		

				}
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}


