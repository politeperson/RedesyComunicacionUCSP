/* Client code in C */

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
#include <sstream>
#include <fstream> // to read the files
#define BUFF_SIZE 9999 + 999 + 99 + 5
using namespace std;
bool exitChat = 0;
string nickname;
int registered = 0;

int countDigits(int n)
{
    int ans = 0;
    while (n > 0)
    {
        ans++;
        n /= 10;
    }
    return ans;
}
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
int find_char(string msg, char b)
{
    for (char c = 0; c < msg.size(); c++)
        if (b == msg[c])
            return c;
    return -1;
}

// my protocol: MP
namespace MP
{
    string process_nickname(string msg)
    {
        string ans = "";
        ans += msg[0]; //N
        int size_nickname = msg.size() - 2;
        if (size_nickname < 10)
            ans += "0" + itostr(size_nickname);
        else
            ans += itostr(size_nickname);
        nickname = msg.substr(2);
        ans += msg.substr(2);
        return ans;
    }
    string process_message(string msg)
    {
        string ans = "";
        ans += msg[0]; //M
        int separatorColon = find_char(msg, ':');
        int size_nickname = separatorColon - 1;
        if (size_nickname < 10)
            ans += "0" + itostr(size_nickname);
        else
            ans += itostr(size_nickname);
        ans += msg.substr(1, size_nickname); // adding the destination nickname

        int size_message = msg.size() - size_nickname + 1;
        if (countDigits(size_message) == 1)
            ans += "00" + itostr(size_message);
        else if (countDigits(size_message) == 2)
            ans += "0" + itostr(size_message);
        else
            ans += itostr(size_message);
        ans += msg.substr(separatorColon + 1);
        return ans;
    }
    string process_global_message(string msg)
    {
        string ans = "A";
        // the size minus two, because th esyntax is "M:<message"
        int size_message = msg.size() - 2;
        if (countDigits(size_message) == 1)
            ans += "00" + itostr(size_message);
        else if (countDigits(size_message) == 2)
            ans += "0" + itostr(size_message);
        else
            ans += itostr(size_message);
        ans += msg.substr(2); //weadd the global message
        return ans;
    }
    string process_file(string msg)
    {
        string ans = "";
        // first we validate if the name of the file exists
        int separatorColon = find_char(msg, ':');
        string filename = msg.substr(separatorColon + 1).c_str();
        ifstream f(filename);
        if (f.good()) // we return the content of a file until the mximun capacity that is 9999
        {             // if the file exist on the directory we can send its content
            ans += "F";
            int size_nick = separatorColon - 1; // size of the destination nickname
            if (size_nick < 10)                 // we add the size of the destination nickname
                ans += "0" + itostr(size_nick);
            else
                ans += itostr(size_nick);
            ans += msg.substr(1, size_nick);     // we add the destination nickname
            int size_filename = filename.size(); // the size of the filename
            if (size_filename < 10)              // we add the size of the filename
                ans += "0" + itostr(size_filename);
            else
                ans += itostr(size_filename);

            ans += filename; //we add the filename

            // now we will read the size of the file
            string line, content = "";
            int size_file = 0;       // the size of the file
            while (getline(f, line)) // we read the file line by line
            {
                if (size_file + line.size() > 9999)
                { // the macimun size of a file content
                    content += line.substr(0, 9999 - size_file);
                    size_file = 9999;
                }
                else
                {
                    content += line + "\n"; // a line ends with a end line
                    size_file += line.size() + 1;
                }
            }
            // we add the size of the file content
            if (countDigits(size_file) == 1)
                ans += "000" + itostr(size_file);
            else if (countDigits(size_file) == 2)
                ans += "00" + itostr(size_file);
            else if (countDigits(size_file) == 3)
                ans += "0" + itostr(size_file);
            else
                ans += itostr(size_file);

            ans += content; // we add the content of a file
            return ans;
        }
        else
        { // the file does not exist on the directory
            return "The file does not exist on your directory";
        }
    }
} // namespace MP
int validation(string msg) // we check if the syntax is correct
{
    switch (msg[0])
    {
    case 'N':
        if (msg[1] != ':')
            return -1;
        return 1;
        break;
    case 'M':
        // if we can not encounter a ':', or we can not find a destination name
        if (msg.find(':') == -1 || msg.find(':') < 2)
            return -1;
        return 2;
        break;
    case 'A':
        if (msg[1] != ':')
            return -1;
        return 3;
        break;
    case 'L':
        return 4;
        break;
    case 'F':
        // we check if the user does not provide a nickname, or a colon
        // or does not provide the name of a file
        if (msg.find(':') == -1 || msg.find(':') < 2 || find_char(msg, ':') == msg.size() - 1)
            return -1;
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

void reading(int SocketFD)
{
    int n;
    char buffer[BUFF_SIZE];
    string msg;
    do
    {
        // Reading
        n = read(SocketFD, buffer, 1);
        buffer[n] = '\0';
        msg.assign(buffer);
        switch (msg[0])
        {
        case '1': // the code for a user that was already registered
            registered = 0;
            break;
        case 'M': // we read a message from another user
        {
            string FinalMessage = "";
            n = read(SocketFD, buffer, 2); //we read the size of the origin nickname
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_origin = strtoi(msg);

            n = read(SocketFD, buffer, size_origin); //we read the origin nickname
            buffer[n] = '\0';
            msg.assign(buffer);
            FinalMessage += msg + ":"; // adding to the message the nickname of origin and a colon

            n = read(SocketFD, buffer, 3); //we read the size of received message
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_message = strtoi(msg);

            n = read(SocketFD, buffer, size_message); //we read the received message
            buffer[n] = '\0';
            msg.assign(buffer);
            FinalMessage += buffer;
            cout << FinalMessage << endl;
            break;
        }
        case 'A': // we read a global message
        {
            string FinalMessage = "";
            n = read(SocketFD, buffer, 2); //we read the size of the origin nickname
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_origin = strtoi(msg);

            n = read(SocketFD, buffer, size_origin); //we read the origin nickname
            buffer[n] = '\0';
            msg.assign(buffer);
            FinalMessage += msg + ":"; // adding to the message the origin nickname and a colon

            n = read(SocketFD, buffer, 3); //we read the size of received message
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_message = strtoi(msg);

            n = read(SocketFD, buffer, size_message); //we read the received message
            buffer[n] = '\0';
            msg.assign(buffer);
            FinalMessage += buffer;
            cout << FinalMessage << endl;
            break;
        }
        case 'L': // we read a list of users
        {
            string ListUsers = "List of active users "; //our list of users
            n = read(SocketFD, buffer, 2);              //we read the number of users
            buffer[n] = '\0';
            msg.assign(buffer);
            int numberUsers = strtoi(msg);
            ListUsers += "[" + itostr(numberUsers) + "]:\n";
            int nick_size = 0;
            for (int i = 0; i < numberUsers; ++i) // we will read all the nicknames
            {
                n = read(SocketFD, buffer, 2); //we read the nickname size of i-th user
                buffer[n] = '\0';
                msg.assign(buffer);
                nick_size = strtoi(msg);

                n = read(SocketFD, buffer, nick_size); //we read the nickname of i-th user
                buffer[n] = '\0';
                msg.assign(buffer);
                ListUsers += msg + "\n";
            }
            cout << ListUsers << endl;
            break;
        }
        case 'F': // we read the content of  a file
        {
            string FileMessage = "";

            n = read(SocketFD, buffer, 2); // we read the size of the origin nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_originnick = strtoi(msg); // the size of the origin nickname

            n = read(SocketFD, buffer, size_originnick); // we read the origin nickname
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);

            FileMessage += msg; // we add the origin nickname

            n = read(SocketFD, buffer, 2); // we read the filename size
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_filename = strtoi(msg);

            n = read(SocketFD, buffer, size_filename); // we read the filename
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            FileMessage += " [" + msg + "]:\n"; // we add the filename

            n = read(SocketFD, buffer, 4); // we read the size of the content file
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            int size_content = strtoi(msg);

            n = read(SocketFD, buffer, size_content); // we read the content file
            if (n < 0)
                perror("ERROR reading from socket");
            buffer[n] = '\0';
            msg.assign(buffer);
            FileMessage += msg; // we add the content of the file
            cout << FileMessage << endl;
            break;
        }
        default:
        {
            cout << msg;
            // Reading
            n = read(SocketFD, buffer, BUFF_SIZE);
            buffer[n] = '\0';
            msg.assign(buffer);
            cout << msg << endl;
            break;
        }
        }
    } while (!exitChat);
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
}

void writing(int SocketFD)
{
    int n;
    bool send_message = 1; // the client could not send a message if this variable is false
    string msg;
    do
    {
        // Writing user who will receive the message
        cin >> msg;
        int code = validation(msg);
        send_message = 1;
        switch (code)
        {
        case 1: // nickname
            if (!registered)
            {
                msg = MP::process_nickname(msg);
                registered = 1;
            }
            else
            {
                cout << "Already registered" << endl;
                send_message = 0;
            }
            break;
        case 2: // a personal message
            msg = MP::process_message(msg);
            break;
        case 3: // a global message
            msg = MP::process_global_message(msg);
            break;
        case 4: // list of al users
            msg = "L";
            break;
        case 5: // sending a file
            msg = MP::process_file(msg);
            if (msg[0] != 'F')
            { // if the file does not exists
                send_message = 0;
                cout << msg << endl;
            }
            break;
        case 6:
            exitChat = 1;                // we quit the chat
            n = write(SocketFD, "Q", 1); // we send the quit signal
            if (n < 0)
                perror("ERROR writing to socket");
            break;
        default:
            cout << "Error, write your message in the correct form" << endl;
            send_message = 0;
            break;
        }
        if (!exitChat && send_message)
        {
            //cout << "your message: " << msg << endl;
            n = write(SocketFD, msg.c_str(), msg.size());
            if (n < 0)
                perror("ERROR writing to socket");
        }
    } while (!exitChat);
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
}

int main(void)
{
    struct sockaddr_in stSockAddr;
    int Res;
    int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }
    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));
    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(50001);
    Res = inet_pton(AF_INET, "127.0.0.1", &stSockAddr.sin_addr);
    // Validation
    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    /////////////////////////
    // first message of the protocol
    /////////////////////////

    cout << "Instructions:\n";
    cout << "At the start of any message follow this syntax\n";
    cout << "Write \"N:<name>\" to register your nickname\n";
    cout << "Write \"M<name>:<message>\" to send a message to a user\n";
    cout << "Write \"A:<message>\" to send a global message\n";
    cout << "Write \"L\" to see the list of active users\n";
    cout << "Write \"F<name>:<name_of_the_file>\" to send the content of a file to another user\n";
    cout << "Write \"Q\" to quit the chat\n";
    cout << "First you must register your nickname\n"
         << endl;

    thread(reading, SocketFD).detach();
    thread(writing, SocketFD).detach();

    do
    {
    } while (!exitChat);

    return 0;
}

// different codifications