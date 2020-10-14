// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <map>
#include <thread>
#include <string>
#include <vector>
#include <utility>
using namespace std;

#define PORT 50005
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr, cliaddr;
// nickname -> <ipaddr, port>
map<string, pair<string, int>> tableUsers;
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

int receiveMsg(string msg, string ipaddr, int port)
{
    int n;
    char buffer[MAXLINE];

    cout << "Message: " << msg << endl;
    cout << "IPaddr: " << ipaddr << endl;
    cout << "Port: " << port << endl;
    cliaddr.sin_port = port;
    cout << cliaddr.sin_port << endl;
    sendto(sockfd, "hello from server", MAXLINE,
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           sizeof(cliaddr));

    /*   while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        buffer[n] = '\0';
        msg.assign(buffer);
        cout << msg << endl;
        switch (msg[0])
        {
        case 'N':
        {
            cout << "nickanme\n";
            vector<string> messages = Parse(msg);
            string nickname = messages[1]; // the new nickname
            // if the nickname was not registered
            if (tableUsers.empty())
            {
                tableUsers[nickname] = cliaddr.sin_port; // setting the nickname and the port
                cout << "newnick: " << nickname << " port: " << cliaddr.sin_port << endl;
            }
            else
            {
                if (tableUsers.find(nickname) == tableUsers.end())
                {
                    tableUsers[nickname] = cliaddr.sin_port; // setting the nickname and the port
                    cout << "newnick: " << nickname << " port: " << cliaddr.sin_port << endl;
                }
                else // we send the error message for registration
                {
                    string errorRegistration = "not_registered";
                    sendto(sockfd, errorRegistration.c_str(), MAXLINE,
                           MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                           len);
                }
            }
            break;
        }
        case 'M':
        {
            vector<string> messages = Parse(msg);
            string originNick = messages[1];
            string destNick = messages[2];
            string sendMessage = messages[3];
            // if the user was encountered
            if (tableUsers.find(destNick) != tableUsers.end())
            {
                // we set the port of destination
                cliaddr.sin_port = tableUsers[destNick];
                string myMessage = "M," + originNick + "," + sendMessage;
                sendto(sockfd, myMessage.c_str(), MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            else
            {
                sendto(sockfd, "user_not_found", MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            break;
        }
        case 'A':
        {
            vector<string> messages = Parse(msg);
            string originNick = messages[1];
            string sendMessage = messages[2];
            string globalMessage = "A," + originNick + "," + sendMessage;
            // we send a message to all users
            for (map<string, int>::iterator it = tableUsers.begin(); it != tableUsers.end(); ++it)
            {
                cliaddr.sin_port = it->second;
                sendto(sockfd, globalMessage.c_str(), MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            break;
        }
        case 'L':
        {
            string users = "L,";
            // we add a list of users
            for (map<string, int>::iterator it = tableUsers.begin(); it != tableUsers.end(); ++it)
            {
                users += it->first + ",";
            }
            sendto(sockfd, users.c_str(), MAXLINE,
                   MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                   len);
            break;
        }
        case 'F':
        {
            vector<string> newFile = Parse(msg);
            string originNick = newFile[1];
            string destNick = newFile[2];
            string fileName = newFile[3];
            string contentFile = newFile[4];
            if (tableUsers.find(destNick) != tableUsers.end())
            {
                string messageFile = "F," + originNick + "," + fileName + "," + contentFile;
                cliaddr.sin_port = tableUsers[destNick]; //we set the port of the user
                sendto(sockfd, messageFile.c_str(), MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            else
            {
                sendto(sockfd, "user_not_found", MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            break;
        }
        case 'Q':
        {
            vector<string> messages = Parse(msg);
            string nickname = messages[1]; // the user nickname
            cout << "quit message " << nickname << "\n";
            tableUsers.erase(nickname);
            break;
        }
        default:
            break;
        }
    }
*/
    return 0;
}

// Driver code
int main()
{

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int n;

    // first the server waits for the nickname of the user
    char buffer[MAXLINE];
    string msg;
    unsigned int len = sizeof(cliaddr);
    while (1)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
        buffer[n] = '\0';
        msg.assign(buffer);
        string address = string(inet_ntoa(cliaddr.sin_addr));
        // int port = ntohs(cliaddr.sin_port);
        thread(receiveMsg, msg, address, cliaddr.sin_port).detach();
    }
    tableUsers.clear();
    return 0;
}
