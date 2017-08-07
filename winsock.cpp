#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include <ctype.h>

void help(char*);
void error(char*);
void setupConnection();
void closeConnection();
hostent *checkAnddress(char*);
SOCKET getConnection(char*);

WSADATA wsadata;

int main(int argc, char *argv[])
{
    if(argc != 2) help(argv[0]);
    setupConnection();

    closeConnection();
    return 0;
}

void help(char *str)
{
    printf("uso: %s ipv4anddress", str);
    printf(" or");
    printf(" %s hostname\n", str);
    exit(1);
}

void setupConnection()
{
    if(WSAStartup(MAKEWORD(2,2), &wsadata) < 0)
    {
        error("WSAStartup()");
    }
}

void closeConnection()
{
    WSACleanup();
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

SOCKET getConnection(char *host)
{
    hostent *host = checkAnddress(argv[1]);
    if(host == NULL)
    {
        error("host null");
    }

    SOCKET sock;
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_addr.s_addr = *(u_long*) host->h_addr_list[0];
    sock_addr.sin_family = host->h_addrtype;
    sock_addr.sin_port = htons(80);

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        error("socket");
    }

    if(connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        error("connect");
    }
    return sock;

}

hostent *checkAnddress(char *host)
{
    struct in_addr server;
    struct hostent *hostaddr;
    DWORD dwError;
    char **aliases;
    int i = 0;
    if(isalpha(host[0]))
    {
        hostaddr = gethostbyname(host);
    }
    else
    {
        server.s_addr = inet_addr(host);
        if(server.s_addr == INADDR_NONE)
        {
            printf("O endereco ipv4 nao e valido\n");
            return NULL;
        }
        else
        {
            hostaddr = gethostbyaddr((char*)&server, 4, AF_INET);
        }
    }

    if(hostaddr == NULL)
    {
        dwError = WSAGetLastError();
        if(dwError != 0)
        {
            if(dwError == WSAHOST_NOT_FOUND)
            {
                printf("Host nao encontrado\n");
                return NULL;
            }
            else if(dwError == WSANO_DATA)
            {
                printf("Nao foi encontrado dado\n");
                return NULL;
            }
            else
            {
                printf("Funcao erro: %ld\n", dwError);
                return NULL;
            }
        }
    }
    else
    {
        printf("Nome oficial %s\n", hostaddr->h_name);
        for(aliases = hostaddr->h_aliases; *aliases != 0; aliases++)
        {
            printf("Nome alternativo #%d: %s\n", ++i, *aliases);
        }
        i = 0;
        printf("Tipo de endereco: ");
        switch(hostaddr->h_addrtype)
        {
        case AF_INET:
            printf("AF_INET\n");
            break;
        case AF_INET6:
            printf("AF_INET6\n");
            break;
        case AF_NETBIOS:
            printf("AF_NETBIOS\n");
            break;
        default:
            printf("%d\n", hostaddr->h_addrtype);
            break;
        }
        printf("Tamanho do endereco %d\n", hostaddr->h_length);

        if(hostaddr->h_addrtype == AF_INET)
        {
            while(hostaddr->h_addr_list[i] != 0)
            {
                server.s_addr = *(u_long*) hostaddr->h_addr_list[i++];
                printf("Endereco IPV4 #%d: %s\n", i, inet_ntoa(server));
            }
        }
        else if(hostaddr->h_addrtype == AF_INET6)
            printf("Endereco IPV6\n");
    }

    return hostaddr;
}
