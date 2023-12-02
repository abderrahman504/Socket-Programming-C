#include <stdio.h>
#include <winsock2.h>
#include <bits/stdc++.h>
#include <iostream>



using namespace std;



int main()
{

    cout << "---------- CLIENT RUNNING ---------" << endl;

    //local variables

    WSADATA winSockData;
    int iWsaStartUp;
    int iWsaCleanUp;

    SOCKET TCPServerSocket;
    SOCKET TCPClientSocket;
    int iCloseSocket;

    struct sockaddr_in TCPServerAdd;
    struct sockaddr_in TCPClientAdd;

    int iTCPClientAdd = sizeof(TCPClientAdd);

    int iBind;

    int iListen;

    int iConnect;
    SOCKET sAcceptSocket;

    int iSend;
    char SenderBuffer[512] = "Hello from the client!";
    int iSenderBuffer = strlen(SenderBuffer)+1;

    int iRecv;
    char RecvBuffer[512];
    int iRecvBuffer = strlen(RecvBuffer)+1;




    //STEP -1 WSAStartUp Fun
    iWsaStartUp = WSAStartup(MAKEWORD(2,2),&winSockData);
    if(iWsaStartUp != 0){
        cout << "WSA StartUp Failed!!" << endl;
    }
    cout << "WSA StartUp Succefully" << endl;

    //STEP -2 Socket Creation
    TCPClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(TCPClientSocket == INVALID_SOCKET){
        cout << "TCP Client Socket Failiedddd" << endl;
    }
    cout << "TCP Client Socket Successs" << endl;

    //STEP -3 Fil the Structure
    TCPServerAdd.sin_family = AF_INET;
    TCPServerAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
    TCPServerAdd.sin_port = htons(8000);

    //STEP -4 Connect
    iConnect = connect(TCPClientSocket,(SOCKADDR*)&TCPServerAdd,sizeof(TCPServerAdd));
    if(iConnect == SOCKET_ERROR){
        cout << "connect failed error: " << WSAGetLastError() << endl;
    }
    cout << "connect success" << endl;

    //STEP -5 Recv Data From client
    iRecv = recv(TCPClientSocket, RecvBuffer, iRecvBuffer,0);
    if(iRecv == SOCKET_ERROR){
        cout << "Revieve data failed error: " << WSAGetLastError() << endl;
    }
    cout << "Revieve data success" << endl;
    cout << "The Data Recieved => " << RecvBuffer << endl;


    //STEP -6 send data to client
    iSend = send(TCPClientSocket,SenderBuffer,iSenderBuffer,0);
    if(iSend == SOCKET_ERROR){
        cout << "Send data failed error: " << WSAGetLastError() << endl;
    }
    cout << "Send data success" << endl;

    

    //STEP -9 Close Socket
    iCloseSocket = closesocket(TCPClientSocket);
    if(iCloseSocket == SOCKET_ERROR){
        cout << "Close  failed error: " << WSAGetLastError() << endl;
    }
    cout << "Close  success" << endl;

    //STEP -10 CleanUp from DLL
    iWsaCleanUp = WSACleanup();
    if(iWsaCleanUp == SOCKET_ERROR){
        cout << "Clean failed error: " << WSAGetLastError() << endl;
    }
    cout << "Clean success" << endl;



    
    int x;
    cin >> x;
    return 0;
}
