// README
// Instructions to use correctly the following program, you must follow this steps.
// Preliminary: if you want you can create a Makefile with the above code, or just skip this process
// and go to step 2).
// ##########################
// all: run

// run:
// 	g++ -std=c++11 server.cpp -o server.exe -lpthread
// 	g++ -std=c++11 client.cpp -o client.exe -lpthread
// ############################
// 1) on the directory comand line only type [$ make run] or [$ make], it will compile the two files
// 2) next you execute the server typing [$ ./server.exe], and it will wait for clients
// 3) them, you must open <<two or more terminals>> and type in each of them [$ ./client.exe]
// 4) a message will appear in each of them, saying something like: "primero ingresa tu nombre sin espacios"
// 5) you type your name without spaces and then you can send messages to other users in this way
//   - <name_of_the_receiver>:<message> (do not include the "<,>" symbols, it is only text)
//   - if the name of the user is not in the table a message will appear to you saying: "el usuario no existe"
//   - Note that the above message do not have spaces.
// 6) to exit from the chat just type "chau" in the client terminal, and the client program will stop.
// 7) Lastly you close the server window.

/* Server code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <sstream>

using namespace std;
// this is the maximun size that can be hold be hold by my buffer,
// following the guidelines on moodle
#define BUFF_SIZE 9999 + 999 + 99 + 5
map<string, int> tableUsers;
map<string, string> logMsg; // a list of messages that handle some errors

string itostr(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}
int strtoi(string n)
{
    stringstream ss;
    ss << n;
    int p;
    ss >> p;
    return p;
}
int find_char(string buff, char b)
{
    for (char c = 0; c < buff.size(); c++)
    {
        if (b == buff[c])
            return c;
    }
    return -1;
}
int find_char(char buff[BUFF_SIZE], char b)
{
    for (char c = 0; buff[c] != '\0'; c++)
    {
        if (b == buff[c])
            return c;
    }
    return -1;
}
void buildLogMessages() // function to load different predefined messages
{
    logMsg["not_finded"] = "User was not encountered";
    logMsg["nick_registered"] = "1another user has the same nickname, choose other nickname";
    logMsg["must_register"] = "Register your user first";
    logMsg["exit"] = "chau";
}

// with this function we validate the received message
// we return codes to take different actions
int validation(string msg)
{
    switch (msg[0])
    {
    case 'N':
        return 1;
        break;
    case 'M':
        return 2;
        break;
    case 'A':
        return 3;
        break;
    case 'L':
        return 4;
        break;
    case 'F':
        return 5;
        break;
    case 'Q':
        return 6;
        break;
    default:
        return -1;
        break;
    }
}

void receive_send(int SocketClient)
{
    string msg, nickname;
    int n;
    char buffer[BUFF_SIZE];
    bool registered = 0, exitChat = 0; // first the user will be not registered
    while (!registered && !exitChat)   // first the user must be registered
    {
        n = read(SocketClient, buffer, 1);
        if (n < 0)
            perror("ERROR reading from socket");
        buffer[n] = '\0';
        msg.assign(buffer);
        int code = validation(msg);
        if (code == 1) // now we can register our user
        {
            n = read(SocketClient, buffer, 2); // we read the size of the nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int nick_size = strtoi(msg); // 2 digits of a number

            n = read(SocketClient, buffer, nick_size);
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);     // we read the nickname
            if (tableUsers.empty()) // if the table of user is empty we add a new user
            {
                nickname = msg; // we register our nickname, for the case when we want to change it
                tableUsers[msg] = SocketClient;
                registered = 1;
            }
            else // we find if another user has the same nickname
            {
                // no other user has the same nickname
                if (tableUsers.find(msg) == tableUsers.end())
                {
                    nickname = msg; // we register our nickname, for the case when we want to change it
                    tableUsers[msg] = SocketClient;
                    registered = 1;
                }
                else // another user has the same nickname
                {
                    n = write(SocketClient, logMsg["nick_registered"].c_str(), logMsg["nick_registered"].size());
                    if (n < 0)
                        perror("ERROR writing to socket");
                }
            }
        }
        else if (code == 6) // the user quit the chat
        {
            exitChat = 1;
            break;
        }
        else // we write to the client that the user must be registered first
        {
            n = write(SocketClient, logMsg["must_register"].c_str(), logMsg["must_register"].size());
            if (n < 0)
                perror("ERROR writing to socket");
        }
    }
    while (!exitChat)
    {
        n = read(SocketClient, buffer, 1); // first we read only one byte, the code
        if (n < 0)
            perror("ERROR reading from socket");
        buffer[n] = '\0';
        msg.assign(buffer);
        switch (validation(msg))
        {
        case 2: // send a message to a user
        {
            string FinalMessage = "M"; // we create the first message
            if (nickname.size() < 10)
                FinalMessage += "0" + itostr(nickname.size());
            else
                FinalMessage += itostr(nickname.size());
            FinalMessage += nickname;

            n = read(SocketClient, buffer, 2); // we read the nickname size of the receiver
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);

            int size_nick = strtoi(msg);

            n = read(SocketClient, buffer, size_nick); // we read the new nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            string receiver_nick = msg;
            if (tableUsers.find(msg) != tableUsers.end()) // if the user was encountered on the table
            {

                n = read(SocketClient, buffer, 3); // we read the size of the message
                if (n < 0)
                    perror("ERROR reading from socket");
                buffer[n] = '\0';
                msg.assign(buffer);

                FinalMessage += msg; // we append the size of the message

                int size_message = strtoi(msg);
                n = read(SocketClient, buffer, size_message); // we read the message
                if (n < 0)
                    perror("ERROR reading from socket");
                buffer[n] = '\0';
                msg.assign(buffer);

                FinalMessage += msg; // we append the message
                n = write(tableUsers[receiver_nick], FinalMessage.c_str(), FinalMessage.size());
                if (n < 0)
                    perror("ERROR writing to socket");
            }
            else
            {
                n = write(SocketClient, logMsg["not_finded"].c_str(), logMsg["not_finded"].size());
                if (n < 0)
                    perror("ERROR writing to socket");
            }
            break;
        }
        case 3: // send a global message
        {
            string GlobalMessage = "A";
            int size_nick = nickname.size();
            if (size_nick < 10) // we add the size of our nickname
                GlobalMessage += "0" + itostr(size_nick);
            else
                GlobalMessage += itostr(size_nick);

            GlobalMessage += nickname;

            n = read(SocketClient, buffer, 3); // we read the size of the message
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            GlobalMessage += msg; // we add the size of the message
            int size_message = strtoi(msg);

            n = read(SocketClient, buffer, size_message); // we read the global message
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            GlobalMessage += msg; // we add the global message

            map<string, int>::iterator User;
            for (User = tableUsers.begin(); User != tableUsers.end(); ++User)
            {
                // we write the global message for each user
                n = write(tableUsers[User->first], GlobalMessage.c_str(), GlobalMessage.size());
                if (n < 0)
                    perror("ERROR writing to socket");
            }
            break;
        }
        case 4: // send the list of active users
        {
            string ListUsers = "L";
            if (tableUsers.size() < 10) // we add the number of active users
                ListUsers += "0" + itostr(tableUsers.size());
            else
                ListUsers += itostr(tableUsers.size());

            map<string, int>::iterator User;
            int size_nick = 0;
            for (User = tableUsers.begin(); User != tableUsers.end(); ++User) // we add the size of nickname of each user and his nickname
            {
                size_nick = User->first.size(); // the size_nick of the i-th user
                if (size_nick < 10)             // the nick_size must be of size 2
                    ListUsers += "0" + itostr(size_nick);
                else
                    ListUsers += itostr(size_nick);
                ListUsers += User->first; // we add the nickname of the i-th user
            }
            // we write the message
            n = write(SocketClient, ListUsers.c_str(), ListUsers.size());
            if (n < 0)
                perror("ERROR writing to socket");
            break;
        }
        case 5: // send a file
        {
            string FileMessage = "F";
            if (nickname.size() < 10) //we add the origin size nickname
                FileMessage += "0" + itostr(nickname.size());
            else
                FileMessage += itostr(nickname.size());

            FileMessage += nickname; // we add the origin nickname

            n = read(SocketClient, buffer, 2); // we read the size of the receiver nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_nick = strtoi(msg); // the size of the destination nickname

            n = read(SocketClient, buffer, size_nick); // we read the receiver nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);

            string receiver_nick = msg; // the nickname of the receiver

            n = read(SocketClient, buffer, 2); // we read the filename size
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_filename = strtoi(msg);

            // we add the size filename
            FileMessage += msg;

            n = read(SocketClient, buffer, size_filename); // we read the filename
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            FileMessage += msg; // we add the filename

            n = read(SocketClient, buffer, 4); // we read the size of the content file
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_content = strtoi(msg);
            FileMessage += msg; // we add the size of the content file

            n = read(SocketClient, buffer, size_content); // we read the content file
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            FileMessage += msg; // we add the content of the file
            // cout << FileMessage << endl;
            // we write the file message to the receiver
            n = write(tableUsers[receiver_nick], FileMessage.c_str(), FileMessage.size());
            if (n < 0)
                perror("ERROR writing to socket");
            break;
        }
        case 6: // quit the chat
            exitChat = 1;
            // we delete the user from the table of current active users
            tableUsers.erase(nickname);
            break;
        default: // we do not resend anything
            break;
        }
    }
    shutdown(SocketClient, SHUT_RDWR);
    close(SocketClient);
}

int main(void)
{
    buildLogMessages(); // to build all the log messages
    struct sockaddr_in stSockAddr;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (-1 == SocketFD)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(50001);
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if (-1 == bind(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == listen(SocketFD, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    /////////////////////////////////
    int n;
    char buffer[BUFF_SIZE];
    for (;;)
    {
        // the first message has to be your nickname and the nickname
        // of the another user that you want to chat separated by a comma
        int SocketClient = accept(SocketFD, NULL, NULL);
        if (SocketClient < 0)
        {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        thread(receive_send, SocketClient).detach();
    }

    close(SocketFD);
    return 0;
}
