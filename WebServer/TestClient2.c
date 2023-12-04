#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "../Network.h"

#define BUFFER 1000000


int talk_to_server(SOCKET conn)
{
    char buf[BUFFER];
    int i=1;
    while(1)
    {
        char msg[100] = "GET /Test.html HTTP/1.1\r\n\r\n Hi From the Clieeentt\r\n";
        int send_result = send(conn, msg, strlen(msg), 0);
        if (send_result == SOCKET_ERROR)
        {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(conn);
            WSACleanup();
            return 1;
        }
        int recv_result = recv(conn, buf, BUFFER, 0);
        if (recv_result > 0)
            printf("Recieved: %s\n", buf);
        else if (recv_result == 0)
            printf("Recieved nothing\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());
        
        i++;
        Sleep(1000);
    }
}


int main(char args[])
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char servername[] = "localhost";
    iResult = getaddrinfo(servername, SERVER_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET conn_socket = INVALID_SOCKET;
    conn_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (conn_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //Connecting to server
    iResult = connect(conn_socket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR){
        closesocket(conn_socket);
        conn_socket = INVALID_SOCKET;
    }
    if (conn_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    talk_to_server(conn_socket);

    printf("Closing client\n");
    return 0;
}