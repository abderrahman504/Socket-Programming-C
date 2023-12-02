#include <stdio.h>
#include <winsock2.h>
#include <bits/stdc++.h>
#include <iostream>



using namespace std;



int main()
{

    cout << "---------- SERVER RUNNING ---------" << endl;

    //local variables

    WSADATA winSockData;
    int iWsaStartUp;
    int iWsaCleanUp;

    SOCKET TCPServerSocket;
    int iCloseSocket;

    struct sockaddr_in TCPServerAdd;
    struct sockaddr_in TCPClientAdd;

    int iTCPClientAdd = sizeof(TCPClientAdd);

    int iBind;

    int iListen;

    SOCKET sAcceptSocket;

    int iSend;
    char SenderBuffer[512] = "hello from the server!";
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

    //STEP -2 Fill the Structure
    TCPServerAdd.sin_family = AF_INET;
    TCPServerAdd.sin_addr.s_addr = inet_addr("127.0.0.1");
    TCPServerAdd.sin_port = htons(8000);

    //STEP -3 Create Socket
    TCPServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(TCPServerSocket == INVALID_SOCKET){
        cout << "TCP Server Socket Failiedddd" << endl;
    }
    cout << "TCP Server Socket Successs" << endl;

    //STEP -4 bind function
    iBind = bind(TCPServerSocket,(SOCKADDR*) & TCPServerAdd,(sizeof(TCPServerAdd)));
    if(iBind == SOCKET_ERROR){
        cout << "Binding Failiedddd" << endl;
    }
    cout << "Binding Successs" << endl;

    //STEP -5 Listen Fun
    iListen = listen(TCPServerSocket,200);
    if(iListen == SOCKET_ERROR){
        cout << "Listen faileeed" << endl;
    }
    cout << "Listen success" << endl;

    //STEP -6 Accept
    sAcceptSocket = accept(TCPServerSocket, (SOCKADDR*)&TCPClientAdd,&iTCPClientAdd);
    if(sAcceptSocket == INVALID_SOCKET){
        cout << "DECLINED" << endl;
    }
    cout << "ACCEPTED" << endl;

    //STEP -7 send data to client
    iSend = send(sAcceptSocket,SenderBuffer,iSenderBuffer,0);
    if(iSend == SOCKET_ERROR){
        cout << "Send data failed error: " << WSAGetLastError() << endl;
    }
    cout << "Send data success" << endl;

    //STEP -8 Recv Data From client
    iRecv = recv(sAcceptSocket, RecvBuffer, iRecvBuffer,0);
    if(iRecv == SOCKET_ERROR){
        cout << "Revieve data failed error: " << WSAGetLastError() << endl;
    }
    cout << "Revieve data success" << endl;
    cout << "The data from the client => " << RecvBuffer << endl;

    //STEP -9 Close Socket
    iCloseSocket = closesocket(TCPServerSocket);
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
