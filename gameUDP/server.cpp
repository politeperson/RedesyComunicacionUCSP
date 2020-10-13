// Server side implementation of UDP client-server model
#include "wormServer.h"

#define PORT 50005
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr, cliaddr;
bool finish_game = 0; // a variable that determines if the game finish or not

// nickname -> <ipaddr, port>
map<char, pair<string, int>> tableUsers;

// nickname -> [(x0,y0),(x1,y1),...,(x6,y6)]

int receiveMsg(string msg, string ipaddr, int port)
{
    int n;
    char buffer[MAXLINE];
    unsigned int len = sizeof(cliaddr);

    switch (msg[0]) //a nickname
    {
    case 'N': // the user sended a login message
    {
        if (tableUsers.empty()) // the users table is empty so, we add this nickname
        {
            // we send the confirmation message
            sendto(sockfd, "O200", MAXLINE,
                   MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                   len);

            // setting the nickname with the ip address and the port
            tableUsers[msg[1]] = make_pair(ipaddr, port);
            cout << "new user: " << msg[1] << endl;
            cout << "ip address: " << ipaddr << endl;
            cout << "port: " << port << endl;
            createPlayer(msg.substr(1)); // we create the player on our table of players

            // now we will send to all our clients the coordinates of the new user
            map<char, pair<string, int>>::iterator it;

            for (it = tableUsers.begin(); it != tableUsers.end(); it++)
            {
                cout << inet_aton(it->second.first.c_str(), &cliaddr.sin_addr) << endl;
                cliaddr.sin_port = it->second.second;

                string newUserMsg = "N";
                newUserMsg.push_back(msg[1]);              // the nickname of the new player
                newUserMsg += sendPlayerPositions(msg[1]); //the positions of the new player
                // we send the positions of the new player
                sendto(sockfd, newUserMsg.c_str(), MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
        }
        else
        {
            // the user was not find on the table
            if (tableUsers.find(msg[1]) == tableUsers.end())
            {
                // first we send the confirmation message
                sendto(sockfd, "O200", MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);

                // now we will send to all our clients the coordinates of the new user
                map<char, pair<string, int>>::iterator it;

                // now we send to the new user, all the users already present on game
                for (it = tableUsers.begin(); it != tableUsers.end(); it++)
                {
                    string userMsg = "N";
                    userMsg.push_back(it->first);              // the nickname of the player
                    userMsg += sendPlayerPositions(it->first); //the positions of the new player
                    // // we send the confirmation message
                    sendto(sockfd, userMsg.c_str(), MAXLINE,
                           MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                           len);
                }

                // setting the nickname with the ip address and the port
                tableUsers[msg[1]] = make_pair(ipaddr, port);

                cout << "new user: " << msg[1] << endl;
                cout << "ip address: " << ipaddr << endl;
                cout << "port: " << port << endl;
                createPlayer(msg.substr(1)); // we create the player on our table of players

                // now we will send all the users, that a new user has entered on game
                for (it = tableUsers.begin(); it != tableUsers.end(); it++)
                {
                    inet_aton(it->second.first.c_str(), &cliaddr.sin_addr);
                    cliaddr.sin_port = it->second.second;
                    string newUserMsg = "N";
                    newUserMsg.push_back(msg[1]);              // the nickname of the new player
                    newUserMsg += sendPlayerPositions(msg[1]); //the positions of the new player
                    // // we send the confirmation message
                    sendto(sockfd, newUserMsg.c_str(), MAXLINE,
                           MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                           len);
                }
            }
            else // we send the error message for registration
            {
                sendto(sockfd, "E400", MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
        }
        break;
    }
    case 'M':
    {
        string move_msg;
        char nick_user = msg[2]; // the nickname of the user
        /*
         response form the server
        // ACTION W(1B)
        // Move Is not accepted  100 // e.g. you hit the wall

        // response from the server when you hit the wall
        // ACTION  W (1B)
        // You hit a wall  600(3B) // you hit the wall
        // Points 99 (2B) // your new live // the server will control the marks
        // --------------
        // response from the server  when you hit a worm
        // ACTION W (1B)
        // You hit a worm 650 (3B)//  you hit a worm
        // Points 99 (2B)
        // ----------------
        // response from the server  when you hit the head of a worm
        // ACTION W (1B)
        // You hit the head of a worm  660 (3B)//
        // Points 99 (2B)

        // we receive
        // Action  M (1B)
        // Move   [UDLRF] (1B)
        // User  J(1B)
        // First we have to check if the worm does not colide with a wall
        */
        string colCode = getCollisionCode(msg);
        // cout << colCode << endl;
        if (colCode == "100") // Move is not accepted
        {
            sendto(sockfd, "W100", MAXLINE,
                   MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                   len);
        }
        else if (colCode == "600") // you hit a wall
        {
            // If you hit the wall, your live will be reduce by 1
            PlayersScores[nick_user]--;
            move_msg = "W600" + istr_n(3, PlayersScores[nick_user]);
            sendto(sockfd, move_msg.c_str(), MAXLINE,
                   MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                   len);
        }
        else
        {
            if (colCode == "650") // you hit a worm
            {
                // if you hit a worm, your live will be reduce by 1
                PlayersScores[nick_user]--;
                move_msg = "W650" + istr_n(3, PlayersScores[nick_user]);
                sendto(sockfd, move_msg.c_str(), MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            else if (colCode == "660") // you hit the head of a worm
            {

                // if you hit the head of a worm you will get 50 points
                PlayersScores[nick_user] += 50;
                if (PlayersScores[nick_user] >= 200) // si el usuario llego a los 200 o más puntos, entonces el ganó el juego
                {
                    finish_game = 1; //finalizamos el juego
                    // now we will send all the users, the new winner
                    map<char, pair<string, int>>::iterator it;
                    for (it = tableUsers.begin(); it != tableUsers.end(); it++)
                    {
                        inet_aton(it->second.first.c_str(), &cliaddr.sin_addr);
                        cliaddr.sin_port = it->second.second;
                        string winUserMsg = "G";         // win message G(1B), J(1B) :the nickname of the winner
                        winUserMsg.push_back(nick_user); // the nickname of the updated user
                        cout << winUserMsg << endl;
                        // we send the new move of the user
                        sendto(sockfd, winUserMsg.c_str(), MAXLINE,
                               MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                               len);
                    }
                }
                else
                {
                    move_msg = "W660" + istr_n(3, PlayersScores[nick_user]);
                    sendto(sockfd, move_msg.c_str(), MAXLINE,
                           MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                           len);
                }
            }
            else // Move is OK
            {
                sendto(sockfd, "W300", MAXLINE,
                       MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                       len);
            }
            if (!finish_game)
            {
                // i will not prohibit the collision of two worms
                // 6) The user will send a move msg to the server (enter, up,down,left,rigth)
                // Action  M (1B)
                // Move   [UDLRF] (1B)
                // User  J(1B)
                updatePlayerPositions(msg); // we update the player position

                // server is going to do broadcast (msg to all)
                // ACTION U (1B) update from the server to users
                // NickName  J (1B)
                // Move [LRUDF] (1B)
                map<char, pair<string, int>>::iterator it;
                // now we will send all the users, the new direction of the user
                for (it = tableUsers.begin(); it != tableUsers.end(); it++)
                {
                    inet_aton(it->second.first.c_str(), &cliaddr.sin_addr);
                    cliaddr.sin_port = it->second.second;
                    string updatedUserMsg = "U";
                    updatedUserMsg.push_back(nick_user); // the nickname of the updated user
                    updatedUserMsg.push_back(msg[1]);    // the new direction of the user

                    // we send the new move of the user
                    sendto(sockfd, updatedUserMsg.c_str(), MAXLINE,
                           MSG_CONFIRM, (struct sockaddr *)&cliaddr,
                           len);
                }
            }
        }
        break;
    }
    default:
        break;
    }
    if (finish_game) // if the game finished
    {
        tableUsers.clear();
        Players.clear();
        PlayersScores.clear();
    }
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
        thread(receiveMsg, msg, address, cliaddr.sin_port).detach();
    }
    tableUsers.clear();
    return 0;
}
