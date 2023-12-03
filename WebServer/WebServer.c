#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "Connection.c"
#include "../Network.h"


WSADATA wsaData;

void listening(SOCKET listen_socket)
{
    //Listening on the socket
    if ( listen( listen_socket, SOMAXCONN ) == SOCKET_ERROR ) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError() );
        closesocket(listen_socket);
        WSACleanup();
        return;
    }

    while(1)
    {

        struct sockaddr_in clntAddr;
        int clntAddrLen = sizeof(clntAddr);

        //Accepting a client
        SOCKET client_socket = INVALID_SOCKET;
        client_socket = accept(listen_socket, (struct sockaddr*)&clntAddr,  &clntAddrLen);
        if (client_socket == INVALID_SOCKET) {
            printf("infinite loop");
            continue;
        }


        // Get the IP Address of the client
        char clntName[NI_MAXHOST]; // String to contain client address

        if (getnameinfo((struct sockaddr*)&clntAddr, clntAddrLen, clntName, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) {
            printf("Handling client %s\n", clntName);
        } else {
            perror("Unable to get client address");
        }

        
        ConnectionArgs* args = (ConnectionArgs*) malloc(sizeof(ConnectionArgs));
        args->socket = client_socket;
        Connection* conn = (Connection*) malloc(sizeof(Connection));
        conn->th_args = args;
        printf("Creating thread...\n");
        conn->thread = (HANDLE) _beginthread(connection, 0, (void*)args);
    }
}




int main(char args[])
{
    printf("Starting server...\n");
    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
    printf("WSAStartup failed: %d\n", iResult);
    return 1;
    }

    //Preparing to create a socket
    struct addrinfo *result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, SERVER_PORT, &hints, &result);
    if (iResult != 0) {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
    return 1;
    }

    //Creating a listening socket
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //Binding the socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(result);

    printf("Server started\n");
    listening(ListenSocket);
    printf("Server closing\n");
    return 0;
}