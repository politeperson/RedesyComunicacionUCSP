#ifndef CLIENT_FILE_H
#define CLIENT_FILE_H 1

#include "headers.h"
#include "message.h" // usaremos la clase message para trabajar con el protocolo

class Client
{
private:
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    Message sendMessage, receiveMessage;
    bool acknowledgment;

public:
    explicit Client(string ipaddress, int PORT)
    {
        acknowledgment = 0;
        struct hostent *host;

        host = (struct hostent *)gethostbyname(ipaddress.c_str()); // host con la dirección ip correspondiente

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

        while (true)
        {
        }
    }

    // enviamos un mensaje que le pasamos como string
    void sendMessage(string msg)
    {
        for (int i = 0; i < 3; ++i)
        {

            sendto(sockfd, msg.c_str(), MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
        }
    }

    // recibimos un mensaje y retornamos un string
    string receiveMessage()
    {
        string msg;
        int n;
        unsigned int len = sizeof(servaddr);
        n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *)&servaddr,
                     &len);
        buffer[n] = '\0';
        msg.assign(buffer);
        return msg;
    }

    // al final cerramos el socket
    ~Client()
    {
        close(sockfd);
    }
};

#endif