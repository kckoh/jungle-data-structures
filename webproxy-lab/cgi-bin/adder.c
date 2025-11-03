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

    // ?num1=1&num2=2
    char *path = getenv("REQUEST_URI");
    if (!path) {
        printf("REQUEST_URI not set\n");
        return 1;
    }

    // printf("%s",path);
    if (strstr(path, "cgi-bin")) {
        char *query = strchr(path, '?');  // points to ?num1=1&num2=2

        if (!query || !*(query + 1)) {
            printf("No query found\n");
            return 1;
        }

        char *amp = strchr(query, '&');
        if (!amp) {
            printf("Invalid query format\n");
            return 1;
        }

        char *num_str_1 = strstr(query,"num1=");
        char *num_str_2 = strstr(query,"num2=");
        if (num_str_1 == NULL || num_str_2 == NULL) {
            printf("Missing num1 or num2\n");
            return 1;
        }

        num_str_1 +=5;
        num_str_2 +=5;
        // move past '?'
        // query++;



        int a = atoi(num_str_1);
        int b = atoi(num_str_2);
        int result = a + b;

        char response[64];
        snprintf(response, sizeof(response), "%d", result);
        printf("HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %zu\r\n"
                "\r\n"
                "%s",
                strlen(response), response);

        // char a = *query;        // '1'
        // char b = *(amp + 1);    // '2'
        // int result;
        // char response[64];


        // if (addChars(a, b, &result)) {
        //     // "HTTP/1.1 200 OK\r\n"
        //     // "Content-Type: text/plain\r\n"
        //     // "Content-Length: %zu\r\n"
        //     // "\r\n"
        //     // "%s",
        //     // strlen(response), response);
        //     snprintf(response, sizeof(response), "%d", result);
        //     printf("HTTP/1.1 200 OK\r\n"
        //            "Content-Type: text/plain\r\n"
        //            "Content-Length: %zu\r\n"
        //            "\r\n"
        //            "%s",
        //            strlen(response), response);

        // } else {
        //     printf("Content-Type: text/plain\r\n\r\n");
        //     printf("Invalid digits.\n");
        // }
        }

     return 0;
}
