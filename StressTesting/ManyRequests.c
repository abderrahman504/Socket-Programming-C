#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#include "../Network.h"

#define GET "GET /get.html HTTP/1.1\r\nHost: localhost\r\n\r\n"

struct Args{
    SOCKET conn;
    int id;
};

void conn_thread(void* conn);

int main()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    SOCKET conn = INVALID_SOCKET;
    char servername[16] = "localhost", port[8] = "80";
    //Prep socket creation
    struct addrinfo hints;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    for (int i=0; i<200; i++)
    {
        struct addrinfo *result = NULL;
        iResult = getaddrinfo(servername, port, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            continue;
        }
        //Create socket
        conn = INVALID_SOCKET;
        conn = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (conn == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            continue;
        }
        //Connecting to server
        iResult = connect(conn, result->ai_addr, (int)result->ai_addrlen);
        freeaddrinfo(result);
        if (iResult == SOCKET_ERROR){
            // closesocket(*conn_ptr);
            conn = INVALID_SOCKET;
        }
        if (conn == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            continue;
        }
        // printf("connected%d\n", i);
        struct Args *args = malloc(sizeof(struct Args));
        args->conn = conn;
        args->id = i; 
        _beginthread(conn_thread, 0, (void*)args);
        Sleep(100);
    }
    Sleep(100000);
}



void conn_thread(void* conn)
{
    struct Args *args = (struct Args*)conn;
    char buffer[1024];
    int iResult;
    for (int i=0; i<200; i++)
    {
        printf("no.%d sending GET\n", args->id);
        iResult = send(args->conn, GET, strlen(GET), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            closesocket(args->conn);
            WSACleanup();
            return;
        }
        recv(args->conn, buffer, 1024, 0);
        Sleep(50);
    }
}