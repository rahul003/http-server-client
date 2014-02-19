/***************************************************************************
 *  Soufiane Naciri                                                        *
 *  snaciri@utk.edu                                                        *
 *  ECE 453                                                                *
 *  11/30/2009                                                             *
 ***************************************************************************
 *  server.c - This program is a simple http server. It accepts as command *
 *  line argument a root directory and a port number to initialize the     *
 *  server and waits for incoming connections. Normally, all the request   *
 *  should be http requests. The program then receives the request and     *
 *  parses it to get the file requested. If the program finds the file     *
 *  it sends to the client an HTTP OK reply along with a header and the    *
 *  file content.							   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <dirent.h>
#include "server.h"

/* main() does the principal work for the program, it checks for errors
 * and calls two functions, one to make the socket and another to accept
 * the connection, main then gets the file descriptor and processes data
 * to send to the client.
 */

main(int argc, char **argv)
{
  FILE *fp;
  DIR *dp; //directory to make available
  char *dir, *req, *newpath, *header;
  char get[3], *path, http[9];
  char buffer[81];
  char *hostname;
  char hostpath[1024];
  int port, sockfd, fd;
  char http_ok[] = "HTTP/1.0 200 OK\n";
  char http_notfound[] = "HTTP/1.0 404 Not Found\n";

  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime ( &rawtime );

  req = (char*)malloc(1024*sizeof(char));
  path = (char*)malloc(1024*sizeof(char));
  header = (char*)malloc(1024*sizeof(char));
  newpath = (char*)malloc(1024*sizeof(char));

  if (argc != 4) {
    fprintf(stderr, "usage: [hostname] [directory] [port]\n");
    exit(1);
  }
  
  hostname = argv[1];
  dir = argv[2];
  port = atoi(argv[3]);

  if ( (dp = opendir(dir)) == NULL) {
    fprintf(stderr, "Directory not found\n");
    exit(1);
  }

  //creates a socket given hostname and port number
  sockfd = make_socket(hostname, port);

  for(;;){ //loops forever 
  	printf("\n  Waiting for incoming connection...\n\n");
  
  	fd = get_request(sockfd); //accepts connection from made socket
 
  	recv(fd, req, 100, 0); //receives the http get request

  	sscanf(req, "%s %s %s", get, path, http); //reads and parses the http get request

  	printf("\n\t***Processing request...\n");
  	newpath = path + 1; //ignores the first slash
  	sprintf(hostpath,"%s/%s", dir, newpath);

 	 //makes the header to send along with the requested file
  	sprintf(header,"Date: %sHostname: %s:%d\nLocation: %s\nContent-Type: text/html\n\n", asctime(timeinfo), hostname, port, newpath);

  	printf("\n\t***Sending data to remote host...\n");

	// looks for the requested file if it exists within the given directory
  	if ((fp = fopen(hostpath, "r")) == NULL ){
		send(fd, http_notfound, strlen(http_notfound), 0); //send HTTP 404 Not Found if the file does not exist
	  } else {
		send(fd, http_ok, strlen(http_ok), 0); // if found send HTTP 200 OK  
		send(fd, header, strlen(header), 0); // send the header

  		while ( fgets ( buffer, 81, fp ) ) { //sends the content of the file
		   send(fd, buffer, strlen(buffer), 0);
  		}
	}

	printf("\n\t***End of tranmission\n");

	close(fd); 
}

 close(sockfd);

 free(req);
 free(path);
 free(header);
 free(newpath);
}
