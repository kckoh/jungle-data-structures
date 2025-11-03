#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
// https://www.cs.cmu.edu/afs/cs/academic/class/15213-f08/www/code/tiny/cgi-bin/
// https://csapp.cs.cmu.edu/3e/proxylab.pdf
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";


// Part 1 -> HTTP/1.0 GET requests
// GET the port and
int main(int argc, char **argv) {
  printf("%s\n", user_agent_hdr);

  if (argc != 2){
      fprintf(stderr, "please include port", argv[0]);
      exit(1);
  }
  printf("Proxy starting on port %s\n", argv[1]);


  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_in clientaddr;

  listenfd = Open_listenfd(argv[1]);
  while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
	// TODO: Implement proxy logic here
	// Parse the HTTP request
	// Forward to the destination server
	// Return response to client

	Close(connfd);
  }

  return 0;
}
