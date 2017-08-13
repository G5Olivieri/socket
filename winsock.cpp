#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include <ctype.h>

int init(int, char**);
void error(char*);
void setupConnection();
void closeConnection();
hostent *checkAnddress(char*);
SOCKET clientConnection(char*, int);
SOCKET serverConnection(int);
int sorc(char*);
int enviar(SOCKET, const char*, size_t);
int receber(SOCKET, char*, size_t);
int salvar(char*, size_t);
WSADATA wsadata;

int main(int argc, char *argv[])
{
    system("cls");
    int n;
    SOCKET sock;
    char buff[2048];

    n = init(argc, argv);
    setupConnection();

    switch(n)
    {
    case 0:
        printf("Criando cliente socket para host '%s' porta '%s'\n", argv[2], argv[3]);
        sock = clientConnection(argv[2], atoi(argv[3]));
        sprintf(buff, "GET / HTTP/1.1\r\n\r\n"
                      "Host: %s\r\n"
                      "Connection: Keep-Alive\r\n"
                      "Cache-Control: max-age=0\r\n"
                      "Upgrade-Insecure-Requests: 1\r\n"
                      "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                      "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.90 "
                      "Safari/537.36\r\n"
                      "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
                      "Accept-Encoding: gzip, deflate, br\r\n"
                      "Accept-Language: pt-BR, pt; q=0.5\r\n", argv[2]);
        enviar(sock, buff, strlen(buff));
        while(receber(sock, buff, 2048)) {
            salvar(buff, 2049);
        }
        break;
    case 1:
        printf("Criando server socket para host '127.0.0.1' porta '%s'\n", argv[2]);
        sock = serverConnection(atoi(argv[2]));
        receber(sock, buff, 1024);
        strcpy(buff, "HTTP/1.1 400 Bad Request\r\n"
                    "Date: Sun, 13 Aug 2017 16:31:07 GMT\r\n"
                    "Server: Apache/2.2.22 (Ubuntu)\r\n"
                    "Vary: Accept-Encoding\r\n"
                    "Content-Length: 307\r\n"
                    "Connection: close\r\n"
                    "Content-Type: text/html; charset=iso-8859-1\r\n\r\n"

                    "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
                    "<html><head>\r\n"
                    "<title>400 Bad Request</title>\r\n"
                    "</head><body>\r\n"
                    "\r\n<h1>Bad Request</h1>\r\n"
                    "<p>Your browser sent a request that this server could not understand.<br />\r\n"
                    "</p>\r\n"
                    "<hr>\r\n"
                    "<address>Apache/2.2.22 (Ubuntu) Server at www.ifsp.edu.br Port 80</address>\r\n"
                    "</body></html>\r\n");
        enviar(sock, buff, strlen(buff));
        break;
    default:
        printf("Erro\n");
        break;
    }
    printf("\nFechando conexao\n");
    closesocket(sock);
    closeConnection();
    return 0;
}

int salvar(char *buff, size_t tamanho)
{
    FILE *arq;
    if(!(arq = fopen("buffer.txt", "ab")))
    {
        error("fopen");
    }
    fwrite(buff, tamanho, 1, arq);
    return 0;
}

int sorc(char *str)
{
    if(!strcmp(str, "-c")) return 0;
    if(!strcmp(str, "-s")) return 1;
    return -1;
}

int init(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("\nuso: %s -c <hostname> <porta>\n", argv[0]);
        printf("\nuso: %s -s <porta>\n\n", argv[0]);
        exit(0);
    }
    switch(sorc(argv[1]))
    {
    case 0:
        if(argc < 4) {
            printf("\nuso: %s -c <hostname> <porta>\n\n", argv[0]);
            exit(0);
        }
        return 0;
    case 1:
        if(argc < 3) {
            printf("\nuso: %s -s <porta>\n\n", argv[0]);
            exit(0);
        }
        return 1;
    default:
        printf("\nuso: %s -c <hostname> <porta>\n", argv[0]);
        printf("\nuso: %s -s <porta>\n\n", argv[0]);
        exit(0);
    }
    return -1;
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

SOCKET clientConnection(char *hostname, int porta)
{
    struct hostent *host = checkAnddress(hostname);

    if(host == NULL)
    {
        printf("\nhost null\n");
        exit(0);
    }

    SOCKET sock;
    struct sockaddr_in sock_addr;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_addr.s_addr = *(u_long*) host->h_addr_list[0];
    sock_addr.sin_family = host->h_addrtype;
    sock_addr.sin_port = htons(porta);

    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        error("socket");
    }

    printf("\nConectando ao servidor '%s' na porta '%d'\n", hostname, porta);

    if(connect(sock, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) < 0)
    {
        error("connect");
    }

    return sock;
}

SOCKET serverConnection(int porta)
{
    struct sockaddr_in server, client;
    SOCKET clie, sock;
    int n = 1;
    if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        error("socket");
    }

    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(porta);
    server.sin_family = AF_INET;

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&n, sizeof(int)) < 0)
    {
        error("setsockopt");
    }

    if(bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        error("bind");
    }

    printf("\nBind na porta '%d' done\n", porta);

    if(listen(sock, SOMAXCONN) < 0)
    {
        error("listen");
    }

    printf("\nAguardando conexao\n");

    n = sizeof(sockaddr_in);
    if((clie = accept(sock, (struct sockaddr*)&client, &n)) < 0)
    {
        error("accept");
    }

    printf("\n'%s' conectado\n", inet_ntoa(client.sin_addr));

    return clie;
}

int enviar(SOCKET sock, const char *str, size_t tamanho)
{
    int n;
    printf("\nEnviando '%s'\n", str);
    if((n = send(sock, str, tamanho, 0)) < 0)
    {
        error("send");
    }
    return n;
}

int receber(SOCKET sock, char *str, size_t tamanho)
{
    int n;
    strcpy(str, "");
    if(( n = recv(sock, str, tamanho, 0))  < 0)
    {
        error("recv");
    }
    str[n] = 0;
    printf("\nRecebido '%s'\n", str);
    return n;
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
                printf("\nHost nao encontrado\n");
                return NULL;
            }
            else if(dwError == WSANO_DATA)
            {
                printf("\nNao foi encontrado dado\n");
                return NULL;
            }
            else
            {
                printf("\nFuncao erro: %ld\n", dwError);
                return NULL;
            }
        }
    }
    else
    {
        printf("\nNome oficial %s\n", hostaddr->h_name);
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
    printf("\n");
    return hostaddr;
}
