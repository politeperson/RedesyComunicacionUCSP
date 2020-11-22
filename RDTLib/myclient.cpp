#include "client.h"
#include "message.h"
int main()
{
    int port = 50005;
    string ipAddress = "127.0.0.1";
    Client *myclient = new Client(ipAddress, port);
    string msg = "Julio";
    thread(&Client::Listen, myclient).detach();
    string sendForget = myclient->SendAndForget(msg);
    cout << sendForget << endl;
    delete myclient;
    // string msg = "JULIOPROFEGAAA";
    // Message mymessage;
    // string request = mymessage.encode(msg, '5', 5);
    // cout << request << endl;
    // mymessage.decode(request);
    // cout << "type: " << mymessage.getType() << endl;
    // cout << "id: " << mymessage.getId() << endl;
    // cout << "message: " << mymessage.getContent() << endl;
    // cout << "hash: " << mymessage.getHash() << endl;

    return 0;
}