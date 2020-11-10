#include "client.h"
#include "message.h"
int main()
{
    // int port = 50005;
    // string ipAddress = "127.0.0.1";
    // Client myclient(ipAddress, port);
    // string msg;
    // while (1)
    // {
    //     cout << "ingresa mensaje: ";
    //     cin >> msg;
    //     myclient.sendMessage(msg);
    //     msg = myclient.receiveMessage();
    //     cout << msg << endl;
    // }

    string msg = "JULIOPROFEGAAA";
    Message mymessage;
    string request = mymessage.encode(msg, Solicitation);
    TypeOfMessage mytype;
    int myid;
    string decoded = mymessage.decode(request, mytype, myid);
    if (decoded != "error")
    {
        cout << "type: " << mytype << endl;
        cout << "id: " << myid << endl;
        cout << "message: " << decoded << endl;
    }

    return 0;
}