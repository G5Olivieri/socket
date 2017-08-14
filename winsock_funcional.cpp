#include <stdio.h>
#include <winsock.h>

int main()
{
    WSADATA wsadata;
    SOCKET sock;
    struct sockaddr_in addr;
    struct hostent  * host = NULL;
    FILE *arq;
    // Incializa winsock
    if(WSAStartup(0x101,&wsadata)==-1)
    {
        perror("WSAStarup");
        return 1;
    }

    // Cria socket
    if((sock = socket(AF_INET,SOCK_STREAM,0))==SOCKET_ERROR)
    {
        perror("socket");
        return 1;
    }

    // Configura socket
    addr.sin_family=AF_INET;
    addr.sin_port = htons(80);

    // Tenta resolver host
    host = gethostbyname("www.ifsp.edu.br");

    if(host == NULL)
    {
        perror("host");
        return 1;
    }

     // Ajusta IP do host
     memcpy(&addr.sin_addr,host->h_addr_list[0],host->h_length);

    // Tenta se conectar
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==SOCKET_ERROR)
    {
      perror("connect");
      return 1;
    }

    // Requisição
    char req[] = "GET / HTTP/1.1\r\nhost: www.ifsp.edu.br\r\n\r\n";
    send(sock,req,strlen(req),0); // Envia requisição

    char buffer[0x200]; // Buffer para armazenar dados

    int bytes = 1; // Bytes recebidos
    if(!(arq = fopen("buffer.txt", "wb")))
        return 1;

    while(bytes > 0) // Enquanto estiver recebendo
    {
        memset(buffer,0x0,0x200); // Limpa o buffer
        bytes = recv(sock,buffer,0x200,0); // Recebe dados
        fprintf(arq, "%s",buffer); // Mostra-os
    }

    fclose(arq);
    closesocket(sock); // Fecha socket
    WSACleanup(); // Libera winsock
    return 0;

}
