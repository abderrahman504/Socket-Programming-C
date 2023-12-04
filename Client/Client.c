#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../Network.h"

#define DEFAULT_COMMANDS "commands.txt"
#define BUFFER 10000

void start(FILE*);
int run_command(char*, char*, char*, char*);
int connect_to_server(char*, char*, SOCKET*);
int handle_get(SOCKET, char*, char*, char*);
int handle_post(SOCKET, char*, char*, char*);


/*
This function runs the commands in file one by one.
It creates a socket with the specified server name and port and sends the specified request.
After processing the response it shuts the socket down and repeats until end of file.
*/
void start(FILE* file)
{
    char recv_buf[BUFFER];
    char line[100];
    char DEFAULT_PORT[] = "80";
    int error_count = 0;
    printf("Reading from commands file...\n");
    char* check = fgets(line, 100, file);
    while (check != NULL) //While more commands exist.
    {
        printf("Next command...\n");
        //Parsing line from commands file
        char* token = strtok(line, " ");
        char method[5];
        if (strcmp(token, "client_get") == 0) strcpy(method, "GET");
        else strcpy(method, "POST");
        char *file_path = strtok(NULL, " ");
        char *servername = strtok(NULL, " ");
        char *port = strtok(NULL, "\n");
        if (port == NULL) port = DEFAULT_PORT;
       
        //Performing command
        error_count += run_command(method, file_path, servername, port);
        //Fetch next command
        check = fgets(line, 100, file);
    }
    printf("Command file finished with %d errors.\n", error_count);
}


/*
Runs the given command.
Creates a socket to the specified server and sends the request.
Returns 1 if there was an error with doing the command, and 0 otherwise.
*/
int run_command(char* method, char* file_path, char* servername, char* port)
{
    SOCKET conn;
    int sock_result = connect_to_server(servername, port, &conn);
    if (sock_result == 1) return 1;
    if (strcmp(method, "GET") == 0) //GET request
    {
        handle_get(conn, file_path, servername, port);
    }
    else //POST request
    {
        handle_post(conn, file_path, servername, port);
    }
    shutdown(conn, SD_SEND);
    closesocket(conn);
    return 0;
}


int handle_get(SOCKET conn, char* path, char* servername, char* port)
{
    //Construct request
    char request[128] = "";
    strcat(request, "GET ");
    strcat(request, "/");
    strcat(request, path);
    strcat(request, " HTTP/1.1\r\n\r\n");
    //Send request
    printf("Sending GET to server...\n");
    int res = send(conn, request, 128, 0);
    if (res == SOCKET_ERROR){
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(conn);
        return 1;
    }
    //Receive response
    char response[BUFFER];
    printf("Recieving response...\n");
    res  = recv(conn, response, BUFFER, 0);
    if (res == 0){
        printf("Received nothing\n");
        return 1;
    } else if (res < 0){
        printf("rcv failed with error %d\n",  WSAGetLastError());
        return 1;
    }
    //print response
    printf(response);
    //read status
    strtok(response, " ");
    char* status = strtok(NULL, " ");
    if (strcmp(status, "200") == 0) //OK
    {
        printf("\n"); //Print newline after requested file body
        //read blank line
        strtok(NULL, "\n");
        char *token = strtok(NULL, "\n");
        char* body = token + (strlen(token)+1);
        //read file and store it
        FILE* dest_file = fopen(path, "w");
        if (dest_file == NULL){
            printf("Couldn't store file after GET\n");
            return 1;
        }
        fprintf(dest_file, body);
        fclose(dest_file);
        printf("File stored\n");
    }
    return 0;
}

int handle_post(SOCKET conn, char* path, char* servername, char* port)
{
    char request[10000];
    strcat(request, "POST ");
    strcat(request, "/");
    strcat(request, path);
    strcat(request, " HTTP/1.1\r\n\r\n");
    //Read file
    char body[10000] = "";
    FILE* f = fopen(path, "r");
    if (f==NULL){
        printf("Failed to open file to be posted\n");
        return 1;
    }
    
    char line[500];
    while(fgets(line, 500, f) != NULL){
        strcat(body, line);
    }
    
    //Append file to request
    strcat(request, body);
    //Send request.
    printf("Sending POST to server...\n");
    int res = send(conn, request, 10000, 0);
    if (res == SOCKET_ERROR){
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(conn);
        return 1;
    }
    //Receive response
    char response[BUFFER];
    printf("Recieving response...\n");
    res  = recv(conn, response, BUFFER, 0);
    if (res == 0){
        printf("Received nothing\n");
        return 1;
    } else if (res < 0){
        printf("rcv failed with error %d\n",  WSAGetLastError());
        return 1;
    }
    //print response
    printf(response);
    return 0;
}


/*
Creates and connects a socket to the specified server and port.
conn is an output pointer that contains the created socket.
Returns 0 if connected succesfully, and 1 otherwise.
*/
int connect_to_server(char* servername, char* port, SOCKET* conn_ptr)
{
    int iResult;
    //Prep socket creation
    struct addrinfo *result = NULL, hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(servername, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        return 1;
    }
    //Create socket
    *conn_ptr = INVALID_SOCKET;
    *conn_ptr = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (*conn_ptr == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        return 1;
    }

    //Connecting to server
    iResult = connect(*conn_ptr, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR){
        closesocket(*conn_ptr);
        *conn_ptr = INVALID_SOCKET;
    }
    if (*conn_ptr == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        return 1;
    }
    return 0;
}

/*
Running from command line:
./Client.exe
./Client.exe [server_ip] [server_port]
./Client.exe [server_ip] [server_port] [commands_file]
*/
int main(int argc, char *argv[])
{
    char server_name[16], server_port[8], command_file[32];
    //Checking arguments for server name, port, commands file.
    if (argc == 1){
        printf("Client running with no arguments-default server and port and commands.txt as the commands file...\n");
        strcpy(server_name, SERVER_NAME);
        strcpy(server_port, SERVER_PORT);
        strcpy(command_file, DEFAULT_COMMANDS);
    }
    else if (argc == 3){
        printf("Client running with 2 arguments-server name and port, with commands.txt as commands file...\n");
        strcpy(server_name, argv[1]);
        strcpy(server_port, argv[2]);
        strcpy(command_file, DEFAULT_COMMANDS);
    }
    else if (argc == 4){
        printf("Client running with 3 arguments-server name, port, and commands file...\n");
        strcpy(server_name, argv[1]);
        strcpy(server_port, argv[2]);
        strcpy(command_file, argv[3]);
    }
    else{
        printf("Invalid number of arguments (%d). Shutting client down\n", argc);
        return 1;
    }
    //Trying to open commands file
    FILE* file = fopen("commands.txt", "r");
    if (file == NULL){
        printf("Couldn't open commands file %s. Make sure this file exists\n. Closing client.\n", command_file);
        return 1;
    }
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    printf("Client connected to server\n");
    start(file);

    printf("Client shutting down.\n");
    return 0;
}
