// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <math.h>
#include <sstream>
using namespace std;

#define PORT 50005
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;
bool exitChat = 0;
bool registered = 0;
string nickname; // a global nickname of the client

// return a string with a number
// with certain kind of digits
string numDigits(int n, int digits)
{
    string zeros(digits - (int)floor(log10(n) + 1), '0');
    stringstream ss;
    ss << digits;
    return zeros + ss.str();
}

// the message comes with comas separated, we return a list of strings
vector<string> Parse(string msg)
{
    vector<string> messages;
    string aux = "";
    for (int i = 0; i < msg.size(); ++i)
    {
        if (msg[i] == ',')
        {
            messages.push_back(aux);
            aux = "";
        }
        else
        {
            aux += msg[i];
        }
    }
    if (!aux.empty())
    {
        messages.push_back(aux);
    }
    return messages;
}

int receiveMsg()
{
    int n;
    string msg;
    char buffer[MAXLINE];
    unsigned int len = sizeof(servaddr);
    while (!exitChat)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        msg.assign(buffer);
        cout << msg << endl;
        /*if (msg == "not_registered")
        {
            registered = 0;
            nickname = "";
            cout << "Error on registration, choose another nickname\n";
        }
        else if (msg == "user_not_found")
        {
            cout << "Error on sending message, user was not found\n";
        }
        else
        {
            switch (msg[0])
            {
            case 'M':
            {
                vector<string> messages = Parse(msg);
                string originNick = messages[1];
                string message = messages[2];
                cout << "Message from: " << originNick << endl;
                cout << "Message: " << message << endl;
                break;
            }
            case 'A': //global message
            {
                vector<string> messages = Parse(msg);
                string originNick = messages[1];
                string message = messages[2];
                cout << "Global message from: " << originNick << endl;
                cout << "Message: " << message << endl;
                break;
            }
            case 'L':
            {
                vector<string> users = Parse(msg);
                cout << "List of active Users: \n";
                for (int i = 1; i < users.size(); ++i)
                {
                    cout << "User [" << i << "]: " << users[i] << endl;
                }
                break;
            }
            case 'F':
            {
                vector<string> newFile = Parse(msg);
                string originNick = newFile[1];
                string fileName = newFile[2];
                string contentFile = newFile[3];
                // we create a new file
                ofstream receivedFile(fileName);
                receivedFile << contentFile; // we write the received content into it
                receivedFile.close();
                ////////////////////////////////////////
                cout << "You received a new file from: " << originNick << endl;
                cout << "Name of the file: " << fileName << endl;
                cout << "Check your directory" << endl;
                break;
            }
            default:
                break;
            }
        }*/
    }
    return 0;
}

void sendMsg()
{
    int n;
    string msg;
    char buffer[MAXLINE];
    unsigned int len = sizeof(servaddr);
    while (!exitChat)
    {
        cin >> msg;
        sendto(sockfd, msg.c_str(), MAXLINE,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        /*switch (msg[0])
        {
        case 'N':
        {
            if (registered)
                cout << "cannot change your nickname\n";
            else
            {
                string nick = "N";
                cout << "nickname: ";
                do
                {
                    cin >> msg;            // we enter the client nickname
                } while (msg.size() > 10); // only 10 bytes of size
                nickname = msg;
                nick += msg;
                cout << nick << endl;
                sendto(sockfd, nick.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                       sizeof(servaddr));
                registered = 1;
            }
            break;
        }
        case 'M':
        {
            if (!registered)
            {
                cout << "first register your nickname\n";
                break;
            }
            string messageSend = "M" + nickname + ",";
            cout << "Enter destination username: ";
            cin >> msg;
            messageSend += msg + ',';
            cout << "Enter message: ";
            cin >> msg;
            messageSend += msg;
            sendto(sockfd, messageSend.c_str(), MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            break;
        }
        case 'A': // a global message
        {
            if (!registered)
            {
                cout << "first register your nickname\n";
                break;
            }
            string messageSend = "A," + nickname + ",";
            cout << "write the global message: ";
            cin >> msg;
            messageSend += msg;
            sendto(sockfd, messageSend.c_str(), MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            break;
        }
        case 'L': //list al users
        {
            if (!registered)
            {
                cout << "first register your nickname\n";
                break;
            }
            sendto(sockfd, "L", MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            break;
        }
        case 'F':
        {
            if (!registered)
            {
                cout << "first register your nickname\n";
                break;
            }
            string messageSend = "F," + nickname + ","; //we add the origin nickname
            cout << "Write nickname of the destination user: ";
            cin >> msg;
            messageSend += msg + ",";
            cout << "Write the name of the file: ";
            cin >> msg;
            ifstream f(msg);
            if (f.good()) // if the file exists
            {
                messageSend += msg + ","; //we add to the message the name of the file
                // now we will read the file
                string line, content = "";
                int size_file = 0;
                while (getline(f, line)) // we read the file line by line
                {
                    if (size_file + line.size() > 1000)
                    { // the macimun size of a file content
                        content += line.substr(0, 1000 - size_file);
                        size_file = 1000;
                    }
                    else
                    {
                        content += line + "\n"; // a line ends with an end line
                        size_file += line.size() + 1;
                    }
                }
                messageSend += content; //we add to the message the content of the file
                //we send the message
                sendto(sockfd, messageSend.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                       sizeof(servaddr));
            }
            else
            {
                cout << "The file does not exists on your directory" << endl;
            }
            break;
        }
            
                case 'Q':
            {
                string quit = "Q" + nickname;
                sendto(sockfd, quit.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                       sizeof(servaddr));
                exit(0); // quit the system
            }
        default:
            cout << "Escoge una opción válida\n";
            break;
        }*/
    }

    return;
}

// Driver code
int main()
{
    char buffer[MAXLINE];
    struct hostent *host;

    host = (struct hostent *)gethostbyname((char *)"127.0.0.1");

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr = *((struct in_addr *)host->h_addr);

    /////////////////////////
    // first message of the protocol
    /////////////////////////

    // int n;
    // unsigned int len;
    cout << "ingresa una acción: " << endl;
    thread(sendMsg).detach();
    thread(receiveMsg).detach();

    while (1)
    {
    }
    close(sockfd);
    return 0;
}
