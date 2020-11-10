#ifndef SERVER_FILE_H
#define SERVER_FILE_H 1

#include "headers.h"
#include "message.h"

class Server
{
private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    // el mensaje que enviamos y el que recibimos
    map<int, Message> tableOfMessages;
    Message messageSend, messageReceive;

public:
    // para el servidor solo es necesario enviar un puerto
    explicit Server(int PORT)
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
    }

    // esta función permitirá que el servidor empiece a escuchar las solicitudes
    // de los diferentes clientes que le hablen al servidor
    void Listen()
    {
        string message, address;
        int n;
        unsigned int len = sizeof(cliaddr);
        while (true)
        {
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&cliaddr,
                         &len);
            buffer[n] = '\0';
            message.assign(buffer);
            // lanzamos el thread para que maneje el mensaje recibido
            address.clear();
            address = string(inet_ntoa(cliaddr.sin_addr));
            thread(receiveMessage, message, address, cliaddr.sin_port).detach();
        }
    }

    // enviamos un mensaje al cliente que este configurado en nuestro
    void receiveMessage(string message, string ipaddr, int port)
    {
        Message temporalMessage;
        struct sockaddr_in temporalCliaddr;
        memset(&temporalCliaddr, 0, sizeof(temporalCliaddr));

        // decodificamos el mensaje
        temporalMessage.decode(message);

        // Filling server information
        temporalCliaddr.sin_family = AF_INET; // IPv4
        inet_aton(ipaddr.c_str(), &temporalCliaddr);
        temporalCliaddr.sin_port = port;

        switch (messageReceive.getType())
        {
        case '1': // request
            // enviar response

            break;
        case '5':
        {
            // enviamos un acknowledgment con el código 10
            int currenthash = messageReceive.getContent();

            break;
        }
        default:
            cout << "error: anonymous message" << endl;
            break;
        }
        sendto(sockfd, msg.c_str(), MAXLINE,
               MSG_CONFIRM, (const struct sockaddr *)&temporalCliaddr,
               len);
    }

    /*
    // esta función codificará un mensaje del tipo request = 1
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    void request(string message, int IdMessage = -1)
    {
        sendMessage(messageSend.encode(message, Request, IdMessage));
    }
    // esta función codificará un mensaje del tipo response = 2
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string response(string message, int IdMessage = -1)
    {
        TypeOfMessage TypeMessage = Response;
        return encode(message, TypeMessage, IdMessage);
    }
    // esta función codificará un mensaje del tipo Solicitation = 3
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string solicitation(string message, int IdMessage = -1)
    {
        TypeOfMessage TypeMessage = Solicitation;
        return encode(message, TypeMessage, IdMessage);
    }
    // esta función codificará un mensaje del tipo replay = 4
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string replay(string message, int IdMessage = -1)
    {
        TypeOfMessage TypeMessage = Replay;
        return encode(message, TypeMessage, IdMessage);
    }
    // esta función codificará un mensaje del tipo send_and_forget = 5
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string send_and_forget(string message, int IdMessage = -1)
    {
        TypeOfMessage TypeMessage = SendAndForget;
        return encode(message, TypeMessage, IdMessage);
    }/**/
    // esta función codificará un mensaje del tipo notification = 6
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string notification(string message, int IdMessage = -1)
    {
        TypeOfMessage TypeMessage = Notification;
        return encode(message, TypeMessage, IdMessage);
    } /**/
    // al final cerramos el socket
    ~Server()
    {
        close(sockfd);
    }
};

#endif