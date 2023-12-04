#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../Network.h"


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
        printf("connected%d\n", i);
        Sleep(100);
    }
    Sleep(100000);
}