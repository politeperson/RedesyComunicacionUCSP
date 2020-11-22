#ifndef SERVER_FILE_H
#define SERVER_FILE_H 1

#include "headers.h"
#include "message.h"

class Server
{
private:
    int sockfd;
    // esta variable me indicará cuando es que el servidor debe dejar de escuchar
    // por los mensajes recibidos
    bool listening;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[MAXLINE];
    // una tabla de mensajes, para que no haya duplicación de datos
    // id del mensaje -> el mensaje
    map<int, Message> tableOfMessages;

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

    // enviamos un mensaje al cliente que este configurado en nuestro
    void receiveMessage(string message, string ipaddr, int port)
    {
        Message temporalMessage;
        struct sockaddr_in temporalCliaddr;
        memset(&temporalCliaddr, 0, sizeof(temporalCliaddr));

        // decodificamos el mensaje
        temporalMessage.decode(message);

        // registramos el mensaje en nuestra tabla general
        // si el mensaje no fue registrado en la tabla, eso me indica que es otro mensaje con el mismo id
        // por lo que no debemos procesarlo
        if (!this->registerMessage(temporalMessage))
            return;

        // Filling server information
        temporalCliaddr.sin_family = AF_INET; // IPv4
        inet_aton(ipaddr.c_str(), &temporalCliaddr.sin_addr);
        temporalCliaddr.sin_port = port;
        unsigned int len = sizeof(temporalCliaddr);

        // cuando recibimos un mensaje del cliente siempre debemos reenviar un ack
        // para indicar que hemos recibido el mensaje
        string acknowledgment;
        switch (temporalMessage.getType())
        {
        case '1': // request
            // enviar response

            break;
        case '5': // send and forget cli->ser
        {
            // enviamos un acknowledgment con el código 9
            // y los posibles errores que hubiesen
            // verificamos que no haya corrupción de datos
            int currenthash = temporalMessage.hash(temporalMessage.getContent());
            if (currenthash == temporalMessage.getHash())
            {
                // los dos hash son iguales, por lo tanto no hay errores
                // enviaremos el código 1 que significa que el mensaje llegó sin errores
                acknowledgment = temporalMessage.encode("1", '9', temporalMessage.getId());
                sendto(sockfd, acknowledgment.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&temporalCliaddr,
                       len);
            }
            else
            {
                // la data fue corrompida, por lo que debemos enviar el código de error 2 (data corruption)
                // reenviamos el mensaje de tipo 9, con el mismo código del mensaje recibido
                // y el error en la data
                acknowledgment = temporalMessage.encode("2", '9', temporalMessage.getId());
                sendto(sockfd, acknowledgment.c_str(), MAXLINE,
                       MSG_CONFIRM, (const struct sockaddr *)&temporalCliaddr,
                       len);
            }
            break;
        }
        default:
            cout << "error: anonymous message" << endl;
            break;
        }
    }

    // esta función permitirá que el servidor empiece a escuchar las solicitudes
    // de los diferentes clientes que le hablen al servidor
    void Listen()
    {
        listening = 1;
        string message, address;
        int n;
        unsigned int len = sizeof(cliaddr);
        while (listening)
        {
            n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                         MSG_WAITALL, (struct sockaddr *)&cliaddr,
                         &len);
            buffer[n] = '\0';
            message.assign(buffer);
            // lanzamos el thread para que maneje el mensaje recibido
            address.clear();
            address = string(inet_ntoa(cliaddr.sin_addr));
            // soltamos un thread para que se procese el mensaje
            std::thread(&Server::receiveMessage, this, message, address, cliaddr.sin_port).detach();
        }
    }

    // retorna true si el mensaje fue encontrado
    // false si no fue encontrado
    bool findMessage(int idMessage)
    {
        if (tableOfMessages.empty())
            return false;
        if (tableOfMessages.find(idMessage) != tableOfMessages.end())
            return false;
        return true;
    }
    // this function is used to store a message by its id on the table of messages
    bool registerMessage(Message &message) // passing by reference is more fast
    {
        if (!findMessage(message.getId()))
        {
            // insertamos el mensaje
            tableOfMessages.insert(make_pair(message.getId(), message));
            return true; // el mensaje fue insertado, no estaba en la tabla de mensajes
        }
        else
            return false; //el mensaje fue encontrado por lo tanto no fue insertado
    }

    // esta función es utilizada para  eliminar un mensaje recibido de la tabla de mensajes
    bool deleteMessage(int idMessage)
    {
        // si el mensaje no fué encontrado, no hay nada que eliminar
        if (!findMessage(idMessage))
            return false;
        tableOfMessages.erase(idMessage);
        return true;
    }

    // esta función enviará un mensaje a un cliente especificado
    // con su dirección ip y su puerto
    // bool sendMessage(string message, string ipaddr, int port)
    // {
    // }

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
    }/*
    // esta función codificará un mensaje del tipo notification = 6
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string notification(string message, int IdMessage = -1)
    {
        return encode(message, '6', IdMessage);
    } /**/
    // al final cerramos el socket
    ~Server()
    {
        close(sockfd);
    }
};

#endif