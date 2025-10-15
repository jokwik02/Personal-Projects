#include "node.h"
#define PORT 1111
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *message = "Hello, Server!";

    // Create the client socket
	sock = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Replace with the server's IP address
	inet_pton(AF_INET, "130.239.42.173", &server_addr.sin_addr);
    // Connect to the server
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Send a message to the server
    send(sock, message, strlen(message), 0);
    printf("Message sent to server\n");

    // Read the response from the server
    read(sock, buffer, BUFFER_SIZE);
    printf("Message from server: %s\n", buffer);

    // Close the socket
    close(sock);

    return 0;
}
