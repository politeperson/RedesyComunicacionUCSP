#include "server.h"

int main()
{
    int port = 50005;
    Server *myserver = new Server(port);
    thread(&Server::Listen, myserver).detach();
    while (1)
    {
    }
    delete myserver;
    return 0;
}