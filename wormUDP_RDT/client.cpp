// Client side implementation of UDP client-server model
#include "wormClient.h"

#define PORT 50005
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;
bool exitChat = 0;
bool registered = 0;
char nickname, winner_nick; // the nickname of the user and the nickname of the winner
bool game_changed = 0;
bool exit_game = 0;
bool ACK = 0;

void sleepcp(int milli)
{
    // Cross-platform sleep function
    clock_t end_time;
    end_time = clock() + milli * CLOCKS_PER_SEC / 1000;
    while (clock() < end_time)
    {
        if (ACK)
            return;
    }
}

int receiveMsg()
{
    int n;
    string msg;
    char buffer[MAXLINE];
    unsigned int len = sizeof(servaddr);
    while (!exit_game)
    {
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        msg.assign(buffer);
        // reenviamos el ack
        sendto(sockfd, "ACK", MAXLINE,
               MSG_CONFIRM, (const struct sockaddr *)&servaddr,
               len);
        if (msg == "ACK")
        {
            ACK = 1;
        }
        else
        {
            switch (msg[0])
            {
            case 'O':
            {
                registered = 1;
                break;
            }
            case 'E':
                cout << "nickname already in use" << endl;
                break;
            case 'N':
            {
                createPlayer(msg.substr(1));
                game_changed = 1;
                break;
            }
            case 'W':
            {

                string code = msg.substr(1, 3);
                if (code == "600") // you hit a wall
                {
                    // message received
                    // W (1B)
                    // 600 (3B)
                    // XXX (3B) the new score
                    PlayersPoints[nickname] = stri(msg.substr(4));
                }
                else if (code == "650") // you hit a worm
                {
                    // message received
                    // W (1B)
                    // 650 (3B)
                    // XXX (3B) the new score
                    PlayersPoints[nickname] = stri(msg.substr(4));
                }
                else if (code == "660") // you hit the head of the worm
                {
                    // message received
                    // W (1B)
                    // 660 (3B)
                    // XXX (3B) the new score
                    PlayersPoints[nickname] = stri(msg.substr(4));
                }
                game_changed = 1; // we say that the table game has changed
                // else is an invalid movement so we do nothing
                break;
            }
            case 'U':
            {

                // server is going to do broadcast (msg to all)
                // ACTION U (1B) update from the server to users
                // NickName  J (1B)
                // Move [LRUDF] (1B)
                updatePlayerPositions(msg); // we update the player position
                game_changed = 1;
                break;
            }
            case 'G':
            {
                exit_game = 1;
                winner_nick = msg[1];
                break;
            }
            default:
                break;
            }
        }
    }

    return 0;
}

void playgame()
{
    int n;
    string msg;
    char buffer[MAXLINE];
    unsigned int len = sizeof(servaddr);

    while (!registered)
    {
        cout << "Enter a letter to register: ";
        cin >> nickname;
        string msg_register = "N";
        msg_register.push_back(nickname);
        msg_register += generatePlayerPositions();
        do
        {
            sendto(sockfd, msg_register.c_str(), MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            sleepcp(300);
        } while (!ACK);
        ACK = 0; // restart the ACK variable
    }

    while (!exit_game)
    {
        cin >> msg;
        string move_msg = "M";
        transform(msg.begin(), msg.end(), msg.begin(), ::toupper);

        if (msg == "W") // move to up
            move_msg += "U";
        else if (msg == "S") //move to down
            move_msg += "D";
        else if (msg == "D") // move to right
            move_msg += "R";
        else if (msg == "A") //move to left
            move_msg += "L";
        else if (msg == "F")
            move_msg += "F";
        else
            move_msg = "NOT_MOVE";
        // if the movement is a valid one
        if (move_msg != "NOT_MOVE")
        {
            move_msg.push_back(nickname); // ok hasta ahora
            do
            {
                sendto(sockfd, move_msg.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                       len);
                sleepcp(300);
            } while (!ACK);
            ACK = 0; // restart the ACK variable
        }
    }
}

void printMapa()
{
    while (!exit_game)
    {
        if (game_changed)
        {
            system("clear");
            printMatrix();
            cout << "User " << nickname << " current life: " << PlayersPoints[nickname] << endl;
            game_changed = 0;
        }
    }
}

// Driver code
int main()
{
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

    thread(receiveMsg).detach();
    thread(playgame).detach();
    thread(printMapa).detach();
    while (!exit_game)
    {
    }
    // we clean all the tables
    Players.clear();
    PlayersPoints.clear();
    char FINISH;
    system("clear");
    if (winner_nick == nickname)
        cout << "You win the game" << endl;
    else
        cout << "User " << winner_nick << " won the game" << endl;

    cout << "Enter a letter to exit: ";
    cin >> FINISH;
    close(sockfd);
    return 0;
}
