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
#include <time.h>

using namespace std;
#define MAX_X 100
#define MAX_Y 45
// player_nick -> [(x0,y0),(x1,y1),...,(x6,y6)]
map<char, vector<pair<int, int>>> Players; //a table with the positions of different Players
// player_nick -> score
map<char, int> PlayersScores;

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

//ACTION  W(1B)
//Move is ok   300 (3B) // you move has been accepted

// response form the server
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
string getCollisionCode(string msg)
{
    map<char, vector<pair<int, int>>>::iterator it;
    it = Players.find(msg[2]); // the 2 position is the nickname
    // the player was encountered, so we verify if there is a collision
    if (it != Players.end())
    {
        char dirPlayer = getPlayerDirection(it->second);
        // we cannot move to the opposite direction of the current one
        if ((dirPlayer == 'U' && msg[1] == 'D') || (dirPlayer == 'D' && msg[1] == 'U'))
            return "100"; // move is not accepted
        if ((dirPlayer == 'R' && msg[1] == 'L') || (dirPlayer == 'L' && msg[1] == 'R'))
            return "100"; // move is not accepted
        // we have to make a copy of the head worm
        pair<int, int> head = it->second[0];
        switch (msg[1]) // we update the copy
        {
        case 'U': // upward
            head.second = head.second - 1;
            break;
        case 'D': // downward
            head.second = head.second + 1;
            break;
        case 'R': // right
            head.first = head.first + 1;
            break;
        case 'L': // left
            head.first = head.first - 1;
            break;
        default: // F: forward
        {
            if (dirPlayer == 'U')
                head.second = head.second - 1;
            else if (dirPlayer == 'D')
                head.second = head.second + 1;
            else if (dirPlayer == 'R')
                head.first = head.first + 1;
            else
                head.first = head.first - 1;
            break;
        }
        }
        // now we check if the new position of the head colides with a wall
        if (head.first == MAX_X - 1 || head.second == MAX_Y - 1 || head.first == 0 || head.second == 0)
            return "600"; // wall collision
        // now we check if the head collides with the head of another worm, or the body of another worm
        for (it = Players.begin(); it != Players.end(); it++)
        {
            for (int j = 0; j < 7; j++)
            {
                if (head == it->second[j])
                {
                    if (j == 0)
                        return "660"; // you hit the head of a worm
                    else
                        return "650"; // you hit a worm
                }
            }
        }
        return "300"; //move is OK
    }
    return "100"; // in theory this will never happen
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
        PlayersScores[msg[0]] = 100; // inicialmente tienes 100 puntos
        map<char, vector<pair<int, int>>>::iterator it;
        it = Players.find(msg[0]); // the 0 position is the nickname
    }
}

// this function will change positions of a worm player in the game
// if it exists, it accept the following message
// 6) The user will send a move msg to the server (enter, up,down,left,rigth)
// Action  M (1B)
// Move   [UDLRF] (1B)
// User  J(1B)
void updatePlayerPositions(string msg)
{
    map<char, vector<pair<int, int>>>::iterator it;
    it = Players.find(msg[2]); // the 2 position is the nickname
    // the player was encountered, so we change its positions
    if (it != Players.end())
    {
        char dirPlayer = getPlayerDirection(it->second);
        // we cannot move to the opposite direction of the current one
        if ((dirPlayer == 'U' && msg[1] == 'D') || (dirPlayer == 'D' && msg[1] == 'U'))
            return;
        if ((dirPlayer == 'R' && msg[1] == 'L') || (dirPlayer == 'L' && msg[1] == 'R'))
            return;
        pair<int, int> head = it->second[0];
        it->second.pop_back(); // we delete the tail coordinates
        switch (msg[1])        // the direction to move
        {
        case 'U': // up
            it->second.insert(it->second.begin(), make_pair(head.first, head.second - 1));
            break;
        case 'D': // down
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
    }
}

// this function will generate a string with the player positions
// [x0y0,x1y1,...,x6y6] of a user in Players table
string sendPlayerPositions(char nick)
{
    map<char, vector<pair<int, int>>>::iterator it;
    it = Players.find(nick);
    // the user was encountered
    if (it != Players.end())
    {
        string positions = "";
        for (int i = 0; i < 7; ++i)
        {
            positions += istr_n(3, it->second[i].first);  // x
            positions += istr_n(3, it->second[i].second); // y
        }
        return positions;
    }
    return ""; // in theory this will never happen on our protocol
}

// sección de las funciones basura
void printPlayers()
{
    if (!Players.empty())
    {
        cout << "printing Players" << endl;

        map<char, vector<pair<int, int>>>::iterator it;
        for (it = Players.begin(); it != Players.end(); ++it)
        {
            cout << it->first << endl;
            cout << it->second[0].first << " " << it->second[0].second << endl;
        }
    }
}
