#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define SERVER_ADDRESS "127.0.0.1"

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[1024];

    // Crear el socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);

    // Conectar al servidor
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al conectar con el servidor");
        exit(EXIT_FAILURE);
    }

    // Recibir mensaje de bienvenida del servidor
    if (recv(client_socket, message, sizeof(message), 0) == -1) {
        perror("Error al recibir datos del servidor");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", message);

    // Lógica del juego (aquí puedes implementar la interacción del usuario para el juego Battleship)

    // Cerrar conexión
    close(client_socket);

    return 0;
}
