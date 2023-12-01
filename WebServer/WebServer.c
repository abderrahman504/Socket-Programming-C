#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "Thread.c"


int main(char args[])
{
    printf("Web server running.\n");
    ConnectionArgs t_args = {80};
    _beginthread(connection, 0, &t_args);

    Sleep(100);
    printf("Web server shutting down.\n");
    return 0;
}