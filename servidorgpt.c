#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_CONNECTIONS 2

int main() {
    int server_socket, client_socket[MAX_CONNECTIONS];
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len;
    char message[] = "¡Bienvenido al juego Battleship!";

    // Crear el socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Vincular el socket a la dirección y al puerto
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error al vincular el socket");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        perror("Error al escuchar conexiones");
        exit(EXIT_FAILURE);
    }

    printf("Esperando a los jugadores...\n");

    // Aceptar conexiones de dos clientes
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        client_address_len = sizeof(client_address);
        client_socket[i] = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket[i] == -1) {
            perror("Error al aceptar la conexión");
            exit(EXIT_FAILURE);
        }
        printf("Jugador %d conectado\n", i + 1);

        // Enviar mensaje de bienvenida al cliente
        send(client_socket[i], message, strlen(message), 0);
    }

    // Lógica del juego (aquí puedes implementar el juego Battleship)
    while (1) {
        for (int i = 0; i < MAX_CONNECTIONS; ++i) {
            recv(client_sockets[i], cs, 1024, 0);
            printf("El cliente dijo: %s\n", cs);
            // Aquí puedes procesar el mensaje recibido del cliente
        }
    }

    // Cerrar conexiones
    for (int i = 0; i < MAX_CONNECTIONS; ++i) {
        close(client_socket[i]);
    }
    close(server_socket);

    return 0;
}
