#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
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
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
#define MAX_X 100
#define MAX_Y 45

// random numbers generator
random_device rd;
mt19937::result_type seed = rd() ^ ((mt19937::result_type)
                                        chrono::duration_cast<chrono::seconds>(
                                            chrono::system_clock::now().time_since_epoch())
                                            .count() +
                                    (mt19937::result_type)
                                        chrono::duration_cast<chrono::microseconds>(
                                            chrono::high_resolution_clock::now().time_since_epoch())
                                            .count());
char mapa[MAX_Y + 2][MAX_X + 2] = {
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
};
/*
char mapa[MAX_X][MAX_Y] = {
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "X                                                                                                  X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"};
*/
// player_nick -> [(x0,y0),(x1,y1),...,(x6,y6)]
map<char, vector<pair<int, int>>> Players; //a table with the positions of different Players
// player_nick -> points
map<char, int> PlayersPoints;

int getNumDigits(int n)
{
    int digits = 0;
    while (n)
    {
        n /= 10;
        digits++;
    }
    return digits;
}

// string to int
int stri(string num)
{
    stringstream ss;
    ss << num;
    int n;
    ss >> n;
    return n;
}
// int to string
string istr(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

// transform an integer to string with left 0's if the number of digits
// of the number (k) is less than (n)
// e.g. n=3,k=11 -> 011, n=3,k=456 -> 456, n=3, k=1 -> 001
string istr_n(int n, int k)
{
    int n_digits = getNumDigits(k);
    string num = istr(k);
    if (n_digits < n)
        num.insert(0, n - n_digits, '0');
    return num;
}

void printMatrix()
{
    for (int i = 0; i < MAX_Y; ++i)
    {
        cout << mapa[i];
        cout << endl;
    }
}

// esta función la uso para verificar si alguna parte de un gusano está en las
// posiciones enviadas, true si algún gusano posee su cuerpo en esa coordenada
bool isBodyPlayerOnCoord(int x, int y)
{
    map<char, vector<pair<int, int>>>::iterator it;
    for (it = Players.begin(); it != Players.end(); it++)
    {
        for (int j = 0; j < 7; ++j)
            if (it->second[j].first == x && it->second[j].second == y)
                return true;
    }
    return false;
}

// this function draw the body of my worm on the map
void drawPlayerOnMatrix(char nick, vector<pair<int, int>> &pos)
{
    mapa[pos[0].second][pos[0].first] = nick;
    for (int i = 1; i < 7; ++i)
    {
        // the body has #'s
        mapa[pos[i].second][pos[i].first] = '#';
    }
}
// this function will return the direction of a user
// based on the coordinates of his position
char getPlayerDirection(vector<pair<int, int>> &pos)
{
    pair<int, int> fh = pos[0]; //first head
    pair<int, int> sh = pos[1]; //second head

    // if the second head is up of the head
    if (sh.second + 1 == fh.second)
        return 'D'; //down
    else if (sh.second - 1 == fh.second)
        return 'U'; //up
    else if (sh.first + 1 == fh.first)
        return 'R'; //right
    else
        return 'L'; //left
}

// you will receive a message of the form
// [x0,y0,x1,y1,...,x6,y6], with positions on x and y
// and will return the positions on a vector of int pairs
vector<pair<int, int>> getPlayerPositions(string msg)
{
    vector<pair<int, int>> positions;
    int x, y;
    string pair_positions;
    for (int i = 0; i < 7; ++i)
    {
        pair_positions = msg.substr(i * 6, 6);
        x = stri(pair_positions.substr(0, 3));
        y = stri(pair_positions.substr(3));
        pair_positions.clear();
        positions.push_back(make_pair(x, y));
    }
    return positions;
}

// this function will change positions of a worm player in the game
// if it exists, it accept the following message
// server is going to do broadcast (msg to all)
// ACTION U (1B) // update from the server to users
// NickName  J (1B)
// Move [LRUDF] (1B)
void updatePlayerPositions(string msg)
{
    map<char, vector<pair<int, int>>>::iterator it;
    it = Players.find(msg[1]); // the 1 position is the nickname
    // the player was encountered, so we change its positions
    if (it != Players.end())
    {
        char dirPlayer = getPlayerDirection(it->second);
        // we cannot move to the opposite direction of the current one
        if ((dirPlayer == 'U' && msg[2] == 'D') || (dirPlayer == 'D' && msg[2] == 'U'))
            return;
        if ((dirPlayer == 'R' && msg[2] == 'L') || (dirPlayer == 'L' && msg[2] == 'R'))
            return;
        pair<int, int> head = it->second[0];
        // guardamos la última posición del gusano
        int x = it->second.back().first, y = it->second.back().second;

        it->second.pop_back(); // we delete the tail coordinates
        switch (msg[2])        // the direction to move
        {
        case 'U': // upward
            it->second.insert(it->second.begin(), make_pair(head.first, head.second - 1));
            break;
        case 'D': // downward
            it->second.insert(it->second.begin(), make_pair(head.first, head.second + 1));
            break;
        case 'R': // right
            it->second.insert(it->second.begin(), make_pair(head.first + 1, head.second));
            break;
        case 'L': // left
            it->second.insert(it->second.begin(), make_pair(head.first - 1, head.second));
            break;
        default: // F: forward
        {
            if (dirPlayer == 'U')
                it->second.insert(it->second.begin(), make_pair(head.first, head.second - 1));
            else if (dirPlayer == 'D')
                it->second.insert(it->second.begin(), make_pair(head.first, head.second + 1));
            else if (dirPlayer == 'R')
                it->second.insert(it->second.begin(), make_pair(head.first + 1, head.second));
            else
                it->second.insert(it->second.begin(), make_pair(head.first - 1, head.second));
            break;
        }
        }
        // verificamos si la cola del gusano puede ser borrada
        if (!isBodyPlayerOnCoord(x, y))
            mapa[y][x] = ' ';

        // lastly we draw the new body on map
        drawPlayerOnMatrix(it->first, it->second);
    }
}

// this functions will create a new player with its positions, if this is note in the Players table
// it accept the following message
// [K,x0,y0,x1,y1,...,x6,y6], the first letter is the user nick, and
// the positions of the worm in the map
void createPlayer(string msg)
{
    map<char, vector<pair<int, int>>>::iterator it;
    it = Players.find(msg[0]);
    // the player was not encountered, we change its positions
    if (it == Players.end())
    {
        Players[msg[0]] = getPlayerPositions(msg.substr(1));
        PlayersPoints[msg[0]] = 100; // by default the points of the new players is 100
        map<char, vector<pair<int, int>>>::iterator it;
        it = Players.find(msg[0]);                 // the 0 position is the nickname
        drawPlayerOnMatrix(it->first, it->second); // we draw the body of the player on the map
    }
}

// this function will generate the initial position of a player
string generatePlayerPositions()
{
    mt19937 gen(seed);
    uniform_int_distribution<unsigned> distrib(7, MAX_Y - 2); // random values are from 7 to 43
    int x, y;
    x = (int)distrib(gen);
    y = (int)distrib(gen);

    string positions = "", xPos = istr_n(3, x); // three bytes
    positions += xPos;
    positions += istr_n(3, y);
    for (int i = 1; i <= 6; ++i)
    {
        positions += xPos;
        positions += istr_n(3, y - i);
    }

    return positions;
}

// funciónes basura
void printPlayers()
{
    if (!Players.empty())
    {

        map<char, vector<pair<int, int>>>::iterator it;
        for (it = Players.begin(); it != Players.end(); ++it)
        {
            cout << it->first << endl;
            cout << it->second[0].first << " " << it->second[0].second << endl;
        }
    }
}
