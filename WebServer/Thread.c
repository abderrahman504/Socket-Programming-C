

typedef struct {
    long long time_since_last_request;
    int id;
    short port;
}Connection;

typedef struct {
    int port;
}ConnectionArgs;


void connection(void* args)
{
    ConnectionArgs* c_args = args;
    printf("Connection started with port %d\n", c_args->port);


    printf("Connection closing\n");
}