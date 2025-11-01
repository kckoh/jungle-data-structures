#include "csapp.h"
#include <_string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
// socket -> sockaddr_in -> bind -> listen -> accept ->read/send -> close

    int server_fd, new_socket;
    // socket struct
    //struct sockaddr_in {
    //     sa_family_t    sin_family; // Address family (AF_INET for IPv4)
    //     in_port_t      sin_port;   // Port number (network byte order)
    //     struct in_addr sin_addr;   // IPv4 address (struct with s_addr)
    //     unsigned char  sin_zero[8]; // Padding (not used)
    // };
    // struct sockaddr_in {
	// __uint8_t       sin_len;
	// sa_family_t     sin_family;
	// in_port_t       sin_port;
	// struct  in_addr sin_addr;
	// char            sin_zero[8];
    //    };
    struct sockaddr_in address;
    // buffer size = 1024
    char buffer[BUFFER_SIZE] = {0};
    // size of the sockaddr_in struct
    int addrlen = sizeof(address);

    // 1. Create socket
    // socket accepts a several argument
    //  socket(int domain, int type, int protocol);
    // domain -> ipv4 or ipv6:
    // returns -1 if error or the file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to IP/Port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // any local IP
    address.sin_port = htons(PORT);        // convert to network byte order - PORT = 8080
    //   bind(int socket, const struct sockaddr *address, socklen_t address_len);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen(int socket, int backlog);
    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // HTTP FORMAT\r\n
    // GET /index.html HTTP/1.1\r\n
    // Host: www.example.com\r\n
    // User-Agent: Mozilla/5.0\r\n
    // Accept: text/html\r\n

    // 요청라인
    // 헤더
    // \r\n
    // body

    // 4. Accept a connection
    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;  // 오류 발생 시 서버 전체 종료하지 말고 다음 연결 대기
        }

        // 클라이언트와 통신
        memset(buffer, 0, BUFFER_SIZE);

        // char *output[BUFFER_SIZE];
        // memset(output, 0, sizeof(output));
        // strcpy(output, "RECEIVED: ");
        // strcat(output, buffer);

        ssize_t valread = recv(new_socket, buffer, BUFFER_SIZE, 0);

        // strcp(output,buffer);
        char request[BUFFER_SIZE];
        // response 버퍼안에다가 넣는다
        char response[BUFFER_SIZE];

            if (valread > 0) {
                printf("Client says:\n%s\n", buffer);

                // Parse the request

                char *token;
                char *buffer_ptr = buffer;

                // reads the client request
                int is_first_line = 1;
                while ((token = strsep(&buffer_ptr, "\r\n")) != NULL){
                    if (*token == '\0'){
                        continue;
                    }

                    // splits them into request line and header
                    // request line GET /index.html HTTP/1.1
                    if (is_first_line){
                        char *method = strsep(&token, " ");
                        char *url = strsep(&token, " ");
                        char *version = strsep(&token, " ");

                        // ONLY SUPPORT GET Method
                        if (strcmp(method, "GET") != 0) {
                            char *errorMsg = "PLEASE ONLY USER GET";
                            snprintf(response, sizeof(response),
                                "HTTP/1.1 405 Method Not Allowed\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: %zu\r\n"
                                "\r\n"
                                "%s",
                                strlen(errorMsg), errorMsg);

                            send(new_socket, response, strlen(response), 0);
                            close(new_socket);
                            continue;
                        }



                        // it includes /index.html -> ++url skips the /
                        FILE *fp = fopen(++url, "r");
                        // if path not found close the connection
                        if (fp == NULL) {
                            perror("ERROR opening file");
                            char *errorMsg = "PATH NOT FOUND";
                            snprintf(response, sizeof(response),
                                "HTTP/1.1 404 Method Not Allowed\r\n"
                                "Content-Type: text/plain\r\n"
                                "Content-Length: %zu\r\n"
                                "\r\n"
                                "%s",
                                strlen(errorMsg), errorMsg);

                            send(new_socket, response, strlen(response), 0);
                            close(new_socket);
                            continue;
                        }



                        is_first_line = 0;
                    }

                    // should i handle the rest? -> probably not

                    printf("%s\n",token);
                }


                // GET
                // index file buffer
                FILE *fp = fopen("index.html", "r");
                if (fp ==NULL){
                    perror("ERROR opening file");
                }
                // read the file into buffer
                int bytes_read = fread(response, 1, BUFFER_SIZE - 1, fp);
                response[bytes_read] = '\0';  // Null-terminate it for safety

                snprintf(response, sizeof(response),
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: %zu\r\n"
                    "\r\n"
                    "%s",
                    strlen(response), response);

                send(new_socket, response, strlen(response), 0);
            }

        close(new_socket); // 클라이언트와의 연결 종료
    }
    close(server_fd);

    return 0;
}
