#ifndef CLIENT_FILE_H
#define CLIENT_FILE_H 1

#include "headers.h"
#include "message.h" // usaremos la clase message para trabajar con el protocolo

#define ACKTime 200

class Client
{
private:
    int sockfd;
    bool listening;
    struct sockaddr_in servaddr;
    char buffer[MAXLINE];
    Message sendedMessage, receivedMessage;
    bool acknowledgment;    // 0 -> no estamos enviando mensaje o no hemos recibido el ack
                            // 1 -> recibimos ack
    int ackCharacteristics; // 1 -> mensaje recibido sin problemas
                            // 2 -> mensaje recibido con errores en datos, segun el hash
                            // 3 -> Time Out Reach
                            // 4 -> Acuse con errores en datos segun hash
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
    }
    void receiveMessage(string message)
    {
        receivedMessage.decode(message);
        unsigned int len = sizeof(servaddr);
        switch (receivedMessage.getType())
        {
        case '9': // response from server send and forget
        {
            // enviamos un acknowledgment con el código 9
            // y los posibles errores que hubiesen
            int currenthash = receivedMessage.hash(receivedMessage.getContent());
            acknowledgment = 1; //indicamos que ya hemos recibido el ack
            if (currenthash != receivedMessage.getHash())
                ackCharacteristics = 4; // 4 -> Acuse con errores en datos segun hash
            else if (receivedMessage.getContent() == "1")
                ackCharacteristics = 1; // 1 -> mensaje recibido sin problemas
            else if (receivedMessage.getContent() == "2")
                ackCharacteristics = 2; // 2 -> mensaje recibido con errores en datos, segun el hash
            break;
        }
        default:
            break;
        }
    }

    void Listen()
    {
        listening = 1;
        int n;
        string message;
        unsigned int len = sizeof(servaddr);
        while (listening)
        {
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&servaddr,
                         &len);
            buffer[n] = '\0';
            message.assign(buffer);
            // soltamos un thread para que se procese el mensaje
            std::thread(&Client::receiveMessage, this, message).detach();
        }
    }

    // espera por el ACK en un intervalo de tiempo en milisegundos
    // retorna true si recibió el ack dentro del intervalo
    // retorna false si no recibió el ack después del intervalo
    bool waitACK(int milli)
    {
        // Cross-platform sleep function
        clock_t end_time;
        end_time = clock() + milli * CLOCKS_PER_SEC / 1000;
        while (clock() < end_time)
            if (acknowledgment)
                return true;
        return false;
    }

    // enviamos un mensaje que le pasamos como string
    string SendAndForget(string message)
    {
        // tres intentos de envío de mensaje
        for (int i = 0; i < 3; ++i)
        {
            sendto(sockfd, sendedMessage.encode(message, '5').c_str(), MAXLINE,
                   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            if (waitACK(ACKTime)) // si llego el ack
                break;
        }
        if (ackCharacteristics == 1)
        {
            return "1";
        }
        else if (ackCharacteristics == 2)
        {
            return "2";
        }
        else if (ackCharacteristics == 4)
        {
            return "3";
        }
        // falta el tiemout reach
        return "1"; // por defecto indicamos que el mensaje llegó sin errores
    }

    // al final cerramos el socket
    ~Client()
    {
        close(sockfd);
    }
};

#endif