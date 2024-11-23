/**
 * @file server.c
 *
 * @brief Basic HTTP server implementation
 * @version 0.2
 * @date 2024-11-21
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib") // Automatically link Winsock library
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket close
#endif

#define MAX 1024
#define PORT 8001
#define MAX_CLIENTS 10

// Function to read a file
char *readFile(const char *fileName)
{
    FILE *file = fopen(fileName, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *buffer = malloc(fileSize + 1);
    if (!buffer)
    {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

// Function to create a socket
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

// Function to bind a socket
int bindSocket(int socket, int port)
{
    struct sockaddr_in serverAddr = {0};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    return bind(socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
}

int main(void)
{
    char *htmlData = readFile("index.html");
    char url[50]; // Buffer to hold the full URL
    if (!htmlData)
    {
        fprintf(stderr, "Failed to read index.html\n");
        return 1;
    }

    char response[MAX * 2];
    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: %zu\r\n\r\n%s", strlen(htmlData), htmlData);
    free(htmlData);

    int serverSocket = createSocket();
    if (serverSocket == -1)
    {
        return 1;
    }

    if (bindSocket(serverSocket, PORT) < 0)
    {
        perror("Bind failed");
        closesocket(serverSocket);
        return 1;
    }

    printf("Server listening on port %d\n", PORT);
    snprintf(url, sizeof(url), "http://localhost:%d", PORT);
    printf("Server running at: %s\n", url);
    listen(serverSocket, MAX_CLIENTS);

    while (1)
    {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket < 0)
        {
            perror("Client connection failed");
            continue;
        }

        send(clientSocket, response, strlen(response), 0);
        printf(" a Client connected\n");
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
