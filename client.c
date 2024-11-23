#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib") // Automatically link Winsock library
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket close
#endif

short createSocket(void)
{
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }
#endif

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket creation failed");
    }
    return sock;
}

int main(int argc, char *argv[])
{
    // Check if the correct number of arguments are provided
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        return 1;
    }

    const char *hostname = argv[1]; // First argument: Hostname (e.g., example.com)
    int port = atoi(argv[2]);       // Second argument: Port (e.g., 80)

    // Create socket
    int sock = createSocket();
    if (sock == -1)
    {
        return 1; // Exit if socket creation failed
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Convert the hostname to an IP address
    struct hostent *he = gethostbyname(hostname);
    if (he == NULL)
    {
        perror("gethostbyname failed");
        closesocket(sock);
        return 1;
    }

    server.sin_addr = *((struct in_addr *)he->h_addr);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Connection failed");
        closesocket(sock);
        return 1;
    }

    // Send HTTP GET request
    const char *http_request = "GET / HTTP/1.1\r\n"
                               "Host: example.com\r\n"
                               "Connection: close\r\n\r\n";

    if (send(sock, http_request, strlen(http_request), 0) == -1)
    {
        perror("Send failed");
        closesocket(sock);
        return 1;
    }

    // Receive and display the response
    char response[4096];
    int bytes_received;
    while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0)
    {
        response[bytes_received] = '\0'; // Null-terminate the response
        printf("%s", response);
    }

    if (bytes_received == -1)
    {
        perror("Recv failed");
    }

    // Close the socket
    closesocket(sock);

#ifdef _WIN32
    WSACleanup(); // Clean up Winsock
#endif

    return 0;
}
