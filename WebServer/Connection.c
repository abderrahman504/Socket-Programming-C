#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    long long time_since_last_request;
    int id;
    short port;
}Connection;

typedef struct {
    SOCKET port;
}ConnectionArgs;


void connection(void* args)
{
    ConnectionArgs* c_args = args;
    printf("Connection started with port %d\n", c_args->port);


    printf("Connection closing\n");
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