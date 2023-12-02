#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_LENGTH 1000

#define NOT_FOUND "HTTP/1.1 404 Not Found\r\n"
#define OK "HTTP/1.1 200 OK\r\n"
#define BLANK_LINE "\r\n"

typedef struct {
    SOCKET socket;
    clock_t last_request;
    char closed;
}ConnectionArgs;

typedef struct {
    ConnectionArgs* th_args;
    HANDLE thread;
}Connection;


void print_optns();
void parse_rqln(char*, char*, char*);
void parse_request(char*, char*, char*);
int handle_get(SOCKET, char*);
int handle_post(SOCKET);
int handle_request(char*, SOCKET);


void connection(void* args)
{
    ConnectionArgs* c_args = args;
    c_args->last_request = clock();
    c_args->closed = 0;
    printf("Connection thread created\n");
    char recv_buf[BUFFER_LENGTH];

    //Waiting for a request from the client
    while(1)
    {
        int recieved = recv(c_args->socket, recv_buf, BUFFER_LENGTH, 0);
        if (recieved >= 0)
        {
            
            printf("Bytes received: %d\n", recieved);
            if(recieved > 0)
            {
                printf("Recieved: %s\n", recv_buf);
                int sendRes = send(c_args->socket, recv_buf, recieved, 0);
                // c_args->last_request = clock();
                // int send_result = handle_request(recv_buf, c_args->socket);
                // if (send_result == SOCKET_ERROR)
                // {
                //     printf("response failed with error: %d\n", WSAGetLastError());
                //     closesocket(c_args->socket);
                //     c_args->closed = 1;
                //     return;
                // }
            }

        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(c_args->socket);
            c_args->closed = 1;
            return;
        }
    }
}


//-----------------------REQUEST PARSING--------------------------//
//Parses a HTTP request sent by a client and prints the type and options.
// method and path are output pointers 
//method contains "GET" or "POST".
//path contains path of the requested file in case of GET.
void parse_request(char* request, char* method, char* path)
{
    //Parsing the request line
    char* request_line = strtok(request, "\n");
    char* url;
    //Extract method and url from request line
    parse_rqln(request_line, method, url);

    char get[] = "GET";
    if (strcmp(method, get) == 0)
    {
        path = (char*) malloc(32);
        sscanf(url, "/%s", path);
    }
    

    free(url);
    //Parsing the request options
    print_optns();
}


// Parses a request line and outputs method and url.
// the output pointers are allocated by this function, so don't allocate them beforehand.
void parse_rqln(char* request_line, char* method, char* url)
{
    method = (char*) malloc(5);
    url = (char*) malloc(64);
    // char version[16];
    sscanf(request_line, "%s %s", method, url);
    // printf("%s\n", method);
}
/*
Prints the request options and blank line.
*/
void print_optns()
{
    char* next_optn = strtok(NULL, "\n");
    while(next_optn != NULL)
    {
        // if (next_optn[0] == '\r')
        //     break;
        printf("%s\n", next_optn);
        next_optn = strtok(NULL, "\n");
    }
    // printf("\n");
}


void free_rqln(char* method, char* path)
{
    free(method);
    free(path);
}
//------------------------END OF REQUEST PARSING--------------------------//
//---------------------------REQUEST HANDLING----------------------------//
/*
Handles request in buffer
*/
int handle_request(char buffer[], SOCKET socket)
{
    char *method, *path;
    //Parse and print the request and options
    parse_request(buffer, method, path);
    char get[] = "GET";
    int result;
    if (strcmp(method, get) == 0) //GET request
    {
        result = handle_get(socket, path);
        free_rqln(method, path);
    }
    else //POST request
    {
        result = handle_post(socket);
    }
    return result;
}

int handle_get(SOCKET socket, char* path)
{
    printf("Opening %s...\n", path);
    FILE *fh = fopen(path, "r");
    if (fh == NULL) //File not found
    {
        printf("Failed to open file\n");
        char response[100];
        strcat(response, NOT_FOUND);
        strcat(response, BLANK_LINE);
        return send(socket, response, 100, 0);
    }
    else //File found
    {
        //implement later
    }
}

int handle_post(SOCKET socket)
{
    //implement later
}

//---------------------------END OF REQUEST HANDLING----------------------------//