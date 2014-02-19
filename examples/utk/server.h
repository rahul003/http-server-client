/**************************************************************************
 * Soufiane Naciri							  *
 * snaciri@utk.edu							  *
 * ECE 453								  *
 **************************************************************************
 * server.h - is the header file containing the function processing	  *
 * the request incoming from the client.				  *
 * Hints and help from:			 				  *
 * http://www.cs.utk.edu/~huangj/cs360/360/notes/Sockets/socketfun.c	  *	
 *									  *
 **************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <signal.h>

/* make_socket() simply gets the hostname and port passed to it
 * and makes a socket out of it and returns it in a file descriptor
 */
int make_socket(char *host, int port)
{
  int sockfd;
  struct sockaddr_in sn;
  struct hostent *he;

  if (!(he = gethostbyname(host))) {
    fprintf(stderr, "Hostname or IP invalid\n");
    exit(1);
  }

  bzero((char*)&sn, sizeof(sn));
  sn.sin_family = AF_INET;
  sn.sin_port = htons((short)port);
  sn.sin_addr = *(struct in_addr*)(he->h_addr_list[0]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (bind(sockfd, (struct sockaddr*)&sn, sizeof(sn)) == -1) {
    perror("bind()");
    exit(1);
  }
  return sockfd;
}

/* get_request() - this function listens to the port passed to it 
 * and accepts the connection between the two machines 
 */
int get_request(int sockfd)
{
  int x;
  char ip[32];
  struct hostent *he;
  struct in_addr ip4;
  struct sockaddr_in sn;

  sn.sin_family = AF_INET;

  listen(sockfd, 2); //listening to a maximum of two connections
  int size = sizeof(sn); 
  if ((x = accept(sockfd, (struct sockaddr*)&sn, &size)) == -1) {
    perror("accept()");
    exit(1);
  }

  strcpy(ip, inet_ntoa(sn.sin_addr)); //gets host ip address
  
  inet_pton(AF_INET, ip, &ip4); //uses ip address to get the name of the host
  he = gethostbyaddr(&ip4, sizeof ip4, AF_INET);

  printf("\t***Connection requested from: %s (%s)\n", he->h_name, ip);
  return x;
}
