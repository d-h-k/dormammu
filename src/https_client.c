#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


int main()
{

        // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    // server
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    //char * target = "kakaoenterprise.com";
    // 도메인에서 gethostbyname 이용해야하는데 좀 걸릴듯
    server.sin_addr.s_addr = inet_addr("210.127.253.73");
    server.sin_family = AF_INET;
    server.sin_port = htons(443); // HTTP uses port 80
    
//https://kakaoenterprise.com
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        fprintf(stderr, "[Error] creating socket\n");
        return -1;
    }

    if (connect(server_socket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        fprintf(stderr, "[Error] connecting to server\n");
        return -1;
    }

    // Send the GET request
    char request[] = 
    "GET / HTTP/1.1\r\nHost: kakaoenterprise.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n";
    if (send(server_socket, request, strlen(request), 0) < 0)
    {
        printf("Error sending request\n");
        return -1;
    }

    // Receive the response
    char response[4096];
    int received = 0;
    while ((received = recv(server_socket, response, 4096, 0)) > 0)
    {
        response[received] = '\0';
        printf("%s", response);
    }

    // Close the socket
    close(server_socket);

    return 0;
}