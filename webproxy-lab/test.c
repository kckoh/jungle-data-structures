#include <_stdlib.h>
#include <_string.h>
#include <string.h> //strdup and strsep
#include <assert.h> // asert
#include <stdio.h> //printf
#include <stdlib.h> // free, getenv
#include <ctype.h> // isdigit


int addChars(char c1, char c2, int *result) {
    if (!isdigit(c1) || !isdigit(c2)) {
        return 0;  // 0 = error (not digits)
    }
    printf("%c %c",c1,c2);
    *result = (c1 - '0') + (c2 - '0');
    return 1;  // 1 = success
}

int main(){
    // char *token, *string, *tofree;

    // tofree = string = strdup(
    //         "HTTP/1.1 200 OK\r\n"
    //         "Content-Type: text/plain\r\n"
    //         "Content-Length: %zu\r\n"
    //         "\r\n"
    //         "Some body starts here\n"
    // );

    // while ((token = strsep(&string, "\r\n")) != NULL){
    //     if (*token == '\0'){
    //         continue;
    //     }
    //     printf("%s\n",token);
    // }


    // strstr -> finds substring
    // strchr -> finds the character in a string
    // <unistd.h>
    // dup2 -> newfd refer to the same underlying file or socket as oldfd
    // int dup2(int oldfd, int newfd);
    // execl -> execute a new program
    //
    // wait -> <unistd.h>
    // header: sys/wait.h
    // CGI
    //


    char *path = "cgi-bin/adder?1&2";

    if (strstr(path, "cgi-bin")) {
            char *query = strchr(path, '?');  // points to "?1&2"
            if (!query || !*(query + 1)) {
                printf("No query found\n");
                return 1;
            }

            // move past '?'
            query++;

            char *amp = strchr(query, '&');
            if (!amp) {
                printf("Invalid query format\n");
                return 1;
            }

            char a = *query;        // '1'
            char b = *(amp + 1);    // '2'
            int result;

            printf("a=%c, b=%c\n", a, b);

            if (addChars(a, b, &result))
                printf("Total = %d\n", result);
            else
                printf("Invalid digits.\n");
        }

     return 0;
}
