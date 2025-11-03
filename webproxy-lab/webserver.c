#include <pthread.h>
#include <_string.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> // O_RDONLY


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
        memset(response, 0, BUFFER_SIZE);
        memset(request, 0, BUFFER_SIZE);

            if (valread > 0) {
                printf("Client says:\n%s\n", buffer);

                // Parse the request

                char *token;
                char *buffer_ptr = buffer;

                // reads the client request
                int is_first_line = 1;
                // go through each line of the request
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



                        // CGI
                        if (strstr(url, "/cgi-bin/adder")) {
                            printf("Matched /cgi-bin/adder\n");

                            pid_t pid = fork();


                            if (pid < 0) {
                                    perror("Fork failed");
                                    exit(1);
                                }
                            else if (pid == 0) {
                                // CHILD PROCESS - Execute CGI script

                                // Set environment variables
                                char *query = strchr(url,'?');
                                query++;
                                printf("query: %s\n", query);
                                // After parsing the HTTP request:
                                // 사실 여기도 필요 없을지도..?
                                setenv("REQUEST_METHOD", "GET", 1);
                                setenv("QUERY_STRING", query, 1);  // parsed from URL
                                setenv("SCRIPT_NAME", "/cgi-bin/adder", 1);
                                setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
                                setenv("REQUEST_URI",url,1);
                                setenv("SERVER_SOFTWARE", "MyServer/1.0", 1);
                                setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);


                                dup2(new_socket, STDOUT_FILENO);
                                // Execute CGI script
                                execl("./cgi-bin/adder", "adder", NULL);

                            // If execl returns, it failed
                                perror("execl failed");
                                exit(1);
                            }
                            else {
                                // PARENT PROCESS - Wait for child

                                int status;
                                waitpid(pid, &status, 0);  // Wait for child to finish

                                if (WIFEXITED(status)) {
                                    printf("Child exited with status %d\n", WEXITSTATUS(status));
                                }

                            }

                        }
                        // index.html
                        else if (strstr(url, ".html")){
                            // GET
                            // index file buffer
                            // parse the url
                            // http://localhost:8080/index.html
                            // parse anything after 8080 ->

                            char *parsed_uri = url;                            // Skip leading "/" to get the filename
                            if (parsed_uri[0] == '/') {
                                parsed_uri++;
                            }

                            if (parsed_uri == NULL) {
                                    fprintf(stderr, "Invalid URL format\n");
                                    snprintf(response, sizeof(response),
                                             "HTTP/1.1 400 Bad Request\r\n"
                                             "Content-Type: text/html\r\n"
                                             "Content-Length: 50\r\n"
                                             "\r\n"
                                             "<html><body><h1>400 Bad Request</h1></body></html>");
                                    send(new_socket, response, strlen(response), 0);
                                    break;  // or return -1, depending on your control flow
                                }


                            printf("parsed uri: %s\n",parsed_uri);

                            // SECURITY: Validate the path
                            if (strstr(parsed_uri, "..") != NULL) {
                                // Reject paths with ".." to prevent directory traversal
                                fprintf(stderr, "Path traversal attempt blocked\n");
                                return -1;
                            }

                            int fd = open(parsed_uri, O_RDONLY);
                            if (fd ==-1){
                                perror("ERROR opening file");
                                snprintf(response, sizeof(response),
                                         "HTTP/1.1 404 Not Found\r\n"
                                         "Content-Type: text/html\r\n"
                                         "Content-Length: 48\r\n"
                                         "\r\n"
                                         "<html><body><h1>404 Not Found</h1></body></html>");
                                send(new_socket, response, strlen(response), 0);
                            }
                            // read the file into buffer
                            //
                            // int bytes_read = fread(response, 1, BUFFER_SIZE - 1, fp);
                            // // void * mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

                            // response[bytes_read] = '\0';  // Null-terminate it for safety

                            // snprintf(response, sizeof(response),
                            //     "HTTP/1.1 200 OK\r\n"
                            //     "Content-Type: text/plain\r\n"
                            //     "Content-Length: %zu\r\n"
                            //     "\r\n"
                            //     "%s",
                            //     strlen(response), response);

                            // send(new_socket, response, strlen(response), 0);

                            // Get file size
                             struct stat sb;
                             if (fstat(fd, &sb) == -1) {
                                 perror("ERROR getting file size");
                                 close(fd);
                                 break;
                             }
                             off_t filesize = sb.st_size;

                             // Map the file into memory
                             char *file_content = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
                             if (file_content == MAP_FAILED) {
                                 perror("ERROR mapping file");
                                 close(fd);
                                 break;
                             }
                             close(fd);

                             char header[BUFFER_SIZE];
                            snprintf(header, sizeof(header),
                                "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: %ld\r\n"
                                "\r\n",
                                (long) filesize);

                            // send the header firtst
                            send(new_socket, header, strlen(header), 0);

                            // send the content
                            send(new_socket, file_content, filesize, 0);

                            munmap(file_content, filesize);

                        }

                        else if (strstr(url, "/api/videos/sample.mp4")){
                            int video_fd = open("sample.mp4", O_RDONLY);
                            struct stat file_stat;

                            if(fstat(video_fd, &file_stat) == -1){
                                perror("ERROR OPPENING the tracking.mpg: ");
                                close(video_fd);
                                break;
                            }

                            char *map = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, video_fd, 0);
                                 // send(int socket, const void *buffer, size_t length, int flags);


                            if (map == MAP_FAILED) {
                                perror("ERROR mapping file");
                                close(video_fd);
                                break;
                            }

                            // send headers first
                            // snprintf(char * restrict str, size_t size, const char * restrict format, ...);
                            // accept-ranges -> allows you to move between plays
                            // if the format is not supported, it forwards to download
                            snprintf(response, sizeof(response),
                                "HTTP/1.1 200 OK\r\n"
                                "Content-Type: video/mp4\r\n"
                                "Content-Length: %ld\r\n"
                                "Content-Disposition: inline\r\n"
                                "Accept-Ranges: bytes\r\n"
                                "\r\n"
                                ,(long) file_stat.st_size);

                            send(new_socket,response, strlen(response),0);


                            // send the video file
                            send(new_socket, map, file_stat.st_size, 0);


                            // close them
                            munmap(map, file_stat.st_size);
                            close(video_fd);

                        }
                         // NOT CGI
                        else{
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
                        }




                        // Then execute the CGI script


                        is_first_line = 0;
                    }

                    // should i handle the rest? -> probably not


                    printf("%s\n",token);
                }



            }

        close(new_socket); // 클라이언트와의 연결 종료
    }
    close(server_fd);

    return 0;
}
