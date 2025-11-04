#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include "csapp.h"

static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

typedef struct {
    int connfd;
    struct sockaddr_in clientaddr;
    char port[8];
} thread_args;

void *handle_client(void *vargp);

int main(int argc, char **argv) {
  if (argc != 2){
      fprintf(stderr, "Usage: %s <port>\n", argv[0]);
      exit(1);
  }

  fprintf(stderr, "Proxy starting on port %s\n", argv[1]);
  fflush(stderr);

  int client_listenfd, client_connfd, server_connfd;
  socklen_t clientlen;
  struct sockaddr_in clientaddr;

  client_listenfd = Open_listenfd(argv[1]);
  pthread_t threads;
  int i = 0;
  while (1) {
    clientlen = sizeof(clientaddr);

    fprintf(stderr, "[WAITING] Ready to accept connection...\n");
    fflush(stderr);

    client_connfd = Accept(client_listenfd, (SA *)&clientaddr, &clientlen);

    thread_args *t = malloc(sizeof(thread_args));
    t->connfd = client_connfd;
    t->clientaddr = clientaddr;
    strcpy(t->port, argv[1]);

    pthread_create(&threads, NULL, handle_client, t);
    pthread_detach(threads);  // Now we can detach
    i++;


  }

  return 0;
}

void *handle_client(void *vargp){

    thread_args *args = (thread_args *)vargp;
    int client_connfd = args->connfd;
    int server_connfd;


    fprintf(stderr, "[ACCEPTED] Connection accepted\n");
    fflush(stderr);

    // 1. READ CLIENT REQUEST
    rio_t rio_client;
    Rio_readinitb(&rio_client, client_connfd);

    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];

    fprintf(stderr, "[READING] About to read request line...\n");
    fflush(stderr);

    Rio_readlineb(&rio_client, buf, MAXLINE);

    fprintf(stderr, "[REQUEST] %s", buf);
    fflush(stderr);

    sscanf(buf, "%s %s %s", method, uri, version);

    // 2. PARSE THE URI
    char host[MAXLINE], port[MAXLINE], path[MAXLINE];
    memset(host, 0, MAXLINE);
    strcpy(port, "80");

    if (strncmp(uri, "http://", 7) == 0) {
        char *host_start = uri + 7;
        char *path_start = strchr(host_start, '/');

        if (path_start) {
            strcpy(path, path_start);
            *path_start = '\0';
        } else {
            strcpy(path, "/");
        }

        char *colon = strchr(host_start, ':');
        if (colon) {
            *colon = '\0';
            strcpy(host, host_start);
            strcpy(port, colon + 1);
        } else {
            strcpy(host, host_start);
        }
    } else {
        strcpy(path, uri);
    }

    fprintf(stderr, "[PARSED] host=%s port=%s path=%s\n", host, port, path);
    fflush(stderr);

    // 3. READ ALL CLIENT HEADERS
    fprintf(stderr, "[HEADERS] Reading headers...\n");
    fflush(stderr);

    Rio_readlineb(&rio_client, buf, MAXLINE);
    while (strcmp(buf, "\r\n") != 0 && strcmp(buf, "\n") != 0) {
        fprintf(stderr, "[HEADER] %s", buf);
        fflush(stderr);

        if (host[0] == '\0' && strncasecmp(buf, "Host:", 5) == 0) {
            char *value = buf + 5;
            while (*value == ' ') value++;

            char host_line[MAXLINE];
            sscanf(value, "%s", host_line);

            char *colon = strchr(host_line, ':');
            if (colon) {
                *colon = '\0';
                strcpy(host, host_line);
                strcpy(port, colon + 1);
            } else {
                strcpy(host, host_line);
            }
        }

        Rio_readlineb(&rio_client, buf, MAXLINE);
    }

    fprintf(stderr, "[CONNECTING] to host=%s port=%s path=%s\n", host, port, path);
    fflush(stderr);

    // 4. CONNECT TO ORIGIN SERVER
    server_connfd = Open_clientfd(host, port);
    if (server_connfd < 0) {
        fprintf(stderr, "[ERROR] Failed to connect to %s:%s\n", host, port);
        fflush(stderr);
        Close(client_connfd);
        return NULL;
    }

    fprintf(stderr, "[CONNECTED] to origin server\n");
    fflush(stderr);

    // 5. SEND REQUEST (HTTP/1.0!)
    sprintf(buf, "%s %s HTTP/1.0\r\n", method, path);
    Rio_writen(server_connfd, buf, strlen(buf));

    sprintf(buf, "Host: %s\r\n", host);
    Rio_writen(server_connfd, buf, strlen(buf));

    Rio_writen(server_connfd, user_agent_hdr, strlen(user_agent_hdr));
    Rio_writen(server_connfd, "Connection: close\r\n", 19);
    Rio_writen(server_connfd, "Proxy-Connection: close\r\n", 25);
    Rio_writen(server_connfd, "\r\n", 2);

    fprintf(stderr, "[SENT] Request to origin\n");
    fflush(stderr);

    // 6. FORWARD RESPONSE
    rio_t rio_server;
    Rio_readinitb(&rio_server, server_connfd);

    int n;
    int total_bytes = 0;
    while ((n = Rio_readnb(&rio_server, buf, MAXLINE)) > 0) {
        Rio_writen(client_connfd, buf, n);
        total_bytes += n;
    }

    fprintf(stderr, "[COMPLETE] Forwarded %d bytes\n\n", total_bytes);
    fflush(stderr);

    Close(server_connfd);
    Close(client_connfd);

    free(args);
    return NULL;



}
