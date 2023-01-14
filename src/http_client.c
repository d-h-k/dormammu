#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // server
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_addr.s_addr = inet_addr("www.kakaoenterprise.com");
    server.sin_family = AF_INET;
    server.sin_port = htons(80); // HTTP uses port 80
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
    char request[] = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
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