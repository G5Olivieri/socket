#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iphlpapi.h>

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int _cdecl main(int argc, char *argv[])
{
    // Parte 1 - Montando o ambiente cliente
    // Testando se o usúario digitou o ip do server
    if(argc < 2)
    {
        printf("Uso: %s <ip_server>\n", argv[0]);
        return -1;
    }
    // Inicializando API SOCKET WINSOCK2
    WSADATA wsadata;
    int iResult = 0;
    iResult = WSAStartup(MAKEWORD(2,2), &wsadata);
    if(iResult) // diferente de zero caso ocorra erro
    {
        printf("WSAStartup error");
        return -1;
    }

    // Inicializando valores struct sockaddr através de addrinfo
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    /*   Call the getaddrinfo function requesting the IP address - chama a função requesitando o endereço de ip
     *   for the server name passed on the command line. The TCP port - para o nome do servidor passado pela linha de comando. A porta TCP
     *   on the server that the client will connect to is defined by - do servidor que o cliente conectará é definido por
     *   DEFAULT_PORT as 27015 in this sample. The getaddrinfo function - DEFAULT_PORT como 27015 nesse exemple. A funçao
     *   returns its value as an integer that is checked for errors. - retorna valor como inteiro que é para checar erros
     */
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if(iResult)
    {
         printf("getaddrinfo error");
         WSACleanup();
         return -1;
    }

    // creando socket objeto
    SOCKET ConnectSocket = INVALID_SOCKET;

    /* Call the socket function and return its value to the ConnectSocket variable. - Chama a função socket e para a variavel
    For this application, use the first IP address returned by the call to - nessa aplicação, usa o primeiro ip returnad pela chamada por
    getaddrinfo that matched the address family, socket type, and protocol specified - getaddrinfo que combina o endereço familia e protocolo especificado
    in the hints parameter. In this example, a TCP stream socket was specified with - em hints. nesse examplo, um stream TCP foi especializado com
    a socket type of SOCK_STREAM and a protocol of IPPROTO_TCP. The address family - um socket do tipo SOCK_STREAM e um protocolo de IPROTO_TCP. A familia do endereço
    was left unspecified (AF_UNSPEC), so the returned IP address could be either - ?, então returnou ip address poderia ser outros.
    an IPv6 or IPv4 address for the server. If the client application wants to
    connect using only IPv6 or IPv4, then the address family needs to be set to
    AF_INET6 for IPv6 or AF_INET for IPv4 in the hints parameter.
     */

    /* Tentado conectar a lista de endereços das especificações getaddrinfo */
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        // Creando socket para conectar ao servidor
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        // testando se o socket é válido
        if(ConnectSocket == INVALID_SOCKET)
        {
            printf("Socket error\n");
            WSACleanup();
            return -1;
        }
        // Parte 2 - conectando servidor
        // Call the connect function, passing the created socket and the sockaddr structure as parameters - chama a função connect, passando o socket criado a estrutura sockaddr
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen); // socket, estrutura sockaddr, tamanho da estrutura
        if(iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    // Should really try the next address returned by getaddrinfo - devemos realmente tentar o proximo endereço retornado pela função
    // if the connect call failed - se a connect falhou
    // But for this simple example we just free the resources - mas para esse ecemplo simples nos somente limpamos os resources
    // returned by getaddrinfo and print an error message - retornado pela função e printar uma menssagem de erro
    freeaddrinfo(result);
    if(ConnectSocket == INVALID_SOCKET)
    {
        printf("Erro conexao com servidor");
        WSACleanup();
        return -1;
    }


    // Parte 3 - enviando e recebendo dados
    // buffer de entrada e saida
    int recvbuflen = DEFAULT_BUFLEN;
    char sendbuf[] = "Rola pequena";
    char recvbuf[DEFAULT_BUFLEN];

    // enviando o buffer inicial
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0); // socket, send_buffer, send_buffer_len, IP = 0
    if(iResult == SOCKET_ERROR)
    {
        printf("Send error\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return -1;
    }

    printf("Bytes enviando: %s\n", sendbuf);

    // shutdown the connection for sending since no more data will be sent - Encerrar a conexão para o envio, pois não serão enviados mais dados
    // the client can still use the ConnectSocket for receiving data - do cliente pode ainda usar o ConnectScoekt para receber dados
    iResult = shutdown(ConnectSocket, SD_RECEIVE);
    if(iResult == SOCKET_ERROR)
    {
        printf("shutdown error\n");
        closesocket(ConnectSocket);
        WSACleanup();
        return -1;
    }

    // recebendo dados
    do{
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0)
        {
            printf("Dados recebidos %d\n", iResult);
        }
        else if(iResult == 0)
        {
            printf("Fechando conexao\n");
        }
        else
        {
            printf("recv error\n");
        }
    }while(iResult > 0);


    // Parte 4 - Disconectando cliente
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}
