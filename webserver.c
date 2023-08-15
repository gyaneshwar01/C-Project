#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib") // Link with the Winsock library


void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

    // Read the HTTP request from the client 
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == SOCKET_ERROR) {
        perror("Error reading from socket");
        return;
    }

    // Print the received HTTP request
    printf("Received HTTP request:\n%s\n", buffer);

    // Open and read the HTML file
    FILE *html_file = fopen("index.html", "rb");
    if (html_file == NULL) {
        perror("Error opening HTML file");
        send(client_socket, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 37, 0);
        closesocket(client_socket);
        return;
    }

    fseek(html_file, 0, SEEK_END);
    long html_file_size = ftell(html_file); 
    fseek(html_file, 0, SEEK_SET); 


    char *html_content = (char *)malloc(html_file_size + 1);
    if (html_content == NULL) {
        perror("Memory allocation error");
        fclose(html_file);
        send(client_socket, "HTTP/1.1 500 Internal Server Error\r\n\r\n", 37, 0);
        closesocket(client_socket);
        return;
    }

    fread(html_content, 1, html_file_size, html_file);
    fclose(html_file);

    // Respond with the HTML content
    char response_header[BUFFER_SIZE];
    snprintf(response_header, BUFFER_SIZE, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n", html_file_size);
    send(client_socket, response_header, strlen(response_header), 0);
    send(client_socket, html_content, html_file_size, 0);

    free(html_content); 

    // Close the client connection
    closesocket(client_socket);
}


int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    // Initialize Winsock
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsa); 
    if (iResult != 0) {
        perror("Failed to initialize Winsock");
        exit(EXIT_FAILURE);
    }

    // Create the server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (server_socket == INVALID_SOCKET) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);


    // Bind the server socket to the specified address and port 
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Error binding");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        perror("Error listening");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connections
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET) {
            perror("Error accepting connection");
            continue;
        }

        // Handle the client request
        handle_client(client_socket);
    }

    // Close the server socket
    closesocket(server_socket);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
