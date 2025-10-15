#include "node.h"
#define PORT 1111
#define BUFFER_SIZE 1024

int main (){
	int server_fd, client_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *response = "Hello, Client!";

    // Create the server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
     bind(server_fd, (struct sockaddr *)&address, sizeof(address));
  

    listen(server_fd, 3);

    printf("Server is listening on port %d\n", PORT);

    // Accept a connection from a client
	client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addr_len);

    // Read data sent by the client
    read(client_socket, buffer, BUFFER_SIZE);
    printf("Message from client: %s\n", buffer);

    // Send a response to the client
    send(client_socket, response, strlen(response), 0);
    printf("Response sent to client\n");

    // Close sockets
    close(client_socket);
    close(server_fd);

    return 0;
}