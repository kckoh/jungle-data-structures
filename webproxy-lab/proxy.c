#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "csapp.h"

#define MAX_CACHE_SIZE 1049000   /* 1 MiB */
#define MAX_OBJECT_SIZE 102400   /* 100 KiB */

// STEPS to implement
// 1. initialize the cache_t -> DONE
// 2. check if the total_size exceeds the max_cache_size
// 4. check if it's cached by iterating the head
// 5. not found -> check if the total_size exceeds the max_cache_size
// 6. if it exceeds, keep removing the tail until the space is available
// 7 otherwise update the head pointer
// 6. found -> just return the data


typedef struct cache_block {
    char url[MAXLINE];      // key (ex: "http://www.cmu.edu/index.html")
    char *data;             // malloc된 실제 object 데이터
    int size;               // object 크기
    int reference_bit;      // LRU approximation: clock
    struct cache_block *prev;
    struct cache_block *next;
} cache_block_t;

typedef struct {
    cache_block_t *head;    // MRU (가장 최근 사용)
    cache_block_t *tail;    // LRU (가장 오래된)
    int total_size;
    pthread_rwlock_t lock;  // 읽기/쓰기 동기화
} cache_t;

static cache_t cache;

// 전방 선언
void serve_from_cache(int fd, cache_block_t *cache_blk);
void move_to_front(cache_t *cache_table, cache_block_t *cache_blk);
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

typedef struct {
    int connfd;
    struct sockaddr_in clientaddr;
    char port[8];
} thread_args;

void *handle_client(void *vargp);

void cache_init(cache_t *cache);

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

  //1.  initialize the cache
  cache_init(&cache);

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


    char full_url[MAXLINE];
    snprintf(full_url, MAXLINE, "%s:%s%s", host, port, path);

    // iterate to find the cache
    //2. iterate the linked list

    cache_block_t *temp = cache.head;
    while (temp != NULL){
        if (strcmp(full_url, temp->url) == 0){
            // Cache HIT!
            temp->reference_bit = 1;

            // Serve the entire cached HTTP response
            serve_from_cache(client_connfd, temp);

            Close(client_connfd);
            free(args);
            return NULL;
        }
        temp = temp->next;
    }


    // NOT FOUND
    // store the data
    // check if the total_size exceeds the max_cache_size
    // 6. if it exceeds, keep removing the tail until the space is available
    // 7 otherwise update the head pointer
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
    char response_buf[MAX_OBJECT_SIZE];  // Buffer to accumulate entire response

    rio_t rio_server;
    Rio_readinitb(&rio_server, server_connfd);


    // copy all headers + data
    int n;
    int total_bytes = 0;
    while ((n = Rio_readnb(&rio_server, buf, MAXLINE)) > 0) {
        // Forward to client
        Rio_writen(client_connfd, buf, n);

        // Accumulate for caching (if small enough)
        if (total_bytes + n <= MAX_OBJECT_SIZE) {
            memcpy(response_buf + total_bytes, buf, n);
        }

        total_bytes += n;
    }


    // typedef struct cache_block {
    //     char url[MAXLINE];      // key (ex: "http://www.cmu.edu/index.html")
    //     char *data;             // malloc된 실제 object 데이터
    //     int size;               // object 크기
    //     struct cache_block *prev;
    //     struct cache_block *next;
    // } cache_block_t;

    // typedef struct {
    //     cache_block_t *head;    // MRU (가장 최근 사용)
    //     cache_block_t *tail;    // LRU (가장 오래된)
    //     int total_size;
    //     pthread_rwlock_t lock;  // 읽기/쓰기 동기화
    // } cache_t;

    // Now cache it if it fits
    // LRU Approximation with Clock
    if (total_bytes <= MAX_OBJECT_SIZE) {



        // LRU eviction
        while (cache.total_size + total_bytes > MAX_CACHE_SIZE){
            if (cache.tail == NULL) break;
            // General idea
            // STEPS
            // save the tail and iterate from the tail
            cache_block_t *tail = cache.tail;
            // if the reference bit = 1 -> second chance
            // from the tail, set all the reference bit to 0
            if (tail->reference_bit == 1) {
                      tail->reference_bit = 0;  // ← 한 번만!

                      // tail에서 제거
                      cache.tail = tail->prev;
                      if (cache.tail) {
                          cache.tail->next = NULL;
                      }

                      // head로 이동
                      tail->prev = NULL;  // ← 추가!
                      tail->next = cache.head;
                      if (cache.head) {
                          cache.head->prev = tail;
                      }
                      cache.head = tail;
                      if (cache.tail == NULL) {  // ← 추가!
                          cache.tail = tail;
                      }

                      continue;
                  }
            //
            // if reference bit = 0, remove
            cache.tail = tail->prev;
            //
            if( cache.tail){
               cache.tail ->next = NULL;
            }
            else{
                cache.head = NULL;
            }
            cache.total_size -=tail->size;

            free(tail->data);
            free(tail);

        }


        // create a cache block -> make sure to use the full_url
        cache_block_t *t =malloc(sizeof(cache_block_t));
        strcpy(t->url, full_url);
        t->data = malloc(total_bytes);
        memcpy(t->data, response_buf, total_bytes);
        t->size = total_bytes;
        t->reference_bit = 1;


        // cache update
        t->prev = NULL;
        t->next = cache.head;
        if (cache.head != NULL) {  // ← NULL 체크!
            cache.head->prev = t;
        } else {
            cache.tail = t;  // ← 첫 항목이면 tail도!
        }
        cache.head = t;

        cache.total_size += total_bytes;
    }


    fprintf(stderr, "[COMPLETE] Forwarded %d bytes\n\n", total_bytes);
    fflush(stderr);

    Close(server_connfd);
    Close(client_connfd);

    free(args);
    return NULL;



}


void cache_init(cache_t *cache) {
    cache->head = NULL;
    cache->tail = NULL;
    cache->total_size = 0;
    pthread_rwlock_init(&cache->lock, NULL);  // if you use threading
}




void serve_from_cache(int fd, cache_block_t *cache_blk)
{
    // Just write the entire cached response (status line + headers + body)
    Rio_writen(fd, cache_blk->data, cache_blk->size);
}


void move_to_front(cache_t *cache_table, cache_block_t *cache_blk){
    // Already at front? Do nothing
    if (cache_table->head == cache_blk){
        return;
    }

    // REMOVE from current position
    if (cache_blk->prev != NULL){
        cache_blk->prev->next = cache_blk->next;
    }

    if (cache_blk->next != NULL){
        cache_blk->next->prev = cache_blk->prev;
    } else {
        // Was the tail - update tail pointer
        cache_table->tail = cache_blk->prev;
    }

    // MOVE to front
    cache_blk->prev = NULL;
    cache_blk->next = cache_table->head;

    if (cache_table->head != NULL){
        cache_table->head->prev = cache_blk;
    }

    cache_table->head = cache_blk;

    // Edge case: if list was empty
    if (cache_table->tail == NULL){
        cache_table->tail = cache_blk;
    }
}
