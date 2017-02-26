#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0X501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int _cdecl main(int argc, char *argv[])
{
    WSADATA wsadata;
    int iResult = 0;
    int iSendResult;
    iResult = WSAStartup(MAKEWORD(2,2), &wsadata);
    if(iResult)
    {
        printf("WSAStartup() error\n");
        return -1;
    }

    struct addrinfo *result = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult)
    {
        printf("getaddrinfo error\n");
        WSACleanup();
        return -1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenSocket == INVALID_SOCKET)
    {
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }
    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind error\n");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }
    freeaddrinfo(result);

    if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen error\n");
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }

    SOCKET ClientSocket = INVALID_SOCKET;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if(ClientSocket == INVALID_SOCKET)
    {
        printf("accept error\n");
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }
    closesocket(ListenSocket);

    char sendbuf[] = "Voce esta conectado";
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    do{
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0)
        {
            printf("Bytes recebidos: %d\n", iResult);
            iSendResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
            if(iSendResult == SOCKET_ERROR)
            {
                printf("send error\n");
                closesocket(ClientSocket);
                WSACleanup();
                return -1;
            }
            printf("Bytes enviados: %d\n", iSendResult);
        }
        else if(iSendResult == 0)
        {
            printf("Conexao fechada\n");
        }
        else if(iSendResult < 0)
        {
            printf("recv error\n");
            closesocket(ClientSocket);
            WSACleanup();
            return -1;
        }
    }while(iResult > 0);

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown error\n");
        closesocket(ClientSocket);
        WSACleanup();
        return -1;
    }

    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}
