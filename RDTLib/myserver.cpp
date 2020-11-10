#include "server.h"

int main()
{
    int port = 50005;
    Server myserver(port);
    string msg, hola = "hola";
    while (1)
    {
        msg = myserver.receiveMessage();
        cout << "client: " << msg << endl;
        myserver.sendMessage("hola");
    }

    return 0;
}