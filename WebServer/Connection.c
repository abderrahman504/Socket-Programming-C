#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define BUFFER_LENGTH 1000

typedef struct {
    ConnectionArgs* th_args;
    HANDLE thread;
}Connection;

typedef struct {
    SOCKET* socket;
    clock_t last_request;
}ConnectionArgs;


void connection(void* args)
{
    ConnectionArgs* c_args = args;
    c_args->last_request = clock();
    printf("Connection established with a client...\n");
    char recv_buf[BUFFER_LENGTH];

    //Waiting for data from client
    while(1)
    {
        int recieved = recv(c_args->socket, recv_buf, BUFFER_LENGTH, 0);
        if (recieved >= 0)
        {
            printf("Bytes received: %d\n", recieved);

            int iSendResult = send(c_args->socket, recv_buf, recieved, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("echo failed with error: %d\n", WSAGetLastError());
                closesocket(c_args->socket);
                return;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(c_args->socket);
            return;
        }
    }
}



//Parses a HTTP request sent by a client.
//Returns a string containing a requested file, or NULL incase of a POST request
char* parse_request(char* request)
{
    //Parsing the request line
    char* request_line = strtok(request, "\n");
    char *method, *url, *version;
    parse_rqln(request_line, method, url, version);
    
    //Parsing the request options
    parse_optns(request);
}


// Parses a request line and outputs method, url, and http version.
// the output pointers are allocated by this function, so don't allocate them beforehand.
void parse_rqln(char* request_line, char* method, char* url, char* version)
{
    method = (char*) malloc(5);
    url = (char*) malloc(64);
    version = (char*) malloc(16);
    sscanf(request_line, "%s %s %s", method, url, version);
    printf("%s\n", method);
}


void parse_optns(char* request)
{
    char* next_optn = strtok(NULL, "\n");
    while(next_optn != NULL)
    {
        if (next_optn[0] == '\r')
            break;
        printf("%s\n", next_optn);
        next_optn = strtok(NULL, "\n");
    }
    printf("\n");
}


void free_rqln(char* method, char* url, char* version)
{
    free(method);
    free(url);
    free(version);
}