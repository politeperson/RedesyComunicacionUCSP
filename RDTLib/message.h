#ifndef MESSAGE_FILE_H
#define MESSAGE_FILE_H 1

#include "headers.h"

#define MAXIDNUMBER 65353 // el número de ID que puedo generar que puedo generar

// la clase la utilizaré para implementar las reglas del protocolo
// habrá funciones de codificación y decodificación de un mensaje
class Message
{
private:
    // esta variable es estática ya que la utilizaré en la generación de varios mensajes
    static int IdCounter;
    int IdLength, MessageLength, HashKey;
    string content;
    int Id, hash;
    char Type;

public:
    Message()
    {
        HashKey = 666;
        IdLength = 5;
        MessageLength = 3;
        Id = -1;      // inicialmente el mensaje no tiene identificador
        Type = '1';   // el mensaje será de tiop request por defecto
        content = ""; //el contenido vacío por defecto
    }
    // esta función incrementará el identificador de mensaje que enviemos, todo esto
    // con el objetivo de que ningún mensaje enviado repita identificadores
    void increase_id_counter()
    {
        IdCounter = (IdCounter + 1) % (MAXIDNUMBER + 1);
        if (IdCounter == 0)
            IdCounter = 1;
    }
    // la función retorna el contenido de mi variable privada
    string getContent()
    {
        return content;
    }
    // la función retorna el contenido de mi variable Id
    int getId()
    {
        return Id;
    }
    // la función retorna el tipo de mensaje
    char getType()
    {
        return Type;
    }
    // la función retorna el contenido de mi variable privada
    void setContent(string _content)
    {
        content.clear();
        content.assign(_content.begin(), _content.end());
    }
    // obtnenemos el hash
    int getHash()
    {
        return hash;
    }
    // la función retorna el contenido de mi variable Id
    void setId(int _Id)
    {
        Id = _Id;
    }
    // la función retorna el tipo de mensaje
    void setType(char _Type)
    {
        Type = _Type;
    }

    // estas funciones me serán de ayuda para la codificación y decodificación
    // convertimos de tipo entero a string
    string istr(int n)
    {
        stringstream ss;
        ss << n;
        return ss.str();
    }
    // convertimos de tipo string a entero
    int stri(string n)
    {
        stringstream ss;
        ss << n;
        int k;
        ss >> k;
        return k;
    }
    // transform an integer to string with left 0's if the number of digits
    // of the number (k) is less than (n)
    // e.g. n=3,k=11 -> 011, n=3,k=456 -> 456, n=3, k=1 -> 001
    string istr_n(int n, int k)
    {
        int n_digits = (int)floor(log10(k) + 1);
        string num = istr(k);
        if (n_digits < n)
            num.insert(0, n - n_digits, '0');
        return num;
    }

    // hash function, receive a string makes a sum of his weights and return
    // the modulo of that number with our hashkey
    int hash(string message)
    {
        int sum = 0;
        for (int i = 0; i < message.size(); sum += ((int)message[i++]) % HashKey)
            ;
        return sum;
    }
    // esta función decodificará un mensaje que utilice el protocolo
    // retornará el mensaje recibido, el tipo de mensaje que es y el identificador
    // del mensaje
    string decode(string message)
    {
        // primero extraemos el tipo de mensaje que es
        Type = message[0];

        // segundo extraemos el identificador del mensaje
        setId(stri(message.substr(1, IdLength)));

        // tercero extraemos el tamaño del mensaje
        int data_size = stri(message.substr(1 + IdLength, MessageLength));

        // cuarto extraemos el mensaje y retornamos el resultado
        setContent(message.substr(1 + IdLength + MessageLength, data_size));
        return content;
    }

    // esta función codificará el mensaje que enviemos, con un tipo definido
    // además de un ID por defecto, si es -1 entonces le generamos un nuevo identificador
    // sino codificamos el mensaje con el identificador enviado
    string encode(string message, char TypeMessage, int IdMessage = -1)
    {
        // asignamos el contenido a nuestra variable local
        setContent(message);
        // también asignamos el tipo de mensaje a codificar y lo asignamos
        setType(TypeMessage);
        // también asignamos el identificador a nuestro mensaje
        setId(IdMessage);

        string EncodedMessage = "";
        // primero se agrega el tipo de mensaje
        EncodedMessage.push_back(TypeMessage);

        // segundo se agrega el identificador del mensaje
        // MessageID (5B) por defecto
        // si no hemos especificado un id, generamos uno y lo agregamos al mensaje
        // sino le agregamos el id especificado para el mensaje
        EncodedMessage += (IdMessage == -1) ? istr_n(IdLength, IdCounter) : istr_n(IdLength, IdMessage);
        increase_id_counter(); // incrementamos el contador de identificadores, para otro id

        // tercero se agrega el tamaño del mensaje
        // Data_size (3B) por defecto
        EncodedMessage += istr_n(3, message.size());

        // cuarto se agrega el mensaje
        EncodedMessage += message;

        // quinto se agrega el hash dado por la fórmula
        // HASH = SUM(Message) % 666, donde SUM(message) es la suma de los valores enteros de cada
        // letra en el mensaje
        EncodedMessage += istr_n((int)floor(log10(HashKey) + 1), hash(message));

        // sexto agregamos el "padding" o en castellano "relleno" de solo 0's
        EncodedMessage += string(MAXLINE - EncodedMessage.size(), '0');

        return EncodedMessage;
    }

    ~Message()
    {
    }
};

int Message::IdCounter = 1;
#endif
