#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUFFER_LENGTH 10000

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
int handle_post(SOCKET,char*);
int handle_request(char*, SOCKET);


void connection(void* args)
{
    ConnectionArgs* c_args = (ConnectionArgs*)args;
    free(args);
    c_args->last_request = clock();
    c_args->closed = 0;
    printf("Connection thread created\n");
    char recv_buf[BUFFER_LENGTH];

    //Waiting for a request from the client
    while(1)
    {
        memset(recv_buf, 0, sizeof(recv_buf));
        int recieved = recv(c_args->socket, recv_buf, BUFFER_LENGTH, 0);
        if (recieved >= 0)
        {
            
            printf("Bytes received: %d\n", recieved);
            if(recieved > 0)
            {
                printf("Recieved: %s\n", recv_buf);
                
                int sendRes = handle_request(recv_buf,c_args->socket);

                // int sendRes = send(c_args->socket, recv_buf, recieved, 0);
                // c_args->last_request = clock();
                // int send_result = handle_request(recv_buf, c_args->socket);

                if (sendRes == SOCKET_ERROR)
                {
                    printf("response failed with error: %d\n", WSAGetLastError());
                    closesocket(c_args->socket);
                    c_args->closed = 1;
                    return;
                }
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
// void parse_request(char* request, char* method, char* path)
// {
//     //Parsing the request line
//     char* request_line = strtok(request, "\n");
//     char* url;
//     //Extract method and url from request line
//     parse_rqln(request_line, method, url);

//     char get[] = "GET";
//     if (strcmp(method, get) == 0)
//     {
//         path = (char*) malloc(32);
//         sscanf(url, "/%s", path);
//     }
    

//     free(url);
//     //Parsing the request options
//     print_optns();
// }


// Parses a request line and outputs method and url.
// the output pointers are allocated by this function, so don't allocate them beforehand.
// void parse_rqln(char* request_line, char* method, char* url)
// {
//     method = (char*) malloc(5);
//     url = (char*) malloc(64);
//     // char version[16];
//     sscanf(request_line, "%s %s", method, url);
//     // printf("%s\n", method);
// }
/*
Prints the request options and blank line.
*/
// void print_optns()
// {
//     char* next_optn = strtok(NULL, "\n");
//     while(next_optn != NULL)
//     {
//         // if (next_optn[0] == '\r')
//         //     break;
//         printf("%s\n", next_optn);
//         next_optn = strtok(NULL, "\n");
//     }
//     // printf("\n");
// }


// void free_rqln(char* method, char* path)
// {
//     free(method);
//     free(path);
// }
//------------------------END OF REQUEST PARSING--------------------------//
//---------------------------REQUEST HANDLING----------------------------//
/*
Handles request in buffer
*/
int handle_request(char buffer[], SOCKET socket)
{
    char method[10];
    char path[100];
    sscanf(buffer, "%s %s",method,path);

    if(strcmp(method,(char*)"GET")==0){
        
        printf("GET\n");
        printf("%s\n",path);
        return handle_get(socket,path);
    }
    if(strcmp(method,(char*)"POST")==0){
        // post_request();
        printf("POST\n");
        return handle_post(socket,buffer);
    }
}

int handle_get(SOCKET socket, char* path)
{
    printf("Opening %s...\n", path);
    FILE *file = fopen(path, "r");
    if (file == NULL) //File not found
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
        // Determine the size of the file
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Allocate memory to store the entire content of the file
        char *file_content = (char *)malloc(file_size + 1); // +1 for null terminator

        // Check if memory allocation was successful
        if (file_content == NULL) {
            perror("Error allocating memory");
            fclose(file);
            char response[100];
            strcat(response, NOT_FOUND);
            strcat(response, BLANK_LINE);
            return send(socket, response, 100, 0);

        }

        // Read the entire content of the file into the buffer
        fread(file_content, 1, file_size, file);

        // Null-terminate the string
        file_content[file_size] = '\0';

        // send data
        // printf("%s",file_content);
        int result = send(socket, file_content, file_size, 0);

        // Clean up: close the file and free the allocated memory
        fclose(file);
        free(file_content);

        return result;

    }
}

int handle_post(SOCKET socket,char* buffer)
{
    //implement later
    printf("handling POST ...");
    char path[100],method[10];
    printf("buffer: %s",buffer);
    sscanf(buffer, "%s %s",method,path);
    printf("the path is: %s",path);


    // Find the start of the request body
    char *bodyStart = strstr(buffer, "\r\n\r\n");
    int result;
    if (bodyStart != NULL) {
        bodyStart += 4;  // Move past the empty line
        printf("buffer: %s",bodyStart);
        FILE* file = fopen(path, "wb");
        if (file == NULL) {
            result = send(socket,NOT_FOUND,sizeof(NOT_FOUND),0);
            perror("Error opening file");
            return result;
        }
        
        fwrite(bodyStart, 1, BUFFER_LENGTH, file);

        char response[100];
        strcat(response, OK);
        strcat(response, BLANK_LINE);

        result = send(socket,response,100,0);
        fclose(file);

        
        return result;

    }
    else{

        char response[100];
        strcat(response, NOT_FOUND);
        strcat(response, BLANK_LINE);
        
        result = send(socket,response,100,0);
        perror("no message content");

        return result;
    }
}

//---------------------------END OF REQUEST HANDLING----------------------------//